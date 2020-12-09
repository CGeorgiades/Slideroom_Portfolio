/*
 * ExprAST.cpp
 *
 *  Created on: Nov 22, 2020
 *      Author: chris
 */

#include "ExprAST.h"
#include "Parse/Parser.h"
#include <llvm/Support/raw_os_ostream.h>
#include <iostream>



/* == visitor == */

void IntExprAST::visit(Visitor& v) {v.visitIntExpr(*this);}
void BoolExprAST::visit(Visitor& v) {v.visitBoolExpr(*this);}
void DoubleExprAST::visit(Visitor& v) {v.visitDoubleExpr(*this);}
void CharExprAST::visit(Visitor& v) {v.visitCharExpr(*this);}
void VarRefExprAST::visit(Visitor& v) {v.visitVarRefExpr(*this);}
void OpExprAST::visit(Visitor& v) {v.visitOpExpr(*this);}
void CallExprAST::visit(Visitor& v) {v.visitCallExpr(*this);}
void ArrayListExprAST::visit(Visitor& v) {v.visitArrayListExpr(*this);}
void StringExprAST::visit(Visitor& v) {v.visitStringExpr(*this);}
void DotExprAST::visit(Visitor& v) {v.visitDotExpr(*this);}
void BracketExprAST::visit(Visitor& v) {v.visitBracketExpr(*this);}
