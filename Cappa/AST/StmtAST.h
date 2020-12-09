/*
 * StmtAST.h
 *
 *  Created on: Nov 22, 2020
 *      Author: chris
 */

#ifndef STMTAST_H_
#define STMTAST_H_
#include "AST.h"

class LetStmtAST: public virtual StmtAST, public Symbol {
public:
	std::optional<ExprPtr> typeExpr;
	std::optional<std::vector<ExprPtr>> arraySpec;
	std::string name;
	std::optional<ExprPtr> assTo;
	LetStmtAST(std::string name, ExprPtr assTo): Symbol(name, this), name(name), assTo(std::move(assTo))  {
		(*this->assTo)->setParent(*this);
	}
	LetStmtAST(ExprPtr typeExpr, std::string name): Symbol(name, this), typeExpr(std::move(typeExpr)), name(std::move(name)) {
		(*this->typeExpr)->setParent(*this);
	}
	LetStmtAST(ExprPtr typeExpr, std::string name, ExprPtr assTo): Symbol(name, this), typeExpr(std::move(typeExpr)), name(name), assTo(std::move(assTo)) {
		(*this->assTo)->setParent(*this); (*this->typeExpr)->setParent(*this);
	}
	//typeExpr with ArraySpec
	LetStmtAST(ExprPtr typeExpr, std::vector<ExprPtr> arraySpec, std::string name): Symbol(name, this), typeExpr(std::move(typeExpr)),
			arraySpec(arraySpec.size() == 0 ? std::optional<std::vector<ExprPtr>>() : std::move(arraySpec)), name(std::move(name)) {
		(*this->typeExpr)->setParent(*this);
		if (this->arraySpec) for (auto& e: *this->arraySpec) e->setParent(*this);
	}
	LetStmtAST(ExprPtr typeExpr, std::vector<ExprPtr> arraySpec, std::string name, ExprPtr assTo): Symbol(name, this), typeExpr(std::move(typeExpr)),
			arraySpec(arraySpec.size() == 0 ? std::optional<std::vector<ExprPtr>>() : std::move(arraySpec)), name(name), assTo(std::move(assTo)) {
		(*this->assTo)->setParent(*this); (*this->typeExpr)->setParent(*this);
		if (this->arraySpec) for (auto& e: *this->arraySpec) e->setParent(*this);
	}
	virtual void visit(Visitor& v);
};
class BlockStmtAST: public virtual StmtAST {
public:
	std::vector<StmtPtr> contents;
	BlockStmtAST(std::vector<StmtPtr> contents):
		contents(std::move(contents)) {for (auto& sp: this->contents) sp->setParent(*this);}
	virtual void visit(Visitor& v);
};
class IfStmtAST: public virtual StmtAST {
public:
	ExprPtr Cond;
	StmtPtr Then;
	std::optional<StmtPtr> Else;
	IfStmtAST(ExprPtr Cond, StmtPtr Then):
		Cond(std::move(Cond)), Then(std::move(Then)) {this->Cond->setParent(*this); this->Then->setParent(*this);}
	IfStmtAST(ExprPtr Cond, StmtPtr Then, StmtPtr Else):
		Cond(std::move(Cond)), Then(std::move(Then)), Else(std::move(Else)) {
		this->Cond->setParent(*this); this->Then->setParent(*this); (*this->Else)->setParent(*this);
	}
	virtual void visit(Visitor& v);
};

class ForStmtAST: public virtual StmtAST {
public:
	std::optional<std::unique_ptr<LetStmtAST>> VarDef;
	std::optional<ExprPtr> Cond;
	std::optional<ExprPtr> Iter;
	StmtPtr Loop;
	ForStmtAST(std::optional<std::unique_ptr<LetStmtAST>> VarDef, std::optional<ExprPtr> Cond, std::optional<ExprPtr> Iter, StmtPtr Loop):
		VarDef(std::move(VarDef)), Cond(std::move(Cond)), Iter(std::move(Iter)), Loop(std::move(Loop)) {
		if (this->VarDef) (*this->VarDef)->setParent(*this);
		if (this->Cond) (*this->Cond)->setParent(*this);
		if (this->Iter) (*this->Iter)->setParent(*this);
		this->Loop->setParent(*this);
	}
	//Special symbol resolution: check the 'let' statement
	virtual std::optional<Symbol*> findSym_Spec(StringRef symName);
	virtual void visit(Visitor& v);
};
class WhileStmtAST: public virtual StmtAST {
public:
	ExprPtr Cond;
	StmtPtr Loop;
	WhileStmtAST(ExprPtr Cond, StmtPtr Loop):
		Cond(std::move(Cond)), Loop(std::move(Loop)) {this->Cond->setParent(*this); this->Loop->setParent(*this);}
	virtual void visit(Visitor& v);
};
class ExprStmtAST: public virtual StmtAST {
public:
	ExprPtr expr;
	ExprStmtAST(ExprPtr expr):
		expr(std::move(expr)) {this->expr->setParent(*this);}
	virtual void visit(Visitor& v);
};
class RetStmtAST: public virtual StmtAST {
public:
	std::optional<ExprPtr> toRet;
	RetStmtAST() {}
	RetStmtAST(ExprPtr toRet):
		toRet(std::move(toRet)) {(*this->toRet)->setParent(*this);}
	virtual void visit(Visitor& v);
};


#endif /* STMTAST_H_ */
