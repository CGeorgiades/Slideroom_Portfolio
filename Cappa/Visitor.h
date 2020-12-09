/*
 * Visitor.h
 *
 *  Created on: Nov 24, 2020
 *      Author: chris
 */

#ifndef VISITOR_H_
#define VISITOR_H_
class Visitable;
#include "Sym/Symbol.h"

class Visitor;
class Visitable: public virtual MaySymbol {
public:
	Visitable() {}
	virtual void visit(Visitor& v)=0;
	virtual ~Visitable() {}
};
#include "AST.h"

class Visitor {
public:
	void visit(Visitable* v) {v->visit(*this);}
	void visit(Visitable& v) {v.visit(*this);}

	virtual void visitProgram(ProgramAST&) = 0;
	/* Expression Visitor Methods */
	virtual void visitIntExpr(IntExprAST&) = 0;
	virtual void visitBoolExpr(BoolExprAST&) = 0;
	virtual void visitDoubleExpr(DoubleExprAST&) = 0;
	virtual void visitCharExpr(CharExprAST&) = 0;
	virtual void visitVarRefExpr(VarRefExprAST&) = 0;
	virtual void visitOpExpr(OpExprAST&) = 0;
	virtual void visitCallExpr(CallExprAST&) = 0;
	virtual void visitArrayListExpr(ArrayListExprAST&) = 0;
	virtual void visitStringExpr(StringExprAST&) = 0;
	virtual void visitDotExpr(DotExprAST&) = 0;
	virtual void visitBracketExpr(BracketExprAST&) = 0;
	/* Statement Visitor Methods */
	virtual void visitLetStmt(LetStmtAST&) = 0;
	virtual void visitBlockStmt(BlockStmtAST&) = 0;
	virtual void visitIfStmt(IfStmtAST&) = 0;
	virtual void visitForStmt(ForStmtAST&) = 0;
	virtual void visitWhileStmt(WhileStmtAST&) = 0;
	virtual void visitExprStmt(ExprStmtAST&) = 0;
	virtual void visitRetStmt(RetStmtAST&) = 0;
	/* Global Visitor Methods */
	virtual void visitFunctionPrototypeGlbl(FunctionPrototypeAST&) = 0;
	virtual void visitFunctionGlbl(FunctionAST&) = 0;
	virtual void visitClassGlbl(ClassAST&) = 0;
	virtual void visitClassVarDefGlbl(ClassVarDefAST&) = 0;
	virtual ~Visitor() {}
};

#endif /* VISITOR_H_ */
