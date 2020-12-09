/*
 * SigGenVisitor.cpp
 *
 *  Created on: Dec 3, 2020
 *      Author: chris
 */

#include <Visitors/SigGenVisitor.h>
#include <Visitors/CodegenVisitor.h>


void SigGenVisitor::ReturnVal(WValue* v) {Return = v;}
void SigGenVisitor::ReturnVal(std::optional<WValue*> opt) {Return = opt;}
void SigGenVisitor::NoReturn() {Return = {};}

std::optional<Symbol*> SigGenVisitor::getSym(Visitable& v, StringRef symName) {
	auto opt = v.findSym(symName); //Recursive symbol check
	if (opt) return opt;
	if (genSyms) { //Generic symbol
		for (Symbol* s: *genSyms)
			if (s->getName() == symName)
				return s;
	}
	return {};
}
std::optional<WValue*> SigGenVisitor::eval(Visitable& v) {
	visit(v);
	return Return;
}

SigGenVisitor::SigGenVisitor(Visitable* toGen, std::map<std::string, WType*> genArgs, WType** genResult)
		: genericResult(genResult) {
	genSyms.emplace();
	for (auto& pair: genArgs) {
		//printf("%s\n", pair.first.c_str());
		Symbol* s = new Symbol(pair.first);
		s->setValue(pair.second);
		genSyms->push_back(s);
	}
	toGenGeneric = toGen;
	visit(toGen);
}

void SigGenVisitor::visitProgram(ProgramAST& pgm) {
	for (GlobalPtr& g: pgm.glbls)
		visit(*g);
}
/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
/* == == == == ==  Expression Visitors == == == == == */
/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
void SigGenVisitor::visitIntExpr(IntExprAST& exp) {}
void SigGenVisitor::visitBoolExpr(BoolExprAST& exp) {}
void SigGenVisitor::visitDoubleExpr(DoubleExprAST& exp) {}
void SigGenVisitor::visitCharExpr(CharExprAST&) {}
void SigGenVisitor::visitVarRefExpr(VarRefExprAST& exp) {
	//printf("%s\n", exp.str.c_str());
	auto optSym = getSym(exp, exp.str);
	if (optSym) {
		Symbol* s = *optSym;
		if (s->hasVal()) {
			WValue* wv = *s->getVal();
			if (wv->isWType()) {
				WType* wt = wv->asWType();
				return ReturnVal(wt);
			} else if (wv->isWFunction()) {
				WFunction* wf = wv->asWFunction();
				if (wf->isGeneric()) return ReturnVal(wf);
			}
		}
	}
}
void SigGenVisitor::visitOpExpr(OpExprAST& exp) {
	visit(*exp.LHS);
	if (exp.RHS)
		visit(**exp.RHS);
}
void SigGenVisitor::visitCallExpr(CallExprAST& exp) {
	auto optCallee = eval(*exp.callee);
	std::vector<std::optional<WValue*>> argValOpts;
	for (ExprPtr& e: exp.args)
		argValOpts.push_back(eval(*e));
	if (optCallee) {
		WValue* C = *optCallee;
		if (C->isWType())
		{} //Do nothing, all's well
		else {
			assert("Valued callee is not WType or Generic WFunction!"
					&& C->isWFunction() && C->asWFunction()->isGeneric());
			std::vector<WValue*> passingTypeArgs;
			for (auto& opt: argValOpts) {
				if (opt) passingTypeArgs.push_back((*opt)->asWType());
				else fatal("Passing a non-type to a generic type!");
			}
			auto optResV = C->asWFunction()->call(passingTypeArgs);
			assert("Generic must return value!" && optResV);
			WValue* wv = *optResV;
			assert("Evaluation of generic must be WType!" && wv->isWType());
			return ReturnVal(exp.val = wv->asWType());
		}
	}
}
void SigGenVisitor::visitArrayListExpr(ArrayListExprAST& exp) {
	for (ExprPtr& e: exp.list)
		visit(*e);
}
void SigGenVisitor::visitStringExpr(StringExprAST& exp) {}
void SigGenVisitor::visitDotExpr(DotExprAST& exp) {
	visit(*exp.lexp);
}
void SigGenVisitor::visitBracketExpr(BracketExprAST& exp) {
	visit(*exp.lexp);
	for (ExprPtr& e: exp.list)
		visit(*e);
}
/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
/* == == == == ==  Statement Visitors  == == == == == */
/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
void SigGenVisitor::visitLetStmt(LetStmtAST& stmt) {
	if (stmt.typeExpr)
		visit(**stmt.typeExpr);
	if (stmt.assTo)
		visit(**stmt.assTo);
	if (stmt.arraySpec)
		for (auto& e: *stmt.arraySpec)
			visit(*e);
}
void SigGenVisitor::visitBlockStmt(BlockStmtAST& stmt) {
	for (StmtPtr& s: stmt.contents)
		visit(*s);
}
void SigGenVisitor::visitIfStmt(IfStmtAST& stmt) {
	visit(*stmt.Cond);
	visit(*stmt.Then);
	if (stmt.Else)
		visit(**stmt.Else);
}
void SigGenVisitor::visitForStmt(ForStmtAST& stmt) {
	if (stmt.VarDef)
		visit(**stmt.VarDef);
	if (stmt.Cond)
		visit(**stmt.Cond);
	if (stmt.Iter)
		visit(**stmt.Iter);
	visit(*stmt.Loop);
}
void SigGenVisitor::visitWhileStmt(WhileStmtAST& stmt) {
	visit(*stmt.Cond);
	visit(*stmt.Loop);
}
void SigGenVisitor::visitExprStmt(ExprStmtAST& stmt) {
	visit(*stmt.expr);
}
void SigGenVisitor::visitRetStmt(RetStmtAST& stmt) {
	if (stmt.toRet)
		visit(**stmt.toRet);
}
/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
/* == == == == ==  Global Visitors  == == == == == */
/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
void SigGenVisitor::visitFunctionPrototypeGlbl(FunctionPrototypeAST& proto) {
	std::vector<WType*> argTys;
	if (definingClass)
		argTys.push_back((*definingClass)->getRefTo());
	for (auto& pair: proto.argTypes) {
		auto typeOpt = eval(*pair.first);
		argTys.push_back((*typeOpt)->asWType());
	}
	WType* retType = WType::getVoidTy();
	if (proto.retTypeExpr) {
		auto opt = eval(**proto.retTypeExpr);
		assert(opt);
		retType = (*opt)->asWType();
	}
	WType* FTP = WType::getFuncTy(retType, argTys);
	Function* LLF = Function::Create(*FTP->getLLFnType(), Function::LinkageTypes::ExternalLinkage, proto.name, *LLModule);
	WFunction* WF = new WFunction(LLF, proto.name, FTP);
	if (definingClass)
		LLF->getArg(0)->setName("this");
	for (int i = 0; i < proto.argTypes.size(); i++)
		LLF->getArg(i + (definingClass ? 1 : 0))->setName(proto.argTypes[i].second);
	proto.setValue(WF);
}
void SigGenVisitor::visitFunctionGlbl(FunctionAST& funDef) {
	visit(*funDef.proto);
	visit(*funDef.body);
	WFunction* WF = (*funDef.proto->getVal())->asWFunction();
	assert("Unable to create function from prototype!" && WF);
	funDef.setValue(WF);
}
void SigGenVisitor::visitClassGlbl(ClassAST& cls) {
	if (cls.generics) {
		//Class has generic definition
		if (&cls == toGenGeneric) {
			WType* WT = WType::getClassTy(cls.name);
			std::map<std::string, WValue*> conts;
			for (auto& vd: cls.vars) {
				auto opt = eval(*vd);
				assert("Unresolved type!" && (bool) opt);
				assert("Not a type!" && (*opt)->isWType());
				conts[vd->name] = (*opt)->asWType();
			}
			auto oldClass = definingClass;
			definingClass = WT;
			for (auto& fd: cls.funcs) {
				visit(*fd);
				WFunction* WF = (*fd->getVal())->asWFunction();
				conts[fd->proto->name] = WF;
				if (fd->proto->name == cls.name) WT->addConstructor(WF);
			}
			WT->setClassContents(conts);
			for (auto& fd: cls.funcs) {
				CodegenVisitor cgv;
				cgv.definingClass = WT;
				cgv.visit(*fd);
			}
			definingClass = oldClass;
			return ReturnVal(**genericResult = WT);
		} else
		{} //Do nothing; the class is a generic, but not the one being generated
	} else {
		//Non-generic class impl
		std::map<std::string, WValue*> conts;
		for (auto& vd: cls.vars)
			conts[vd->name] = (*eval(*vd))->asWType();
		auto oldClass = definingClass;
		definingClass = (*cls.getVal())->asWType();
		for (auto& fd: cls.funcs)
			conts[fd->proto->name] = (*eval(*fd))->asWFunction();
		(*cls.getVal())->asWType()->setClassContents(conts);
		definingClass = oldClass;
	}
}
void SigGenVisitor::visitClassVarDefGlbl(ClassVarDefAST& cvd) {
	visit(*cvd.typeExpr);
}
