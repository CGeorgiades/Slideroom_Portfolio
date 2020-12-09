/*
 * SigGenVisitor.h
 *
 *  Created on: Dec 3, 2020
 *      Author: chris
 */

#ifndef VISITORS_SIGGENVISITOR_H_
#define VISITORS_SIGGENVISITOR_H_

#include "Visitor.h"

/*
 * Called after TypeDefVisitor
 * By this point, all types and generics have been initialized; they need to be instantiated
 *
 * SigGenVisitor has two states:
 * 	Standard
 * 	Generic
 *
 * Standard:
 * 	Go down the AST, find every type, resolve it
 * 	Define class bodies and function protos/defs
 *
 * Generic:
 * 	Constructed with an ArrayRef<WType*> consisting of the types passed to the generic constructor
 * 	Identical to standard, accept the generic types will resolve to the types passed instead
 */
class SigGenVisitor: public Visitor {
public:
	std::optional<WValue*> Return;
	void ReturnVal(WValue*);
	void ReturnVal(std::optional<WValue*>);
	void NoReturn();
	std::optional<WValue*> eval(Visitable&);

	std::optional<WType*> definingClass; //Used when defining a class, primarily for function prototype definitions

	std::optional<Symbol*> getSym(Visitable& v, StringRef symName);
	std::optional<std::vector<Symbol*>> genSyms;
	std::optional<Visitable*> toGenGeneric;
	std::optional<WType**> genericResult;
	SigGenVisitor() {}
	SigGenVisitor(Visitable* toGen, std::map<std::string, WType*> genArgs, WType** genResult);
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
#endif /* VISITORS_SIGGENVISITOR_H_ */
