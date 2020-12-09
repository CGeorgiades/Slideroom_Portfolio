/*
 * BuiltinOpers.h
 *
 *  Created on: Nov 21, 2020
 *      Author: chris
 */

#ifndef BUILTINS_H_
#define BUILTINS_H_
#include "Lib.h"
#include "Wrapper.h"
#include "Sym/Symbol.h"
/* All builtin operators */
/* Note: Some operators (e.g. 'I()') may have multiple overloads for the same arguments.
 * This is considered OK since they are used in syntax which will specify the return type */
extern std::map<std::string, std::vector<WFunction*>> BuiltinOps;
extern std::map<std::string, WValue*> BuiltinValues;
extern std::vector<Symbol*> BuiltinSymbols;
void initBuiltins();
std::optional<WFunction*> getImplicitCastFunc(WType* from, WType* to);
bool isImplicitCastFuncDefd(WType* from, WType* to);
#endif /* BUILTINS_H_ */
