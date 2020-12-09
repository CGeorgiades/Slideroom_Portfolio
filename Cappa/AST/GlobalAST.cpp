/*
 * GlobalAST.cpp
 *
 *  Created on: Nov 22, 2020
 *      Author: chris
 */

#include "GlobalAST.h"
#include <llvm/Support/raw_os_ostream.h>
#include <iostream>

/* Visitor */

void FunctionPrototypeAST::visit(Visitor& v) {v.visitFunctionPrototypeGlbl(*this);}
void FunctionAST::visit(Visitor& v) {v.visitFunctionGlbl(*this);}
void ClassVarDefAST::visit(Visitor& v) {v.visitClassVarDefGlbl(*this);}
void ClassAST::visit(Visitor& v) {v.visitClassGlbl(*this);}

std::optional<Symbol*> FunctionAST::findSym_Spec(StringRef symName) {
	return {};
}
