/*
 * AST.h
 *
 *  Created on: Nov 20, 2020
 *      Author: chris
 */

#ifndef AST_H_
#define AST_H_

#include <Builtins.h>
#include "Lib.h"
#include "Parse/Lexer.h"

class StmtAST;
class ExprAST;
class GlobalAST;
class ProgramAST;
//Expression forward-decls
class IntExprAST;
class BoolExprAST;
class DoubleExprAST;
class CharExprAST;
class VarRefExprAST;
class OpExprAST;
class CallExprAST;
class ArrayListExprAST;
class StringExprAST;
class DotExprAST;
class BracketExprAST; //Expr '[]'
//Statement forward-decls
class LetStmtAST;
class BlockStmtAST;
class IfStmtAST;
class ForStmtAST;
class WhileStmtAST;
class ExprStmtAST;
class RetStmtAST;


//Global forward-decls
class FunctionPrototypeAST;
class FunctionAST;
class ClassAST;
class ClassVarDefAST;
#include "Visitor.h"
using namespace llvm;

class StmtAST: public Visitable {
	SrcLocation loc;
public:
	StmtAST(SrcLocation loc = curLoc): loc(loc) {}
	virtual ~StmtAST() {}
	const SrcLocation& getLoc() const {return loc;}
};
class ExprAST: public Visitable {
	SrcLocation loc;
public:
	ExprAST(SrcLocation loc = curLoc): loc(loc) {}
	virtual ~ExprAST() {}
	const SrcLocation& getLoc() const {return loc;}
};
class GlobalAST: public Visitable {
	SrcLocation loc;
public:
	GlobalAST(SrcLocation loc = curLoc): loc(loc) {}
	virtual ~GlobalAST() {}
	const SrcLocation& getLoc() const {return loc;}
};
#define ExprPtr std::unique_ptr<ExprAST>
#define StmtPtr std::unique_ptr<StmtAST>
#define GlobalPtr std::unique_ptr<GlobalAST>
class ProgramAST: public Visitable, public virtual Symbol {
	SrcLocation loc;
public:
	std::vector<GlobalPtr> glbls;
	ProgramAST(std::vector<GlobalPtr> glbls): Symbol(""), glbls(std::move(glbls)) {
		for (auto& g: this->glbls)
			if (g->isSym())
				addSymbol(*g->getSym());
			else g->setParent(*this);
	}
	virtual void visit(Visitor& v);
};

#include "AST/ExprAST.h"
#include "AST/StmtAST.h"
#include "AST/GlobalAST.h"

ExprPtr parseExpr();
StmtPtr parseStmt();
GlobalPtr parseGlobal();

extern WFunction* curFunction;
#endif /* AST_H_ */
