/*
 * BlankVisitor.h
 *	Blank Visitor file meant to be copied for creating new visitors
 *  Created on: Nov 24, 2020
 *      Author: chris
 */

#ifndef BLANKVISITOR_H_
#define BLANKVISITOR_H_
#pragma error "DO NOT INCLUDE THIS FILE!"
#include "Visitor.h"

//Class Template
class BlankVisitor: public Visitor {
public:
	virtual void visitProgram(ProgramAST&);
	/* Expression Visitor Methods */
	virtual void visitIntExpr(IntExprAST&);
	virtual void visitBoolExpr(BoolExprAST&);
	virtual void visitDoubleExpr(DoubleExprAST&);
	virtual void visitCharExpr(CharExprAST&);
	virtual void visitVarRefExpr(VarRefExprAST&);
	virtual void visitOpExpr(OpExprAST&);
	virtual void visitCallExpr(CallExprAST&);
	virtual void visitArrayListExpr(ArrayListExprAST&);
	virtual void visitStringExpr(StringExprAST&);
	virtual void visitDotExpr(DotExprAST&);
	virtual void visitBracketExpr(BracketExprAST&);
	/* Statement Visitor Methods */
	virtual void visitLetStmt(LetStmtAST&);
	virtual void visitBlockStmt(BlockStmtAST&);
	virtual void visitIfStmt(IfStmtAST&);
	virtual void visitForStmt(ForStmtAST&);
	virtual void visitWhileStmt(WhileStmtAST&);
	virtual void visitExprStmt(ExprStmtAST&);
	virtual void visitRetStmt(RetStmtAST&);
	/* Global Visitor Methods */
	virtual void visitFunctionPrototypeGlbl(FunctionPrototypeAST&);
	virtual void visitFunctionGlbl(FunctionAST&);
	virtual void visitClassGlbl(ClassAST&);
	virtual void visitClassVarDefGlbl(ClassVarDefAST&);
};
//Method Implementation Template


void BlankVisitor::visitProgram(ProgramAST&) {}
/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
/* == == == == ==  Expression Visitors == == == == == */
/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
void BlankVisitor::visitIntExpr(IntExprAST& exp) {}
void BlankVisitor::visitBoolExpr(BoolExprAST& exp) {}
void BlankVisitor::visitDoubleExpr(DoubleExprAST& exp) {}
void BlankVisitor::visitCharExpr(CharExprAST&) {}
void BlankVisitor::visitVarRefExpr(VarRefExprAST& exp) {}
void BlankVisitor::visitOpExpr(OpExprAST& exp) {}
void BlankVisitor::visitCallExpr(CallExprAST& exp) {}
void BlankVisitor::visitArrayListExpr(ArrayListExprAST& exp) {}
void BlankVisitor::visitStringExpr(StringExprAST& exp) {}
void BlankVisitor::visitDotExpr(DotExprAST& exp) {}
void BlankVisitor::visitBracketExpr(BracketExprAST& exp) {}
/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
/* == == == == ==  Statement Visitors  == == == == == */
/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
void BlankVisitor::visitLetStmt(LetStmtAST& stmt) {}
void BlankVisitor::visitBlockStmt(BlockStmtAST& stmt) {}
void BlankVisitor::visitIfStmt(IfStmtAST& stmt) {}
void BlankVisitor::visitForStmt(ForStmtAST& stmt) {}
void BlankVisitor::visitWhileStmt(WhileStmtAST& stmt) {}
void BlankVisitor::visitExprStmt(ExprStmtAST& stmt) {}
void BlankVisitor::visitRetStmt(RetStmtAST& stmt) {}
/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
/* == == == == ==  Global Visitors  == == == == == */
/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
void BlankVisitor::visitFunctionPrototypeGlbl(FunctionPrototypeAST& proto) {}
void BlankVisitor::visitFunctionGlbl(FunctionAST& funDef) {}
void BlankVisitor::visitClassGlbl(ClassAST& cls) {}
void BlankVisitor::visitClassVarDefGlbl(ClassVarDefAST& cvd) {}

#endif /* BLANKVISITOR_H_ */
