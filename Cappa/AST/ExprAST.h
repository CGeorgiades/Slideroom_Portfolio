/*
 * ExprAST.h
 *
 *  Created on: Nov 22, 2020
 *      Author: chris
 */

#ifndef EXPRAST_H_
#define EXPRAST_H_
#include "AST.h"

class IntExprAST: public ExprAST {
public:
	int val;
	IntExprAST(int val): val(val) {}
	virtual void visit(Visitor& v);
};
class BoolExprAST: public ExprAST {
public:
	bool val;
	BoolExprAST(bool val): val(val) {}
	virtual void visit(Visitor& v);
};
class DoubleExprAST: public ExprAST {
public:
	double val;
	DoubleExprAST(double val): val(val) {}
	virtual void visit(Visitor& v);
};
class CharExprAST: public ExprAST {
public:
	char ch;
	CharExprAST(char ch): ch(ch) {}
	virtual void visit(Visitor& v);
};
class VarRefExprAST: public ExprAST {
public:
	std::string str;

	VarRefExprAST(StringRef name): str(std::move(name)) {}
	virtual void visit(Visitor& v);
};

class OpExprAST: public ExprAST {
public:
	std::string op;
	ExprPtr LHS;
	std::optional<ExprPtr> RHS;

	OpExprAST(std::string op, ExprPtr LHS,
			std::optional<ExprPtr> RHS):
							op(std::move(op)),	LHS(std::move(LHS)), RHS(std::move(RHS)){
		this->LHS->setParent(*this);
		if (RHS) (*this->RHS)->setParent(*this);
	}
	virtual void visit(Visitor& v);
};
class CallExprAST: public ExprAST {
public:
	ExprPtr callee;
	std::vector<ExprPtr> args;
	std::optional<WValue*> val;

	CallExprAST(ExprPtr callee, std::vector<std::unique_ptr<ExprAST>> args):
		callee(std::move(callee)), args(std::move(args)) {
		this->callee->setParent(*this);
		for (auto& arg: this->args) arg->setParent(*this);
	}
	virtual void visit(Visitor& v);
};
class ArrayListExprAST: public ExprAST {
public:
	std::vector<ExprPtr> list;
	ArrayListExprAST(std::vector<ExprPtr> list): list(std::move(list)) {
		for (auto& e: this->list) e->setParent(*this);
	}
	virtual void visit(Visitor& v);
};
class StringExprAST: public ExprAST {
public:
	std::string str;
	StringExprAST(std::string str): str(std::move(str)) {}
	virtual void visit(Visitor& v);
};
class DotExprAST: public ExprAST {
public:
	ExprPtr lexp;
	std::string elem;
	DotExprAST(ExprPtr lexp, std::string elem): lexp(std::move(lexp)), elem(elem) {this->lexp->setParent(*this);}
	virtual void visit(Visitor& v);
};
class BracketExprAST: public ExprAST {
public:
	ExprPtr lexp;
	std::vector<ExprPtr> list;
	BracketExprAST(ExprPtr lexp, std::vector<ExprPtr> list): lexp(std::move(lexp)), list(std::move(list)) {
		this->lexp->setParent(*this);
		for (auto& e: this->list) e->setParent(*this);
	}
	virtual void visit(Visitor& v);
};
#endif /* EXPRAST_H_ */
