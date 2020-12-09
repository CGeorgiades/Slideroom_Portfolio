/*
 * GlobalAST.h
 *
 *  Created on: Nov 22, 2020
 *      Author: chris
 */

#ifndef GLOBALAST_H_
#define GLOBALAST_H_
#include "AST.h"

class FunctionPrototypeAST: public GlobalAST, public virtual Symbol {
public:
	std::string name;
	//vector <tuple <TypeExpr, isRef, argName>>
	std::vector<std::pair<ExprPtr, std::string>> argTypes;
	std::optional<ExprPtr> retTypeExpr;

	FunctionPrototypeAST(StringRef name, std::vector<std::pair<ExprPtr, std::string>> argTypes, ExprPtr retTypeExpr):
		 Symbol(name, this), name(name), argTypes(std::move(argTypes)), retTypeExpr(std::move(retTypeExpr)) {\
		for (auto& pair: this->argTypes) pair.first->setParent(*this);
		(*this->retTypeExpr)->setParent(*this);
	}
	FunctionPrototypeAST(StringRef name, std::vector<std::pair<ExprPtr, std::string>> argTypes):
		 Symbol(name, this), name(name), argTypes(std::move(argTypes)) {
		for (auto& pair: this->argTypes) pair.first->setParent(*this);
	}
	const std::vector<std::pair<ExprPtr, std::string>>& getArgs() {return argTypes;}
	const std::string& getName() const {return name;}
	virtual void visit(Visitor& v);
};
class FunctionAST: public GlobalAST, public virtual Symbol {
public:
	std::unique_ptr<FunctionPrototypeAST> proto;
	StmtPtr body;
	FunctionAST(std::unique_ptr<FunctionPrototypeAST> proto,
				StmtPtr body):
				 Symbol(proto->name, this), proto(std::move(proto)), body(std::move(body)) {
		this->proto->setParent(*this); this->body->setParent(*this);
	}
	//Special Symbol Resolution: If a class method, check through the class's var defintions
	virtual std::optional<Symbol*> findSym_Spec(StringRef symName);
	virtual void visit(Visitor& v);
};
class ClassVarDefAST: public StmtAST, public virtual Symbol {
public:
	std::string name;
	ExprPtr typeExpr;
	ClassVarDefAST(std::string name, ExprPtr typeExpr):
		 Symbol(name, this), name(name), typeExpr(std::move(typeExpr)) {this->typeExpr->setParent(*this);}
	virtual void visit(Visitor& v);
};
class ClassAST: public GlobalAST, public virtual Symbol {
public:
	std::string name;
	std::vector<std::unique_ptr<ClassVarDefAST>> vars;
	std::vector<std::unique_ptr<FunctionAST>> funcs;
	std::optional<std::vector<std::string>> generics;
	ClassAST(std::string name, std::vector<std::unique_ptr<ClassVarDefAST>> vars, std::vector<std::unique_ptr<FunctionAST>> funcs, std::vector<std::string> generics):
		 Symbol(name, this), name(name), vars(std::move(vars)), funcs(std::move(funcs)), generics(generics.size() == 0 ? std::optional<std::vector<std::string>>() : std::move(generics)) {
		for (auto& v: this->vars) v->setParent(*this);
		for (auto& f: this->funcs) f->setParent(*this);
	}
	virtual void visit(Visitor& v);
};


#endif /* GLOBALAST_H_ */
