/*
 * TypeDefVisitor.h
 *  Created on: Nov 24, 2020
 *      Author: chris
 */

#ifndef TYPEDEFVISITOR_H_
#define TYPEDEFVISITOR_H_
#include "Visitor.h"

//Define *all* types and generics that can/will be used in the language
//No types (or any symbols) should in any way be attempted to be resolved yet
//Note that this defines only type names, but nothing relating to type bodies
class TypeDefVisitor: public Visitor {
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


#endif /* TypeDefVisitor_H_ */
