/*
 * StmtAST.cpp
 *
 *  Created on: Nov 22, 2020
 *      Author: chris
 */

#include "StmtAST.h"
#include "Parse/Parser.h"

/* == Visitor == */

void LetStmtAST::visit(Visitor& v) {v.visitLetStmt(*this);}
void BlockStmtAST::visit(Visitor& v) {v.visitBlockStmt(*this);}
void IfStmtAST::visit(Visitor& v) {v.visitIfStmt(*this);}
void ForStmtAST::visit(Visitor& v) {v.visitForStmt(*this);}
void WhileStmtAST::visit(Visitor& v) {v.visitWhileStmt(*this);}
void ExprStmtAST::visit(Visitor& v) {v.visitExprStmt(*this);}
void RetStmtAST::visit(Visitor& v) {v.visitRetStmt(*this);}


std::optional<Symbol*> ForStmtAST::findSym_Spec(StringRef symName) {
	if (VarDef) {
		if ((*VarDef)->name == symName) return (*VarDef)->getSym();
	}
	return {};
}
