/*
 * Wrapper.cpp
 *
 *  Created on: Nov 23, 2020
 *      Author: chris
 */

#include <Wrapper.h>
#include "Lib.h"
#include "Builtins.h"

/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
/* == == == == == WValue Impl == == == == == */
/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
WValue::WValue(Value* llv, WType* llt) {
	assert(llv->getType()->isPointerTy());
	if (llt == nullptr)
		type = WType::getTy(llv->getType()->getPointerElementType());
	else type = llt;
	storage = llv;
}
WValue::WValue(WType* type): type(type) {
	assert("Cannot create void value!" && !type->isVoidTy());
	storage = LLBuilder.CreateAlloca(type->getLLType());
}

WValue::WValue(WType* elemType, ArrayRef<WValue*> arrayDims) {
	assert(arrayDims.size() > 0);
	if (arrayDims.size() == 1) {
		assert(arrayDims[0]->getType()->canImplicitCastTo(WType::getIntTy()));
		WValue* arrSize = *arrayDims[0]->castTo(WType::getIntTy());
		Value* arrdata = LLBuilder.CreateAlloca(elemType->getLLType(), arrSize->loadLL());
		type = WType::getArrayTy(elemType);
		storage = LLBuilder.CreateAlloca((*type)->getLLType());
		Value* len_gep = LLBuilder.CreateGEP(getStorage(), {
				ConstantInt::get(Type::getInt64Ty(LLContext), 0), //Base Ptr
				ConstantInt::get(Type::getInt32Ty(LLContext), 0) //len idx
		});
		LLBuilder.CreateStore(arrSize->loadLL(), len_gep);
		Value* ptr_gep = LLBuilder.CreateGEP(getStorage(), {
				ConstantInt::get(Type::getInt64Ty(LLContext), 0), //Base Ptr
				ConstantInt::get(Type::getInt32Ty(LLContext), 1) //arr ptr idx
		});
		LLBuilder.CreateStore(arrdata, ptr_gep);
	} else {
		//Multidimensional array
		assert(arrayDims.back()->getType()->canImplicitCastTo(WType::getIntTy()));
		WValue* arrSize = *arrayDims.back()->castTo(WType::getIntTy());
		//Create the outer array object
		type = elemType;
		for (int i = 0; i < arrayDims.size(); i++)
			type = WType::getArrayTy(*type);
		Value* arrdata = LLBuilder.CreateAlloca((*type)->getLLType(), arrSize->loadLL());
		storage = LLBuilder.CreateAlloca((*type)->getLLType());
		Value* len_gep = LLBuilder.CreateGEP(getStorage(), {
				ConstantInt::get(Type::getInt64Ty(LLContext), 0), //Base Ptr
				ConstantInt::get(Type::getInt32Ty(LLContext), 0) //len idx
		});
		LLBuilder.CreateStore(arrSize->loadLL(), len_gep);
		Value* ptr_gep = LLBuilder.CreateGEP(getStorage(), {
				ConstantInt::get(Type::getInt64Ty(LLContext), 0), //Base Ptr
				ConstantInt::get(Type::getInt32Ty(LLContext), 1) //arr ptr idx
		});
		LLBuilder.CreateStore(arrdata, ptr_gep);

		//Construct what's needed for the sub-array
		//We have to create a mini for-loop in order to construct however many arrays we need (all on the stack)
		Function* LF = LLBuilder.GetInsertBlock()->getParent();
		BasicBlock* Head = BasicBlock::Create(LLContext, "", LF);
		BasicBlock* Loop = BasicBlock::Create(LLContext, "", LF);
		BasicBlock* Merge = BasicBlock::Create(LLContext, "", LF);
		//Head!
		LLBuilder.CreateBr(Head);
		LLBuilder.SetInsertPoint(Head);
		//i = 0
		WValue* iter_val = new WValue(WType::getIntTy());
		LLBuilder.CreateStore(ConstantInt::get(Type::getInt32Ty(LLContext), 0), iter_val->getStorage());
		//Loop!
		LLBuilder.CreateBr(Loop);
		LLBuilder.SetInsertPoint(Loop);
		//Allocate the sub-value object
		WValue* subVal = new WValue(elemType, ArrayRef(arrayDims.begin(), arrayDims.size() - 1));
		//Store the subarray object into the array being generated
		Value* idx_gep = LLBuilder.CreateGEP(arrdata, iter_val->loadLL());
		LLBuilder.CreateStore(subVal->loadLL(), idx_gep);
		//i++
		LLBuilder.CreateStore(
				LLBuilder.CreateAdd(iter_val->loadLL(), ConstantInt::get(Type::getInt32Ty(LLContext), 1)),
				iter_val->getStorage());
		//if i < arraySize, continue
		Value* inRange = LLBuilder.CreateICmpSLT(iter_val->loadLL(), arrSize->loadLL());
		LLBuilder.CreateCondBr(inRange, Loop, Merge);
		//Merge!
		Merge->moveAfter(LLBuilder.GetInsertBlock());
		LLBuilder.SetInsertPoint(Merge);
	}
}
WType* WValue::getType() {
	if (type) return *type;
	Type* lt = (*storage)->getType()->getPointerElementType();
	if (lt->isIntegerTy(32))
		type = WType::getIntTy();
	if (lt->isIntegerTy(1))
		type = WType::getBoolTy();
	if (lt->isDoubleTy())
		type = WType::getDoubleTy();
	assert("Type cannot be void!" && !lt->isVoidTy());
	if (!type)
		fatal("Type not known!");
	return *type;
}
bool WValue::isWType() {
	return getType()->isTypeTy();
}
bool WValue::isWFunction() {
	return getType()->isFuncTy() || getType()->isGenericTy();
}
WType* WValue::asWType() {
	assert(getType()->isTypeTy());
	return static_cast<WType*>(this);
}
WFunction* WValue::asWFunction() {
	assert(getType()->isFuncTy() || getType()->isGenericTy());
	return static_cast<WFunction*>(this);
}
Value* WValue::getStorage() {
	if (!storage)
		fatal("Nothing to get!");
	assert((*storage)->getType()->isPointerTy());
	return *storage;
}
Value* WValue::loadLL() {
	Value* A = getStorage();
	return LLBuilder.CreateLoad(A);
}

std::optional<WValue*> WValue::castTo(WType* to) {
	if (!storage) return std::optional<WValue*>(); //Can't cast if no storage to cast with!
	if (to == *type) return this;
	if ((*type)->isRefTy()) {
		//I'm a ref type!
		if (to->isRefTy()) {
			return std::optional<WValue*>(); //TODO class inheritance
		} else {
			if ((*type)->getTypeRefd()->canImplicitCastTo(to)) {
				WValue* nwv = new WValue(LLBuilder.CreateLoad(getStorage()), (*type)->getTypeRefd());
				return nwv->castTo(to);
			}
			return std::optional<WValue*>();
		}
	} else {
		//I'm not a ref type.
		if (to->isRefTy()) return std::optional<WValue*>();
		else {
			auto& vec = BuiltinOps["I()"];
			for (WFunction* wf: vec) {
				if (wf->getArgTy(0) == this->getType() &&
					wf->getRetTy() == to)
					return wf->call({this});
			}
			return std::optional<WValue*>();
		}
	}
}
/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
/* == == == == ==  WType Impl == == == == == */
/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
/* WType Static Values */
WType WType::IntType(Type::getInt32Ty(LLContext));
WType WType::BoolType(Type::getInt1Ty(LLContext));
WType WType::DoubleType(Type::getDoubleTy(LLContext));
WType WType::ByteType(Type::getInt8Ty(LLContext));
WType WType::VoidType;
WType WType::TypeType;
WType WType::GenericType;
std::map<std::vector<WType*>, std::map<WType*, WType*>> WType::FunctionTypes;
std::map<WType*, WType*> WType::RefTypes;
std::map<WType*, WType*> WType::ArrTypes;
/* WType Constructors */
//Function Type
WType::WType(WType* retType, ArrayRef<WType*> args): functionProperties({retType, args}) {
	std::vector<Type*> intArgs;
	for (WType* argT: args)
		intArgs.push_back(argT->getLLType()->getPointerTo());
	if (retType->isVoidTy()) {
		llType = FunctionType::get(Type::getVoidTy(LLContext), intArgs, false);
	} else llType = FunctionType::get(retType->getLLType(), intArgs, false);
}
//Array/Reference Type
WType::WType(WType* t, int arrDims) {
	if (arrDims == 0) {
		RefdType = t;
		llType = t->getLLType()->getPointerTo();
	} else {
		containedArrType = t;
		this->arrDepth = arrDims;
		llType = getLLArrayType(t, arrDims);
		return;
	}
}
//Class Type
WType::WType(std::string name): className(name) {
	StructType* st = StructType::create(LLContext, name);
	st->setName(name);
	llType = st;
}
/* == == WType primary methods == == */
Type* WType::getLLType() {assert("llType does not exist for type!" && (bool) llType); return *llType;}

WType* WType::getType() {
	return &TypeType;
}

void WType::dump() {
	if (isIntTy())
		printf("int\n");
	if (isBoolTy())
		printf("bool\n");
	if (isDoubleTy())
		printf("double\n");
	if (isVoidTy())
		printf("void\n");
	printf("Other type\n");
}
/* == == WType::is*() methods == == */
bool WType::isIntTy() {return this == getIntTy();}
bool WType::isBoolTy() {return this == getBoolTy();}
bool WType::isDoubleTy() {return this == getDoubleTy();}
bool WType::isByteTy() {return this == getByteTy();}
bool WType::isVoidTy() {return this == getVoidTy();}
bool WType::isTypeTy() {return this == &TypeType;}
bool WType::isGenericTy() {return this == &GenericType;}
bool WType::isFuncTy() {return (bool) functionProperties;}
bool WType::isRefTy() {return (bool) RefdType;}
bool WType::isClassTy() {return (bool) className;}
bool WType::isArrayTy() {return (bool) containedArrType;}
/* == == WType::get*() methods == == */
WType* WType::getTy(Type* llt) {
	if (llt->isIntegerTy(32))
		return getIntTy();
	if (llt->isIntegerTy(1))
		return getBoolTy();
	if (llt->isDoubleTy())
		return getDoubleTy();
	if (llt->isVoidTy())
		return getVoidTy();
	//TODO handle ptr types and ref's
	fatal("Unable to resolve type!");
}
WType* WType::getIntTy() {return &IntType;}
WType* WType::getBoolTy() {return &BoolType;}
WType* WType::getDoubleTy() {return &DoubleType;}
WType* WType::getByteTy() {return &ByteType;}
WType* WType::getGenericTy() {return &GenericType;}
WType* WType::getVoidTy() {return &VoidType;}
WType* WType::getRefTo() {return getRefTy(this);}
WType* WType::getFuncTy(WType* retType, ArrayRef<WType*> argTypes) {
	auto& p1 = FunctionTypes[argTypes];
	WType* ft = p1[retType];
	if (ft) return ft;
	return p1[retType] = new WType(retType, argTypes);
}
WType* WType::getRefTy(WType* toRef) {
	WType* wv = RefTypes[toRef];
	if (wv) return wv;
	return RefTypes[toRef] = new WType(toRef, false);
}
WType* WType::getClassTy(std::string name) {
	return new WType(std::move(name));
}
WType* WType::getArrayTy(WType* arrElem) {
	WType* wv = ArrTypes[arrElem];
	if (wv) return wv;
	return ArrTypes[arrElem] = new WType(arrElem, true);
}
/* == == WType function type methods == == */
ArrayRef<WType*> WType::getFnArgs() {
	assert("Not a function!" && functionProperties);
	return functionProperties->second;
}
WType* WType::getFnArgTy(int n) {
	assert("Not a function!" && functionProperties);
	assert(n >= 0 && n < functionProperties->second.size());
	return functionProperties->second[n];
}
WType* WType::getFnRetTy() {
	assert("Not a function!" && functionProperties);
	return functionProperties->first;
}
size_t WType::getFnArgCt() {
	assert("Not a function!" && functionProperties);
	return functionProperties->second.size();
}
std::optional<FunctionType*> WType::getLLFnType() {
	if (functionProperties)
		return static_cast<FunctionType*>(getLLType());
	return std::optional<FunctionType*>();
}
/* == == WType ref type methods == == */
WType* WType::getTypeRefd() {
	assert("Not a ref type!" && isRefTy());
	return *RefdType;
}
/* == == WType class type methods == == */
void WType::setClassContents(std::map<std::string, WValue*>& mp) {
	assert("Not a class!" && isClassTy());
	assert("Class Contents already set!" && !classContents);
	classContents = mp;
	std::vector<Type*> llTypeList;
	for (auto& pair: mp) {
		if (pair.second->getType()->isTypeTy())
			llTypeList.push_back(pair.second->asWType()->getLLType());
	}
	assert(llTypeList.size() > 0);
	StructType* st = static_cast<StructType*>(*llType);
	st->setBody(llTypeList);
}
const std::map<std::string, WValue*>& WType::getClassContents() {
	assert(classContents);
	return *classContents;
}
std::optional<std::pair<int, WType*>> WType::getVar(StringRef varName) {
	assert("Not a class!" && (bool) classContents);
	int i = 0;
	for (auto& pair: *classContents) {
		if (pair.second->getType()->isTypeTy()) {
			if (pair.first == varName) {
				return std::pair<int, WType*>(i, pair.second->asWType());
			}
			i++;
		}
	}
	return std::optional<std::pair<int, WType*>>();
}
std::vector<WFunction*>& WType::getConstructors() {
	return constructors;
}
void WType::addConstructor(WFunction* wf) {
	//assert(isClassTy());
	assert(wf->getArgTy(0) == this->getRefTo());
	constructors.push_back(wf); //TODO prevent collision!
}
/* == == WType array type methods == == */
WType* WType::getArrayElemType() {
	assert(isArrayTy());
	return *containedArrType;
}
/* == == WType implicit cast == == */
bool WType::canImplicitCastTo(WType* to) {
	if (isRefTy() && (*RefdType)->canImplicitCastTo(to)) return true; //Downcasting reference to RValue
	if (isRefTy() && to->isRefTy())
	{} //Do nothing for now, when inheritance + classes are implement, this will have more to do
	//Trying to cast a non-ref to a ref is not allowed.
	if (!isRefTy() && to->isRefTy()) return false;
	//If to == this, or if there is an implicit cast function defined
	return to == this || isImplicitCastFuncDefd(this, to);
}



/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
/* == == == == == WFunction Impl == == == == == */
/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
WFunction::WFunction(Function* F, StringRef name, WType* ftype, bool isClassMethod): WValue(), F(F), name(name)  {
	WValue::type = ftype;
	if (isClassMethod) {
		assert(ftype->getFnArgTy(0)->isClassTy());
		WType* thisTy = ftype->getFnArgTy(0);
		Value* LLThis = F->getArg(0);
		for (auto& vd: thisTy->getClassContents()) {

		}
	}
}

WFunction::WFunction(StringRef name, std::function<WType* (ArrayRef<WType*>)> genericGenerator): name(name), genericGenerator(genericGenerator) {
	WValue::type = WType::getGenericTy();
	genericImpls.emplace();
}
Function* WFunction::getLLFunction() {
	assert(isFunction());
	return *F;
}
FunctionType* WFunction::getLLFType() {
	assert(isFunction());
	return (*F)->getFunctionType();
}
std::vector<WType*> WFunction::getArgs() {
	std::vector<WType*> out;
	for (WType* t: (*type)->getFnArgs())
		out.push_back(t);
	return out;
}
WType* WFunction::getArgTy(int n) {
	return (*type)->getFnArgTy(n);
}
WType* WFunction::getRetTy() {
	return (*type)->getFnRetTy();
}
size_t WFunction::getArgCt() {
	return (*type)->getFnArgCt();
}

bool WFunction::doAllPathsExit() {
	return allPathsDoExit;
}
void WFunction::setAllPathsDoExit() {
	allPathsDoExit = true;
}


bool WFunction::canCall(ArrayRef<WType*> argTypes) {
	//Assertions mirror traditional functions
	if (isGeneric()) {
		for (int i = 0; i < argTypes.size(); i++) {
			if (!argTypes[i]->isTypeTy())
				return false;
		}
		return true;
	}
	assert(isFunction());
	if (argTypes.size() != getArgs().size())
		return false;
	for (int i = 0; i < argTypes.size(); i++) {
		//Try implicitly casting.
		if (!argTypes[i]->canImplicitCastTo(getArgTy(i)))
			return false;
	}
	return true;
}

std::optional<WValue*> WFunction::call(ArrayRef<WValue*> args) {
	std::vector<WType*> argTys;
	for (WValue* wv: args)
		argTys.push_back(wv->getType());
	assert(canCall(argTys));
	if (isGeneric()) {
		std::vector<WType*> argsAsTys;
		for (WValue* arg: args)
			argsAsTys.push_back(arg->asWType());
		auto pair = (*genericImpls)[argsAsTys];
		if (pair != nullptr) return pair;
		WType* typeRes = (*genericGenerator)(argsAsTys);
		(*genericImpls)[argsAsTys] = typeRes;
		return typeRes;
	}
	assert(isFunction());
	std::vector<WValue*> castedArgs;
	for (int i = 0; i < args.size(); i++) {
		//TODO handle casting ref types
		//Current fix: if its byRef, don't cast!
		castedArgs.push_back(*args[i]->castTo(getArgTy(i)));
	}
	std::vector<Value*> LLArgs;
	for (int i = 0; i < args.size(); i++)
		LLArgs.push_back(castedArgs[i]->getStorage());
	if (!getRetTy()->isVoidTy()) {
		WValue* resVal = new WValue(getRetTy());
		Value* callRes = LLBuilder.CreateCall(getLLFType(), getLLFunction(), LLArgs);
		LLBuilder.CreateStore(callRes, resVal->getStorage());
		return resVal;
	}
	LLBuilder.CreateCall(getLLFType(), getLLFunction(), LLArgs);
	return std::optional<WValue*>();
}
bool WFunction::isGeneric() {
	return (bool) genericGenerator;
	//TODO user defined generics
}
bool WFunction::isFunction() {
	return (bool) F;
}
/*
 * Other helper functions
 */
StructType* getLLArrayType(WType* tp, int dims, ArrayRef<int> sizes) {
	assert(dims > 0);
	assert(sizes.size() <= dims);
	Type* T;
	if (dims == 1) {
		T = tp->getLLType();
	} else {
		T = getLLArrayType(tp, dims - 1, sizes);
	}
	return StructType::get(LLContext, {Type::getInt32Ty(LLContext), T->getPointerTo()});
}
std::optional<WFunction*> getPerfectFunctionMatch(ArrayRef<WFunction*> funcs, ArrayRef<WType*> args) {
	for (WFunction* WF: funcs) {
		if (WF->getArgCt() != args.size())
			continue;
		for (int i = 0; i < args.size(); i++) {
			if (WF->getArgTy(i) != args[i]) {
				if (args[i]->isRefTy() && WF->getArgTy(i) == args[i]->getTypeRefd())
				{} //If it's a ref, but the ref dereference is perfect, it's still considered perfect
				else goto match_fail;
			}
		}
		//match perfect!
		return WF;
	match_fail:
		continue;
	}
	return std::optional<WFunction*>();
}
