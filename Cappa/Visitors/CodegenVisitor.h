/*
 * CodeGenVisitor.h
 *
 *  Created on: Nov 24, 2020
 *      Author: chris
 */

#ifndef VISITORS_CODEGENVISITOR_H_
#define VISITORS_CODEGENVISITOR_H_

#include "Visitor.h"
class CodegenVisitor: public Visitor {
public:
	std::optional<WValue*> Return; //'Return' Value from codegen'd object, doesn't always apply

	std::map<std::string, std::vector<WFunction*>> NamedFunctions;
	std::map<std::string, WValue*> NamedValues; //All values which are denoted solely by their name (e.g. vars and classses)

	std::vector<WValue*> callingFrom;	//Used when calling a class method: 'x.y();'
										//Is used as a stack for cases like: 'x.y(c.d())'
										//Is a reference to the class

	std::optional<WType*> definingClass; //Used when defining a class, primarily for function prototype definitions
	std::optional<bool> isConstructor; //Used to determine if a class method is the class constructor
	std::map<std::string, WValue*> curClassVals; //Used when defining a class, var<->type assosiations
	std::optional<WFunction*> curFunction;

	void ReturnVal(WValue*);
	void ReturnVal(std::optional<WValue*>);
	void NoReturn();
	WValue* eval(Visitable& v);
	/* Expression Visitor Methods */
	//Used to indicate that when a function call is chosen among several options, prefer a function that is compatible/perfectly matching this vector's types
	std::vector<WType*> preferredFunctionArgTypes;

	virtual void visitProgram(ProgramAST&);
	/* Expression Visitor Methods */
	virtual void visitIntExpr(IntExprAST&);
	virtual void visitBoolExpr(BoolExprAST& );
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

#endif /* VISITORS_CODEGENVISITOR_H_ */
