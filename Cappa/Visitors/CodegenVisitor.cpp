/*
 * CodeGenVisitor.cpp
 *
 *  Created on: Nov 24, 2020
 *      Author: chris
 */

#include <Visitors/CodegenVisitor.h>
#include <llvm/ADT/APInt.h>

void CodegenVisitor::ReturnVal(WValue* V) {
	Return = V;
}
void CodegenVisitor::ReturnVal(std::optional<WValue*> opt) {
	Return = opt;
}
void CodegenVisitor::NoReturn() {
	Return = std::optional<WValue*>();
}
WValue* CodegenVisitor::eval(Visitable& v) {
	visit(v);
	return *Return;
}

void CodegenVisitor::visitProgram(ProgramAST& pgm) {
	for (GlobalPtr& g: pgm.glbls)
		visit(*g);
}
/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
/* == == == == ==  Expression Visitors == == == == == */
/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
void CodegenVisitor::visitIntExpr(IntExprAST& exp) {
	WValue* v = new WValue(WType::getIntTy());
	Value* C = ConstantInt::get(Type::getInt32Ty(LLContext), exp.val, true);
	LLBuilder.CreateStore(C, v->getStorage());
	ReturnVal(v);
}

void CodegenVisitor::visitBoolExpr(BoolExprAST& exp) {
	WValue* v = new WValue(WType::getBoolTy());
	Value* C = ConstantInt::get(Type::getInt1Ty(LLContext), exp.val, false);
	LLBuilder.CreateStore(C, v->getStorage());
	ReturnVal(v);
}
void CodegenVisitor::visitDoubleExpr(DoubleExprAST& exp) {
	WValue* v = new WValue(WType::getDoubleTy());
	Value* C = ConstantFP::get(Type::getDoubleTy(LLContext), APFloat(exp.val));
	LLBuilder.CreateStore(C, v->getStorage());
	ReturnVal(v);
}
void CodegenVisitor::visitCharExpr(CharExprAST& exp) {
	WValue* v = new WValue(WType::getByteTy());
	Value* C = ConstantInt::get(Type::getInt8Ty(LLContext), exp.ch);
	LLBuilder.CreateStore(C, v->getStorage());
	ReturnVal(v);
}
void CodegenVisitor::visitVarRefExpr(VarRefExprAST& exp) {
	//printf("%s\n", exp.str.c_str());
	auto optSym = exp.findSym(exp.str);
	if (optSym) return ReturnVal((*optSym)->getVal());

	//TODO remove this stuff as more symbol-based resolution is added
	WValue* v = BuiltinValues[exp.str];
	if (v)// return ReturnVal(v);
	{
		//If v is a type, just return v
		if (v->getType()->isTypeTy())
			return ReturnVal(v);
		return ReturnVal(v);
	}
	v = NamedValues[exp.str];
	if (v) {
		if (v->getType()->isTypeTy())
			return ReturnVal(v);
		//If v is a reference to a value, return it
		if (v->getType()->isRefTy())
			return ReturnVal(v);
		//If v is an actual value, create a reference to it
		WValue* refV = new WValue(v->getType()->getRefTo());
		LLBuilder.CreateStore(v->getStorage(), refV->getStorage());
		return ReturnVal(refV);
	}
	if (!v) {
		auto& vec = NamedFunctions[exp.str];
		if (vec.size() == 1) return ReturnVal(vec[0]);
		else if (vec.size() > 1) {
			auto opt = getPerfectFunctionMatch(vec, preferredFunctionArgTypes);
			if (opt) return ReturnVal(*opt);
			std::vector<WFunction*> matches;
			for (WFunction* wf: vec) {
				if (wf->canCall(preferredFunctionArgTypes))
					matches.push_back(wf);
			}
			if (matches.size() == 0) fatal("No matching function!");
			else if (matches.size() == 1) return ReturnVal(matches[0]);
			else fatal("Function call is ambiguous!");
		}
		//TODO function overloading!
		fatal("Unable to resolve '%s'", exp.str.c_str());
	}
	ReturnVal(v);
}
void CodegenVisitor::visitOpExpr(OpExprAST& exp) {
	//printf("%s\n", exp.op.c_str());
	std::vector<WValue*> opArgs;
	std::vector<WType*> opArgTys;
	opArgs.push_back(eval(*exp.LHS));
	WValue* L = opArgs[0];
	opArgTys.push_back(L->getType());
	if (exp.RHS) {
		opArgs.push_back(eval(**exp.RHS));
		opArgTys.push_back(opArgs[1]->getType());
	}
	//TODO Operator overloading
	std::vector<WFunction*> ops = BuiltinOps[exp.op]; //The lexer catches any unknown opers prior to this point
	if (exp.op == "->") {
		assert(exp.RHS);
		WValue* R = opArgs[1];
		assert("Must specifiy type!" && R->getType()->isTypeTy());
		std::vector<WFunction*> matches;
		for (WFunction* WF: ops) {
			//Must be a perfect match for return type
			if (WF->getRetTy() == R->asWType()) {
				if (WF->getArgTy(0) == L->getType()) {
					//Immediately return for a perfect match
					auto opt = WF->call({L});
					return ReturnVal(opt);
				}
				//add to matches if WF can call L if L is implicitly cast
				if (WF->canCall({L->getType()}))
					matches.push_back(WF);

			}
		}
		if (matches.size() == 0)
			fatal("No operator overload for '->'!");
		else if (matches.size() == 1)
			return ReturnVal(matches[0]->call({*L->castTo(matches[0]->getArgTy(0))}));
		else fatal("Ambiguous call for '->'!");
	}

	auto perfect = getPerfectFunctionMatch(ops, opArgTys);
	if (perfect) {
		auto opt = (*perfect)->call(opArgs);
		return ReturnVal(opt);
	}
	std::vector<WFunction*> matches;
	for (WFunction* wf: ops) {
		if (wf->canCall(opArgTys))
			matches.push_back(wf);
	}
	if (matches.size() == 1) {
		auto opt = matches[0]->call(opArgs);
		return ReturnVal(opt);
	} else if (matches.size() == 0)
		fatal("No matching function definition found for operator '%s'!", exp.op.c_str());
	else
		fatal("Ambigous operator overload!");
}
void CodegenVisitor::visitCallExpr(CallExprAST& exp) {
	if (exp.val) return ReturnVal(*exp.val);

	std::vector<WValue*> argVals;
	std::vector<WType*> argTypes;
	if (callingFrom.size() != 0) {
		WValue* refv = new WValue(callingFrom.back()->getType()->getRefTo());
		LLBuilder.CreateStore(callingFrom.back()->getStorage(), refv->getStorage());
		argVals.push_back(refv);
		argTypes.push_back(refv->getType());
	}
	for (int i = 0; i < exp.args.size(); i++)
		argVals.push_back(eval(*exp.args[i]));
	for (int i = 0; i < argVals.size(); i++)
		argTypes.push_back(argVals[i]->getType());

	std::vector<WType*> poppedTypes = std::move(preferredFunctionArgTypes);
	preferredFunctionArgTypes = argTypes;

	WValue* calleeVal = eval(*exp.callee);
	if (calleeVal == nullptr)
	{
		fatal("TODO: Pre-symbol resolution for functions");
	}
	if (calleeVal->getType()->isTypeTy()) { //Calling a type means calling it's constructor
		WType* calleeType = calleeVal->asWType();
		WValue* allocObj = new WValue(calleeType); //Create class
		WValue* refTo = new WValue(calleeType->getRefTo()); //Refence to class
		LLBuilder.CreateStore(allocObj->getStorage(), refTo->getStorage());

		argVals.insert(argVals.begin(), refTo);
		argTypes.insert(argTypes.begin(), calleeType->getRefTo());

		auto& vec = calleeType->getConstructors();
		//Check for perfect match
		auto optPerfMatch = getPerfectFunctionMatch(vec, argTypes);
		if (optPerfMatch) {
			//Call the constructor
			(*optPerfMatch)->call(argVals);
			return ReturnVal(allocObj); //Return the object
		}
		std::vector<WFunction*> matches;
		for (WFunction* wf: vec)
			if (wf->canCall(argTypes))
				matches.push_back(wf);
		if (matches.size() == 0)
			fatal("No matching function call!");
		else if (matches.size() == 1) {
			matches[0]->call(argVals);
			return ReturnVal(allocObj);
		} else fatal("Ambiguous function call!");
	} else if (calleeVal->getType()->isGenericTy()) {
		//Calling a generic type!
		return ReturnVal(calleeVal->asWFunction()->call(argVals));
	}
	assert(calleeVal->getType()->isFuncTy());
	if (callingFrom.size() > 0) {
		argTypes.insert(argTypes.begin(), callingFrom.back()->getType());
		argVals.insert(argVals.begin(), callingFrom.back());
		callingFrom.pop_back();
	}
	WFunction* WF = calleeVal->asWFunction();
	assert(WF->canCall(argTypes));
	ReturnVal(exp.val = WF->call(argVals));
	preferredFunctionArgTypes = poppedTypes;
}
void CodegenVisitor::visitArrayListExpr(ArrayListExprAST& exp) {
	assert(exp.list.size() > 0);
	std::vector<WValue*> evalVals;
	for (auto& e: exp.list)
		evalVals.push_back(eval(*e));
	WType* castToType = evalVals[0]->getType();
	for (int i = 0; i < evalVals.size(); i++)
		if (evalVals[i]->getType() != castToType) {
			if (evalVals[i]->getType()->canImplicitCastTo(castToType))
			{} //Do nothing now; will down cast later.
			else if (castToType->canImplicitCastTo(evalVals[i]->getType())) {
				castToType = evalVals[i]->getType();
			} else fatal("Array does not have consistent type!");
		}
	std::vector<WValue*> castVals;
	for (int i = 0; i < evalVals.size(); i++)
		castVals.push_back(*evalVals[i]->castTo(castToType));

	//The actual array data
	Value* allocArr = LLBuilder.CreateAlloca(castToType->getLLType(), 0, ConstantInt::get(Type::getInt64Ty(LLContext), castVals.size()));
	for (int idx = 0; idx < castVals.size(); idx++) {
		Value* gep = LLBuilder.CreateGEP(allocArr, {
				ConstantInt::get(Type::getInt64Ty(LLContext), idx), //Idx
		});
		LLBuilder.CreateStore(castVals[idx]->loadLL(),gep);
	}

	//Create the array object
	WType* arrayType = WType::getArrayTy(castToType);
	WValue* arrVal = new WValue(arrayType);
	Value* len_gep = LLBuilder.CreateGEP(arrVal->getStorage(), {
			ConstantInt::get(Type::getInt64Ty(LLContext), 0),
			ConstantInt::get(Type::getInt32Ty(LLContext), 0),
	});
	LLBuilder.CreateStore(ConstantInt::get(Type::getInt32Ty(LLContext), castVals.size()), len_gep);
	Value* ptr_gep = LLBuilder.CreateGEP(arrVal->getStorage(), {
			ConstantInt::get(Type::getInt64Ty(LLContext), 0),
			ConstantInt::get(Type::getInt32Ty(LLContext), 1),
	});
	LLBuilder.CreateStore(allocArr, ptr_gep);
	return ReturnVal(arrVal);
}
void CodegenVisitor::visitStringExpr(StringExprAST& exp) {
	//TODO Provide proper 'str' class
	//Create the actual string array
	std::vector<Constant*> consts;
	for (char ch: exp.str)
		consts.push_back(ConstantInt::get(Type::getInt8Ty(LLContext), ch));
	consts.push_back(ConstantInt::get(Type::getInt8Ty(LLContext), 0));
	Value* constStr = ConstantArray::get(
			ArrayType::get(Type::getInt8Ty(LLContext), consts.size()), consts);

	//Array tyoe
	WType* strtype = WType::getArrayTy(WType::getByteTy());
	//Allocate array
	WValue* arrVal = new WValue(strtype);

	//Store the array's size
	Value* len_gep = LLBuilder.CreateGEP(arrVal->getStorage(), {
			ConstantInt::get(Type::getInt64Ty(LLContext),0),
			ConstantInt::get(Type::getInt32Ty(LLContext),0)
	});
	LLBuilder.CreateStore(ConstantInt::get(Type::getInt32Ty(LLContext), exp.str.size()+1), len_gep);

	//Create a global str object
	GlobalValue* strByteSeq = LLBuilder.CreateGlobalString(exp.str, "str");
	Value* str_gep = LLBuilder.CreateGEP(strByteSeq, {
			ConstantInt::get(Type::getInt64Ty(LLContext), 0), //Base Ptr
			ConstantInt::get(Type::getInt32Ty(LLContext), 0), //Ptr to idx 0
	});
	//Store the ptr to the global into the array struct
	Value* arr_ptr_gep = LLBuilder.CreateGEP(arrVal->getStorage(),
			{	ConstantInt::get(Type::getInt64Ty(LLContext),0),
				ConstantInt::get(Type::getInt32Ty(LLContext),1) });
	LLBuilder.CreateStore(str_gep, arr_ptr_gep);

	return ReturnVal(arrVal);
}
void CodegenVisitor::visitDotExpr(DotExprAST& exp) {
	WValue* lv = eval(*exp.lexp);
	if (lv->getType()->isTypeTy())
		fatal("TODO - class methods/statics not implemented!");
	bool isRef = false;
	if (lv->getType()->isRefTy()) {
		isRef = true;
		lv = *lv->castTo(lv->getType()->getTypeRefd());
	}
	//Specification for array 'len' parameter
	if (lv->getType()->isArrayTy()) {
		if (exp.elem == "len") {
			Value* gep = LLBuilder.CreateGEP(lv->getStorage(),
				{ ConstantInt::get(Type::getInt64Ty(LLContext), 0, true),
				  ConstantInt::get(Type::getInt32Ty(LLContext), 0, true)
				});
			return ReturnVal(new WValue(gep, WType::getIntTy()));
		}
	}
	if (lv->getType()->isClassTy()) {
		for (auto& pair: lv->getType()->getClassContents()) {
			if (pair.first == exp.elem) {
				if (pair.second->isWFunction()) {
					Value* refAlloca = LLBuilder.CreateAlloca(lv->getStorage()->getType());
					WValue* wv = new WValue(refAlloca, lv->getType()->getRefTo());
					LLBuilder.CreateStore(lv->getStorage(), refAlloca);
					callingFrom.push_back(wv);
					return ReturnVal(pair.second);
				}
			}
		}
		auto opt = lv->getType()->getVar(exp.elem);
		if (opt) {
			Value* gep = LLBuilder.CreateGEP(lv->getStorage(),
					{ ConstantInt::get(Type::getInt64Ty(LLContext), 0, true),
					  ConstantInt::get(Type::getInt32Ty(LLContext), opt->first, true) }
			);
			WValue* ev = new WValue(gep, opt->second);
			if (isRef) {
				WValue* refv = new WValue(opt->second->getRefTo());
				LLBuilder.CreateStore(ev->getStorage(), refv->getStorage());
				return ReturnVal(refv);
			}
			return ReturnVal(ev);
		}

		fatal("Element does not exist!");
	} else fatal("Element does not exist!");
}

void CodegenVisitor::visitBracketExpr(BracketExprAST& exp) {
	WValue* lv = eval(*exp.lexp);
	std::vector<WValue*> listVals;
	for (ExprPtr& e: exp.list) {
		listVals.push_back(eval(*e));
	}
	if (lv->getType()->isRefTy())
		lv = *lv->castTo(lv->getType()->getTypeRefd());
	if (lv->getType()->isArrayTy()) {
		while( lv->getType()->isArrayTy() &&
						listVals.size() > 0) {
			if (lv->getType()->isRefTy())
				lv = *lv->castTo(lv->getType()->getTypeRefd());
			WValue* idx = *listVals[0]->castTo(WType::getIntTy());
			Value* arr_gep = LLBuilder.CreateGEP(lv->getStorage(),
					{
							ConstantInt::get(Type::getInt64Ty(LLContext), 0), //Base Ptr
							ConstantInt::get(Type::getInt32Ty(LLContext), 1), //Arr Ptr
					});
			Value* arr_ptr = LLBuilder.CreateLoad(arr_gep);
			Value* arr_idx = LLBuilder.CreateGEP(arr_ptr, {
					idx->loadLL() //Arr Idx
			});

			lv = new WValue(arr_idx, lv->getType()->getArrayElemType());
			WValue* allocaV = new WValue(lv->getType()->getRefTo());
			LLBuilder.CreateStore(lv->getStorage(), allocaV->getStorage());
			lv = allocaV;
			listVals.erase(listVals.begin(), listVals.begin() + 1);
		}
		if (listVals.size() == 0) return ReturnVal(lv);
	}
	fatal("Unknown operator []");
}
/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
/* == == == == ==  Statement Visitors  == == == == == */
/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
void CodegenVisitor::visitLetStmt(LetStmtAST& stmt) {
	if (stmt.typeExpr) {
		WType* WT = eval(**stmt.typeExpr)->asWType();
		WValue* WV;
		if (stmt.assTo) {
			WValue* assVal = eval(**stmt.assTo);
			if (assVal->getType()->isArrayTy())
				assVal = new WValue(LLBuilder.CreateBitCast(assVal->getStorage(),
						WT->getLLType()->getPointerTo()), WT);
			auto opt = assVal->castTo(WT);
			if (!opt) fatal("Unable to cast assignment type to value!");
			WV = *opt;
			Value* load = WV->loadLL();
			WV = new WValue(WV->getType());
			LLBuilder.CreateStore(load, WV->getStorage());
		} else {
			std::vector<WValue*> arrDimVals;
			for (auto& e: *stmt.arraySpec) {
				arrDimVals.push_back(eval(*e));
			}
			if (stmt.arraySpec) {
				WV = new WValue(WT, arrDimVals);
			} else WV = new WValue(WT);
		}
		WV->getStorage()->setName(stmt.name);
		NamedValues[stmt.name] = WV;
	} else {
		//Type is s
		assert((bool) stmt.assTo);
		WValue* WV = eval(**stmt.assTo);
		WV->getStorage()->setName("prearrty");
		if (WV->getType()->isArrayTy())
			WV = new WValue(
					LLBuilder.CreateBitCast(WV->getStorage(), WV->getType()->getLLType()->getPointerTo()),
					WV->getType());
		WV->getStorage()->setName(stmt.name);
		NamedValues[stmt.name] = WV;
		/*
		WValue* WV = new WValue(assVal->getType());
		LLBuilder.CreateStore(assVal->loadLL(), WV->getStorage());
		if (WV->getType()->isRefTy()) //TODO Copy Constructor Here
			WV = *WV->castTo(WV->getType()->getTypeRefd());*/
	}
}
void CodegenVisitor::visitBlockStmt(BlockStmtAST& stmt) {
	auto old = NamedValues;
	for (int i = 0; i < stmt.contents.size(); i++)
		visit(*stmt.contents[i]);
	NamedValues = std::move(old);
}
void CodegenVisitor::visitIfStmt(IfStmtAST& stmt) {
	Function* F = LLBuilder.GetInsertBlock()->getParent();
	WValue* CV = eval(*stmt.Cond);
	auto opt = CV->castTo(WType::getBoolTy());
	assert("Conditional Value is not a bool or can't be implicitly cast to bool!" && opt);
	CV = *opt;
	Value* cmpVal = CV->loadLL();
	if (!stmt.Else) {
		//if statement without an 'else'
		BasicBlock* ThenBB = BasicBlock::Create(LLContext, "if_then", F);
		BasicBlock* MergeBB = BasicBlock::Create(LLContext, "if_merge", F);
		//Perform the jump
		LLBuilder.CreateCondBr(cmpVal, ThenBB, MergeBB);
		//'Then' block
		LLBuilder.SetInsertPoint(ThenBB);
		eval(*stmt.Then);
		if (LLBuilder.GetInsertBlock()->getTerminator() == nullptr)
			LLBuilder.CreateBr(MergeBB);
		MergeBB->moveAfter(LLBuilder.GetInsertBlock());
		//Set to 'Merge' Block
		LLBuilder.SetInsertPoint(MergeBB);
	} else {
		//if statement with an 'else'
		BasicBlock* ThenBB = BasicBlock::Create(LLContext, "if_then", F);
		BasicBlock* ElseBB = BasicBlock::Create(LLContext, "if_else", F);
		BasicBlock* MergeBB = BasicBlock::Create(LLContext, "if_merge", F);
		//Perform the jump
		LLBuilder.CreateCondBr(cmpVal, ThenBB, ElseBB);
		//'Then' block
		LLBuilder.SetInsertPoint(ThenBB);
		eval(*stmt.Then);
		if (LLBuilder.GetInsertBlock()->getTerminator() == nullptr)
			LLBuilder.CreateBr(MergeBB);
		//'Else' Block
		ElseBB->moveAfter(LLBuilder.GetInsertBlock());
		LLBuilder.SetInsertPoint(ElseBB);
		eval(**stmt.Else);
		if (LLBuilder.GetInsertBlock()->getTerminator() == nullptr)
			LLBuilder.CreateBr(MergeBB);
		//Set to 'Merge' Block
		MergeBB->moveAfter(LLBuilder.GetInsertBlock());
		LLBuilder.SetInsertPoint(MergeBB);
	}
}
void CodegenVisitor::visitForStmt(ForStmtAST& stmt) {
	Function* F = LLBuilder.GetInsertBlock()->getParent();
	BasicBlock* HeadBB = BasicBlock::Create(LLContext, "for_head", F);
	LLBuilder.CreateBr(HeadBB);
	LLBuilder.SetInsertPoint(HeadBB);
	//VarDef, if present, in current block
	if (stmt.VarDef) {
		visit(**stmt.VarDef);
	}
	std::optional<BasicBlock*> CondBB = {};
	std::optional<BasicBlock*> IterBB = {};
	if (stmt.Iter) IterBB = BasicBlock::Create(LLContext, "for_iter", F);
	if (stmt.Cond) CondBB = BasicBlock::Create(LLContext, "for_condition", F);
	BasicBlock* LoopBB = BasicBlock::Create(LLContext, "for_loop", F);
	BasicBlock* MergeBB = BasicBlock::Create(LLContext, "for_merge", F);
	//Jump if Cond exists, else straight to Loop
	if (stmt.Cond) {
		LLBuilder.CreateBr(*CondBB);
	} else {
		LLBuilder.CreateBr(LoopBB);
	}
	//Iterater if present
	if (stmt.Iter) {
		LLBuilder.SetInsertPoint(*IterBB);
		visit(**stmt.Iter);
		//After iterating, check condition if present
		if (stmt.Cond)
			LLBuilder.CreateBr(*CondBB);
		else LLBuilder.CreateBr(LoopBB);
		if (LLBuilder.GetInsertBlock() != *IterBB)
			(*IterBB)->moveAfter(LLBuilder.GetInsertBlock());
	}
	//Condition Check, if present
	if (stmt.Cond) {
		LLBuilder.SetInsertPoint(*CondBB);
		WValue* wv = eval(**stmt.Cond);
		if (!wv->getType()->canImplicitCastTo(WType::getBoolTy()))
			fatal("Not a boolean type!");
		WValue* cast = *wv->castTo(WType::getBoolTy());
		Value* brcd = cast->loadLL();
		//Jump to Loop or Merge, based on Cond
		LLBuilder.CreateCondBr(brcd, LoopBB, MergeBB);
		if (LLBuilder.GetInsertBlock() != *CondBB)
			(*CondBB)->moveAfter(LLBuilder.GetInsertBlock());
	}
	//Loop
	LLBuilder.SetInsertPoint(LoopBB);
	visit(*stmt.Loop);
	if (stmt.Iter) {
		LLBuilder.CreateBr(*IterBB);
	} else {
		//No iterator
		if (stmt.Cond) {
			//No iterator, so check condition
			LLBuilder.CreateBr(*CondBB);
		} else {
			//No condition or iterator, simply go to head of loop
			LLBuilder.CreateBr(LoopBB);
		}
	}
	MergeBB->moveAfter(LLBuilder.GetInsertBlock());
	LLBuilder.SetInsertPoint(MergeBB);
}
void CodegenVisitor::visitWhileStmt(WhileStmtAST& stmt) {
	Function* F = LLBuilder.GetInsertBlock()->getParent();
	BasicBlock* CondBB = BasicBlock::Create(LLContext, "while_condition", F);
	BasicBlock* LoopBB = BasicBlock::Create(LLContext, "while_loop", F);
	BasicBlock* MergeBB = BasicBlock::Create(LLContext, "while_merge", F);
	//Cond
	LLBuilder.CreateBr(CondBB);
	LLBuilder.SetInsertPoint(CondBB);
	WValue* condVal = eval(*stmt.Cond);
	assert("While condition must be boolean!" && condVal->getType()->isBoolTy());
	Value* LLCond = condVal->loadLL();
	LLBuilder.CreateCondBr(LLCond, LoopBB, MergeBB);
	//Loop
	LLBuilder.SetInsertPoint(LoopBB);
	visit(*stmt.Loop);
	LLBuilder.CreateBr(CondBB);
	//Merge
	MergeBB->moveAfter(LLBuilder.GetInsertBlock());
	LLBuilder.SetInsertPoint(MergeBB);
}
void CodegenVisitor::visitExprStmt(ExprStmtAST& stmt) {
	eval(*stmt.expr);
}
void CodegenVisitor::visitRetStmt(RetStmtAST& stmt) {
	Function *F = LLBuilder.GetInsertBlock()->getParent();
	if (F->getReturnType()->isVoidTy()) {
		if (stmt.toRet) {
			error("Return in non-void function");
		}
		LLBuilder.CreateRetVoid();
	} else {
		if (!stmt.toRet) {
			error("Must return in non-void function!");
		} else {
			WValue* retV = eval(**stmt.toRet);
			auto castRetVOpt= retV->castTo((*curFunction)->getRetTy());
			assert("Could not cast return value to function return!" && (bool) castRetVOpt);
			LLBuilder.CreateRet((*castRetVOpt)->loadLL());
		}
	}
}
/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
/* == == == == ==  Global Visitors  == == == == == */
/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
void CodegenVisitor::visitFunctionPrototypeGlbl(FunctionPrototypeAST& proto) {
#if 1
#define YAY
	ReturnVal(*proto.getVal());
#else

	std::vector<WType*> argTypes;
	if (definingClass)
		argTypes.push_back((*definingClass)->getRefTo());
	for (auto& pair: proto.argTypes) {
		WType* t = eval(*pair.first)->asWType();
		argTypes.push_back(t);
	}
	WType* FTP = WType::getFuncTy(proto.retTypeExpr ? static_cast<WType*>(eval(**proto.retTypeExpr)) : WType::getVoidTy(), argTypes);
	Function *LLF = Function::Create(*FTP->getLLFnType(), GlobalValue::LinkageTypes::ExternalLinkage, proto.name, *LLModule);
	WFunction* WF = new WFunction(LLF, proto.name, FTP);
	if (definingClass)
		WF->getLLFunction()->getArg(0)->setName("this");
	for (int i = 0; i < proto.argTypes.size(); i++) {
		WF->getLLFunction()->getArg(i + (definingClass ? 1 : 0))->setName(
				proto.argTypes[i].second);
	}
	if (!definingClass)
		NamedFunctions[proto.name].push_back(WF);
	proto.setValue(WF);
	ReturnVal(WF);
#endif
}
void CodegenVisitor::visitFunctionGlbl(FunctionAST& funDef) {//TODO Check for prior 'extern' declaration of the function
#ifdef YAY
	WFunction* WF = eval(*funDef.proto)->asWFunction();
	assert("Unable to load function from prototype!" && WF);
	LLBuilder.SetInsertPoint(BasicBlock::Create(LLContext, "entry", WF->getLLFunction()));
	std::vector<WValue*> argWVals;
	for (int i = 0; i < WF->getArgCt(); i++) {
		Value* refAlloca = LLBuilder.CreateAlloca(WF->getLLFunction()->getArg(i)->getType());
		//WValue* WV = new WValue(WF->getLLFunction()->getArg(i), WF->getArgTy(i));
		WValue* WV = new WValue(refAlloca, WF->getArgTy(i)->getRefTo());
		LLBuilder.CreateStore(WF->getLLFunction()->getArg(i), refAlloca);
		argWVals.push_back(WV);
		Symbol* vsym = new Symbol(WF->getLLFunction()->getArg(i)->getName());
		vsym->setValue(WV);
		funDef.addShallowSymbol(*vsym);
	}
	if (definingClass) {
		//Define refs to all class values
		WType* outerClass = *definingClass;
		Value* thisVal = argWVals[0]->loadLL();
		int i = 0;
		for (auto& pair: outerClass->getClassContents()) {
			if (pair.second->getType()->isTypeTy()) {
				Value* gep = LLBuilder.CreateGEP(LLBuilder.CreateLoad(thisVal),
						{	ConstantInt::get(Type::getInt64Ty(LLContext), 0, true),
							ConstantInt::get(Type::getInt32Ty(LLContext), i, true) });
				Value* refAlloca = LLBuilder.CreateAlloca(gep->getType());
				WValue* rv = new WValue(refAlloca, pair.second->asWType()->getRefTo());
				LLBuilder.CreateStore(gep, rv->getStorage());
				Symbol* vsym = new Symbol(pair.first);
				funDef.addShallowSymbol(*vsym);
				vsym->setValue(rv);
				//NamedValues[pair.first] = rv;
				i++;
			} else {} //TODO include functions, etc. here!

		}
	}
	auto oldFunc = curFunction;
	curFunction = WF;
	eval(*funDef.body);
	if (LLBuilder.GetInsertBlock()->getTerminator() == nullptr)
		LLBuilder.CreateRetVoid(); //Just to ensure there's some sort of ret TODO Make throw error, add analysis (in if stmts mainly) to guaruntee a return value

	curFunction = oldFunc;
	return ReturnVal(WF);
#else
	auto oldNames = NamedValues;
	WFunction* WF = eval(*funDef.proto)->asWFunction();
	//Store old 'curFunction'
	auto oldFunc = curFunction;
	curFunction = WF;

	assert("Unable to create function from prototype!" && WF);
	LLBuilder.SetInsertPoint(BasicBlock::Create(LLContext, "entry", WF->getLLFunction()));
	std::vector<WValue*> argWVals;
	for (int i = 0; i < WF->getArgCt(); i++) {
		WValue* WV = new WValue(WF->getLLFunction()->getArg(i), WF->getArgTy(i));
		argWVals.push_back(WV);
	}
	if (definingClass) {
		//Define refs to all class values
		WValue* thisVal = argWVals[0];
		int i = 0;
		for (auto& pair: (*definingClass)->getClassContents()) {
			if (pair.second->getType()->isTypeTy()) {
				Value* gep = LLBuilder.CreateGEP(
						LLBuilder.CreateLoad(thisVal->getStorage()),
						{	ConstantInt::get(Type::getInt64Ty(LLContext), 0, true),
							ConstantInt::get(Type::getInt32Ty(LLContext), i, true) });
				WValue* rv = new WValue(gep, pair.second->asWType());
				NamedValues[pair.first] = rv;
			} else {} //TODO include functions, etc. here!
			i++;

		}
	}

	for (int i = 0; i < WF->getArgCt(); i++) {
		NamedValues[(std::string) WF->getLLFunction()->getArg(i)->getName()] = argWVals[i];
	}

	eval(*funDef.body);
	if (LLBuilder.GetInsertBlock()->getTerminator() == nullptr)
		LLBuilder.CreateRetVoid(); //Just to ensure there's some sort of ret TODO Make throw error, add analysis (in if stmts mainly) to guaruntee a return value
	//LLFPM->run(*WF->getLLFunction());

	//restore old 'curFunction' && 'NamedValues'
	curFunction = oldFunc;
	NamedValues = oldNames;
	funDef.setValue(WF);
	ReturnVal(WF);
#endif
}

void CodegenVisitor::visitClassGlbl(ClassAST& cls) {
	if ((*cls.getVal())->isWFunction()) {
		assert((*cls.getVal())->asWFunction()->isGeneric());
		return; //Do nothing here for generic functions
	}
	auto popCurClassVals = std::move(curClassVals);
	curClassVals.clear();
	for (auto& vd: cls.vars) {
		visit(*vd);
	}
	WType* clsType = (*cls.getVal())->asWType();
	clsType->setClassContents(curClassVals);
	NamedValues[cls.name] = clsType;
	auto popDefClass = definingClass;
	definingClass = clsType;
	auto popIsConstructor = isConstructor;
	for (auto& fd: cls.funcs) {
		isConstructor = fd->proto->getName() == cls.name;
		WValue* v = eval(*fd);
		WFunction* fv = v->asWFunction();
		fv->getLLFunction()->setName(cls.name + "::" + fv->getLLFunction()->getName());
		if (fd->proto->getName() == cls.name) {
			clsType->addConstructor(fv);
		} else fatal("Class methods nyi!");
	}
	definingClass = popDefClass;
	curClassVals = popCurClassVals;
	isConstructor = popIsConstructor;
}
void CodegenVisitor::visitClassVarDefGlbl(ClassVarDefAST& vd) {
	WValue* wvt = eval(*vd.typeExpr);
	WType* wt = wvt->asWType();
	curClassVals[vd.name] = wt;
}


