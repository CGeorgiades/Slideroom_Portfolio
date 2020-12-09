/*
 * AST.cpp
 *
 *  Created on: Nov 20, 2020
 *      Author: chris
 */

#include "AST.h"
#include <iostream>


void ProgramAST::visit(Visitor& v) {
	v.visitProgram(*this);
}
