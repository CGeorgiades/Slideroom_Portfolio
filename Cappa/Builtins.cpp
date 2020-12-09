/*
 * BuiltinOpers.cpp
 *
 *  Created on: Nov 21, 2020
 *      Author: chris
 */

#include <Builtins.h>


std::map<std::string, WValue*> BuiltinValues;
std::map<std::string, std::vector<WFunction*>> BuiltinOps;
std::vector<Symbol*> BuiltinSymbols;
WFunction* startFunc(StringRef Name, std::vector<WType*> args, WType* ret, std::function<void (Function*)> generate) {
	std::vector<Type*> argLL;
	for (int i = 0; i < args.size(); i++)
		argLL.push_back(args[i]->getLLType()->getPointerTo());


	Function* F = Function::Create(FunctionType::get(
			ret->isVoidTy() ? Type::getVoidTy(LLContext) : ret->getLLType(),
			argLL, false), Function::LinkageTypes::PrivateLinkage, Name, LLModule.get());
	BasicBlock* bb = BasicBlock::Create(LLContext, "entry", F);
	LLBuilder.SetInsertPoint(bb);
	generate(F);
	WFunction* wf = new WFunction(F, Name, WType::getFuncTy(ret, args));
	return wf;
}
void defineValue(const std::string& name, WValue* wv) {
	Symbol* s = new Symbol(name);
	s->setValue(wv);
	BuiltinSymbols.push_back(s);
	BuiltinValues[name] = wv;
}
#define DoubleType WType::getDoubleTy()
#define BoolType WType::getBoolTy()
#define IntType WType::getIntTy()
#define ByteType WType::getByteTy()
#define VoidType WType::getVoidTy()

#define LLDoubleType Type::getDoubleTy(LLContext)
#define LLBoolType Type::getInt1Ty(LLContext)
#define LLIntType Type::getInt32Ty(LLContext)
#define LLByteType Type::getInt8Ty(LLContext)
#define LLVoidType Type::getVoidTy(LLContext)


//Defines to make these definitions more concise
//TODO add these to lib.h
#define LOAD(x) LLBuilder.CreateLoad(x)
#define STORE(v, p) LLBuilder.CreateStore(v, p)
#define RET(x) LLBuilder.CreateRet(x)
#define RETVOID LLBuilder.CreateRetVoid()

void initOps() {
	/*== Assignment Operators ==*/
	std::vector<WFunction*> Assn;
	Assn.push_back(startFunc("=", {DoubleType->getRefTo(), DoubleType}, DoubleType, [](Function* F) {
		STORE(LOAD(F->getArg(1)), LOAD(F->getArg(0)));
		RET(LOAD(F->getArg(1)));
	}));
	Assn.push_back(startFunc("=", {IntType->getRefTo(), IntType}, IntType, [](Function* F) {
		STORE(LOAD(F->getArg(1)), LOAD(F->getArg(0)));
		RET(LOAD(F->getArg(1)));
	}));
	Assn.push_back(startFunc("=", {ByteType->getRefTo(), ByteType}, ByteType, [](Function* F) {
		STORE(LOAD(F->getArg(1)), LOAD(F->getArg(0)));
		RET(LOAD(F->getArg(1)));
	}));
	Assn.push_back(startFunc("=", {BoolType->getRefTo(), BoolType}, BoolType, [](Function* F) {
		STORE(LOAD(F->getArg(1)), LOAD(F->getArg(0)));
		RET(LOAD(F->getArg(1)));
	}));
	BuiltinOps["="] = std::move(Assn);

	/*== Arithmetic Operators ==*/
	std::vector<WFunction*> Add;
	Add.push_back(startFunc("+", {DoubleType, DoubleType}, DoubleType, [](Function* F) {
		auto p = LLBuilder.CreateFAdd(LOAD(F->getArg(0)), LOAD(F->getArg(1)));
		RET(p);
	}));
	Add.push_back(startFunc("+", {IntType, IntType}, IntType, [](Function* F) {
		auto p = LLBuilder.CreateAdd(LOAD(F->getArg(0)), LOAD(F->getArg(1)));
		RET(p);
	}));
	BuiltinOps["+"] = std::move(Add);

	std::vector<WFunction*> Sub;
	Sub.push_back(startFunc("-", {DoubleType, DoubleType}, DoubleType, [](Function* F) {
		auto p = LLBuilder.CreateFSub(LOAD(F->getArg(0)), LOAD(F->getArg(1)));
		RET(p);
	}));
	Sub.push_back(startFunc("-", {IntType, IntType}, IntType, [](Function* F) {
		auto p = LLBuilder.CreateSub(LOAD(F->getArg(0)), LOAD(F->getArg(1)));
		RET(p);
	}));
	//Unary Negation
	Sub.push_back(startFunc("-", {IntType}, IntType, [](Function* F) {
		auto p = LLBuilder.CreateNeg(LOAD(F->getArg(0)));
		RET(p);
	}));
	Sub.push_back(startFunc("-", {DoubleType}, DoubleType, [](Function* F) {
		auto p = LLBuilder.CreateFNeg(LOAD(F->getArg(0)));
		RET(p);
	}));
	BuiltinOps["-"] = std::move(Sub);

	std::vector<WFunction*> Mul;
	Mul.push_back(startFunc("*", {DoubleType, DoubleType}, DoubleType, [](Function* F) {
		auto p = LLBuilder.CreateFMul(LOAD(F->getArg(0)), LOAD(F->getArg(1)));
		RET(p);
	}));
	Mul.push_back(startFunc("*", {IntType, IntType}, IntType, [](Function* F) {
		auto p = LLBuilder.CreateMul(LOAD(F->getArg(0)), LOAD(F->getArg(1)));
		RET(p);
	}));
	BuiltinOps["*"] = std::move(Mul);

	std::vector<WFunction*> Div;
	Div.push_back(startFunc("/", {DoubleType, DoubleType}, DoubleType, [](Function* F) {
		auto p = LLBuilder.CreateFDiv(LOAD(F->getArg(0)), LOAD(F->getArg(1)));
		RET(p);
	}));
	Div.push_back(startFunc("/", {IntType, IntType}, IntType, [](Function* F) {
		auto p = LLBuilder.CreateSDiv(LOAD(F->getArg(0)), LOAD(F->getArg(1)));
		RET(p);
	}));
	BuiltinOps["/"] = std::move(Div);

	std::vector<WFunction*> Mod;
	Mod.push_back(startFunc("%", {IntType, IntType}, IntType, [](Function* F) {
		auto p = LLBuilder.CreateSRem(LOAD(F->getArg(0)), LOAD(F->getArg(1)));
		RET(p);
	}));
	BuiltinOps["%"] = std::move(Mod);
	/*== Comparison Operators ==*/
	// '<'
	std::vector<WFunction*> CLT;
	CLT.push_back(startFunc("<", {IntType, IntType}, BoolType, [](Function* F) {
		auto c = LLBuilder.CreateICmpSLT(LOAD(F->getArg(0)), LOAD(F->getArg(1)));
		RET(c);
	}));
	CLT.push_back(startFunc("<", {DoubleType, DoubleType}, BoolType, [](Function* F) {
		auto c = LLBuilder.CreateFCmpOLT(LOAD(F->getArg(0)), LOAD(F->getArg(1)));
		RET(c);
	}));
	BuiltinOps["<"] = std::move(CLT);

	// '>'
	std::vector<WFunction*> CGT;
	CGT.push_back(startFunc(">", {IntType, IntType}, BoolType, [](Function* F) {
		auto c = LLBuilder.CreateICmpSGT(LOAD(F->getArg(0)), LOAD(F->getArg(1)));
		RET(c);
	}));
	CGT.push_back(startFunc(">", {DoubleType, DoubleType}, BoolType, [](Function* F) {
		auto c = LLBuilder.CreateFCmpOGT(LOAD(F->getArg(0)), LOAD(F->getArg(1)));
		RET(c);
	}));
	BuiltinOps[">"] = std::move(CGT);

	// '<='
	std::vector<WFunction*> CLE;
	CLE.push_back(startFunc("<=", {IntType, IntType}, BoolType, [](Function* F) {
		auto c = LLBuilder.CreateICmpSLE(LOAD(F->getArg(0)), LOAD(F->getArg(1)));
		RET(c);
	}));
	CLE.push_back(startFunc("<=", {DoubleType, DoubleType}, BoolType, [](Function* F) {
		auto c = LLBuilder.CreateFCmpOLE(LOAD(F->getArg(0)), LOAD(F->getArg(1)));
		RET(c);
	}));
	BuiltinOps["<="] = std::move(CLE);

	// '>='
	std::vector<WFunction*> CGE;
	CGE.push_back(startFunc(">=", {IntType, IntType}, BoolType, [](Function* F) {
		auto c = LLBuilder.CreateICmpSGE(LOAD(F->getArg(0)), LOAD(F->getArg(1)));
		RET(c);
	}));
	CGE.push_back(startFunc(">=", {DoubleType, DoubleType}, BoolType, [](Function* F) {
		auto c = LLBuilder.CreateFCmpOGE(LOAD(F->getArg(0)), LOAD(F->getArg(1)));
		RET(c);
	}));
	BuiltinOps[">="] = std::move(CGE);

	// '=='
	std::vector<WFunction*> CEQ;
	CEQ.push_back(startFunc("==", {IntType, IntType}, BoolType, [](Function* F) {
		auto c = LLBuilder.CreateICmpEQ(LOAD(F->getArg(0)), LOAD(F->getArg(1)));
		RET(c);
	}));
	CEQ.push_back(startFunc("==", {BoolType, BoolType}, BoolType, [](Function* F) {
		auto c = LLBuilder.CreateICmpEQ(LOAD(F->getArg(0)), LOAD(F->getArg(1)));
		RET(c);
	}));
	CEQ.push_back(startFunc("==", {DoubleType, DoubleType}, BoolType, [](Function* F) {
		auto c = LLBuilder.CreateFCmpOEQ(LOAD(F->getArg(0)), LOAD(F->getArg(1)));
		RET(c);
	}));
	BuiltinOps["=="] = std::move(CEQ);

	// '!='
	std::vector<WFunction*> NEQ;
	NEQ.push_back(startFunc("!=", {IntType, IntType}, BoolType, [](Function* F) {
		auto c = LLBuilder.CreateICmpNE(LOAD(F->getArg(0)), LOAD(F->getArg(1)));
		RET(c);
	}));
	NEQ.push_back(startFunc("!=", {BoolType, BoolType}, BoolType, [](Function* F) {
		auto c = LLBuilder.CreateICmpNE(LOAD(F->getArg(0)), LOAD(F->getArg(1)));
		RET(c);
	}));
	NEQ.push_back(startFunc("!=", {DoubleType, DoubleType}, BoolType, [](Function* F) {
		auto c = LLBuilder.CreateFCmpONE(LOAD(F->getArg(0)), LOAD(F->getArg(1)));
		RET(c);
	}));
	BuiltinOps["!="] = std::move(NEQ);

	/*== Bool Operators ==*/
	// '||'
	std::vector<WFunction*> BOR;
	BOR.push_back(startFunc("||", {BoolType, BoolType}, BoolType, [](Function* F) {
		auto c = LLBuilder.CreateOr(LOAD(F->getArg(0)), LOAD(F->getArg(1)));
		RET(c);
	}));
	BuiltinOps["||"] = std::move(BOR);

	std::vector<WFunction*> BAND;
	BAND.push_back(startFunc("&&", {BoolType, BoolType}, BoolType, [](Function* F) {
		auto c = LLBuilder.CreateAnd(LOAD(F->getArg(0)), LOAD(F->getArg(1)));
		RET(c);
	}));
	BuiltinOps["&&"] = std::move(BAND);

	/*== Cast Operators ==*/
	// 'I()' (Implicit cast)
	std::vector<WFunction*> ICast;
	ICast.push_back(startFunc("I()", {IntType}, DoubleType, [](Function* F) {
		auto c = LLBuilder.CreateSIToFP(LOAD(F->getArg(0)), LLDoubleType);
		RET(c);
	}));
	BuiltinOps["I()"] = std::move(ICast);

	// '->' Explicit Cast Operator
	std::vector<WFunction*> ECast;
	ECast.push_back(startFunc("->", {IntType}, DoubleType, [](Function* F) {
		auto c = LLBuilder.CreateSIToFP(LOAD(F->getArg(0)), LLDoubleType);
		RET(c);
	}));
	ECast.push_back(startFunc("->", {DoubleType}, IntType, [](Function* F) {
		auto c = LLBuilder.CreateFPToSI(LOAD(F->getArg(0)), LLIntType);
		RET(c);
	}));
	ECast.push_back(startFunc("->", {BoolType}, IntType, [](Function* F) {
			auto c = LLBuilder.CreateZExt(LOAD(F->getArg(0)), LLIntType);
			RET(c);
		}));
	BuiltinOps["->"] = std::move(ECast);


	for (auto pair: BuiltinOps) {
		for (auto f: pair.second) {
			f->getLLFunction()->addFnAttr(Attribute::get(LLContext, Attribute::AlwaysInline, (uint64_t) 0));
		}
	}
}
void initVals() {
	defineValue("bool", WType::getBoolTy());
	defineValue("int", WType::getIntTy());
	defineValue("double", WType::getDoubleTy());
	defineValue("byte", WType::getByteTy());
	defineValue("void", WType::getVoidTy());
	defineValue("array", new WFunction("array", [](ArrayRef<WType*> types)->WType* {
		assert(types.size() == 1);
		return WType::getArrayTy(types[0]);
	}));

}
void initBuiltins() {
	initOps();
	initVals();
	FunctionType* printfTp = FunctionType::get(LLIntType, {LLByteType->getPointerTo()}, true);
	Function* printfFunc = Function::Create(printfTp, Function::ExternalLinkage, "printf", *LLModule);
	BuiltinValues["printch"] = startFunc("printch", {ByteType}, VoidType, [=](Function* f) {
		Value* fmtc = ConstantArray::get(ArrayType::get(LLByteType, 3),
			{	ConstantInt::get(LLByteType, '%'),
				ConstantInt::get(LLByteType, 'c'),
				ConstantInt::get(LLByteType, '\0')
			});
		Value* arr = LLBuilder.CreateAlloca(fmtc->getType());
		STORE(fmtc, arr);
		Value* gep = LLBuilder.CreateGEP(arr,
				{	ConstantInt::get(Type::getInt64Ty(LLContext), 0),
					ConstantInt::get(Type::getInt64Ty(LLContext), 0)
				});
		LLBuilder.CreateCall(printfFunc, {gep, LOAD(f->getArg(0))});
		LLBuilder.CreateRetVoid();
	});
	BuiltinValues["println"] = startFunc("println", {}, VoidType, [=](Function* f) {
		Value* fmtc = ConstantArray::get(ArrayType::get(LLByteType, 3),
			{	ConstantInt::get(LLByteType, '\n'),
				ConstantInt::get(LLByteType, '\0')
			});
		Value* arr = LLBuilder.CreateAlloca(fmtc->getType());
		STORE(fmtc, arr);
		Value* gep = LLBuilder.CreateGEP(arr,
				{	ConstantInt::get(Type::getInt64Ty(LLContext), 0),
					ConstantInt::get(Type::getInt64Ty(LLContext), 0)
				});
		LLBuilder.CreateCall(printfFunc, {gep});
		LLBuilder.CreateRetVoid();
	});
	BuiltinValues["printint"] = startFunc("printint", {IntType}, VoidType, [=](Function* f) {
		Value* fmtc = ConstantArray::get(ArrayType::get(LLByteType, 3),
			{	ConstantInt::get(LLByteType, '%'),
				ConstantInt::get(LLByteType, 'd'),
				ConstantInt::get(LLByteType, '\0')
			});
		Value* arr = LLBuilder.CreateAlloca(fmtc->getType());
		STORE(fmtc, arr);
		Value* gep = LLBuilder.CreateGEP(arr,
				{	ConstantInt::get(Type::getInt64Ty(LLContext), 0),
					ConstantInt::get(Type::getInt64Ty(LLContext), 0)
				});
		LLBuilder.CreateCall(printfFunc, {gep, LOAD(f->getArg(0))});
		LLBuilder.CreateRetVoid();
	});
	BuiltinValues["printdouble"] = startFunc("printdouble", {DoubleType}, VoidType, [=](Function* f) {
		Value* fmtc = ConstantArray::get(ArrayType::get(LLByteType, 3),
			{	ConstantInt::get(LLByteType, '%'),
				ConstantInt::get(LLByteType, 'f'),
				ConstantInt::get(LLByteType, '\0')
			});
		Value* arr = LLBuilder.CreateAlloca(fmtc->getType());
		STORE(fmtc, arr);
		Value* gep = LLBuilder.CreateGEP(arr,
				{	ConstantInt::get(Type::getInt64Ty(LLContext), 0),
					ConstantInt::get(Type::getInt64Ty(LLContext), 0)
				});
		LLBuilder.CreateCall(printfFunc, {gep, LOAD(f->getArg(0))});
		LLBuilder.CreateRetVoid();
	});
	BuiltinValues["printstr"] = startFunc("printstr", {WType::getArrayTy(ByteType)}, VoidType, [=](Function* f) {
		Value* fmts = ConstantArray::get(ArrayType::get(LLByteType, 5),
		{	ConstantInt::get(LLByteType, '%'),
			ConstantInt::get(LLByteType, '.'),
			ConstantInt::get(LLByteType, '*'),
			ConstantInt::get(LLByteType, 's'),
			ConstantInt::get(LLByteType, '\0')
		});
		Value* fmts_arr = LLBuilder.CreateAlloca(fmts->getType());
		STORE(fmts, fmts_arr);
		Value* fmtgep = LLBuilder.CreateGEP(fmts_arr,
				{	ConstantInt::get(Type::getInt64Ty(LLContext), 0),
					ConstantInt::get(Type::getInt64Ty(LLContext), 0)
				});
		Value* len_i32 = LLBuilder.CreateGEP(f->getArg(0),
				{ 	ConstantInt::get(Type::getInt64Ty(LLContext), 0),
					ConstantInt::get(Type::getInt32Ty(LLContext), 0)});
		Value* arr_gep = LLBuilder.CreateGEP(f->getArg(0),
				{	ConstantInt::get(Type::getInt64Ty(LLContext), 0),
					ConstantInt::get(Type::getInt32Ty(LLContext), 1)});
		LLBuilder.CreateCall(printfFunc, {fmtgep, LOAD(len_i32), LOAD(arr_gep)});
		LLBuilder.CreateRetVoid();
	});

}
bool isImplicitCastFuncDefd(WType* from, WType* to) {
	auto& vec = BuiltinOps["I()"];
	for (WFunction* WF: vec) {
		if (WF->getArgTy(0) == from &&
			WF->getRetTy() == to)
			return true;
	}
	return false;
}
std::optional<WFunction*> getImplicitCastFunc(WType* from, WType* to) {
	auto& vec = BuiltinOps["I()"];
	for (WFunction* WF: vec) {
		if (WF->getArgTy(0) == from &&
			WF->getRetTy() == to)
			return WF;
	}
	return std::optional<WFunction*>();

}
