/*
 * TypeDefVisitor.cpp
 *  Created on: Nov 24, 2020
 *      Author: chris
 */


#include "TypeDefVisitor.h"
#include "SigGenVisitor.h"
#include "Wrapper.h"


void TypeDefVisitor::visitProgram(ProgramAST& pgm) {
	for (GlobalPtr& g: pgm.glbls)
		visit(*g);
}
/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
/* == == == == ==  Expression Visitors == == == == == */
/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
void TypeDefVisitor::visitIntExpr(IntExprAST& exp) {}
void TypeDefVisitor::visitBoolExpr(BoolExprAST& exp) {}
void TypeDefVisitor::visitDoubleExpr(DoubleExprAST& exp) {}
void TypeDefVisitor::visitCharExpr(CharExprAST&) {}
void TypeDefVisitor::visitVarRefExpr(VarRefExprAST& exp) {}
void TypeDefVisitor::visitOpExpr(OpExprAST& exp) {
	visit(*exp.LHS);
	if (exp.RHS)
		visit(**exp.RHS);
}
void TypeDefVisitor::visitCallExpr(CallExprAST& exp) {
	visit(*exp.callee);
	for (ExprPtr& e: exp.args)
		visit(*e);
}
void TypeDefVisitor::visitArrayListExpr(ArrayListExprAST& exp) {
	for (ExprPtr& e: exp.list)
		visit(*e);
}
void TypeDefVisitor::visitStringExpr(StringExprAST& exp) {}
void TypeDefVisitor::visitDotExpr(DotExprAST& exp) {
	visit(*exp.lexp);
}
void TypeDefVisitor::visitBracketExpr(BracketExprAST& exp) {
	visit(*exp.lexp);
	for (ExprPtr& e: exp.list)
		visit(*e);
}
/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
/* == == == == ==  Statement Visitors  == == == == == */
/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
void TypeDefVisitor::visitLetStmt(LetStmtAST& stmt) {
	if (stmt.typeExpr)
		visit(**stmt.typeExpr);
	if (stmt.assTo)
		visit(**stmt.assTo);
	if (stmt.arraySpec)
		for (auto& e: *stmt.arraySpec)
			visit(*e);
}
void TypeDefVisitor::visitBlockStmt(BlockStmtAST& stmt) {
	for (StmtPtr& s: stmt.contents)
		visit(*s);
}
void TypeDefVisitor::visitIfStmt(IfStmtAST& stmt) {
	visit(*stmt.Cond);
	visit(*stmt.Then);
	if (stmt.Else)
		visit(**stmt.Else);
}
void TypeDefVisitor::visitForStmt(ForStmtAST& stmt) {
	if (stmt.VarDef)
		visit(**stmt.VarDef);
	if (stmt.Cond)
		visit(**stmt.Cond);
	if (stmt.Iter)
		visit(**stmt.Iter);
	visit(*stmt.Loop);
}
void TypeDefVisitor::visitWhileStmt(WhileStmtAST& stmt) {
	visit(*stmt.Cond);
	visit(*stmt.Loop);
}
void TypeDefVisitor::visitExprStmt(ExprStmtAST& stmt) {
	visit(*stmt.expr);
}
void TypeDefVisitor::visitRetStmt(RetStmtAST& stmt) {
	if (stmt.toRet)
		visit(**stmt.toRet);
}
/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
/* == == == == ==  Global Visitors  == == == == == */
/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
void TypeDefVisitor::visitFunctionPrototypeGlbl(FunctionPrototypeAST& proto) {
	for (auto& pair: proto.argTypes)
		visit(*pair.first);
	if (proto.retTypeExpr)
		visit(**proto.retTypeExpr);
}
void TypeDefVisitor::visitFunctionGlbl(FunctionAST& funDef) {
	visit(*funDef.proto);
	visit(*funDef.body);
}
void TypeDefVisitor::visitClassGlbl(ClassAST& cls) {
	if (cls.generics) {
		WFunction *genericGen = new WFunction(cls.name, [&](ArrayRef<WType*> args)->WType*{
			assert("# of generics passed != # expected!" && (args.size() == cls.generics->size()));


			//Implement the class
			std::map<std::string, WType*> map;
			for (int i = 0; i < cls.generics->size(); i++) {
				map[(*cls.generics)[i]] = args[i];
			}
			WType* genRes;
			SigGenVisitor sgv(&cls, map, &genRes);
			(*cls.getVal())->asWFunction();
			return genRes;
		});
		cls.setValue(genericGen);
	} else {
		cls.setValue(WType::getClassTy(cls.name));
		for (auto& vd: cls.vars)
			visit(*vd);
		for (auto& fd: cls.funcs)
			visit(*fd);
	}

}
void TypeDefVisitor::visitClassVarDefGlbl(ClassVarDefAST& cvd) {
	visit(*cvd.typeExpr);
}
