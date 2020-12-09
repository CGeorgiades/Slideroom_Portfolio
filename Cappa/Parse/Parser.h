/*
 * Parser.h
 *
 *  Created on: Nov 20, 2020
 *      Author: chris
 */

#ifndef PARSER_H_
#define PARSER_H_
#include "Lexer.h"
#include "AST.h"
extern Token curToken;
extern Token nextToken;
Token& accept();
bool accept(TokenType);
bool accept(char);
void expect(TokenType);
void expect(char);


ExprPtr parseExpr();
ExprPtr parseTypeExpr();
StmtPtr parseStmt();
GlobalPtr parseGlobal();

//Expressions
ExprPtr parseCharExpr();
ExprPtr parseNumberExpr();
ExprPtr parseBoolExpr();
ExprPtr parseVarRefExpr();
ExprPtr parseBinaryExpr();
ExprPtr parseCallExpr();
ExprPtr parseArrayListExpr();
ExprPtr parsePrimaryExpr();
ExprPtr parseBinOpRHSExpr(int, ExprPtr);
ExprPtr parseBracketExpr(ExprPtr);

//Type Expressions
//Semantically have identical representations as regular expressions
//Parser-wise are different
ExprPtr parseIdentTypeExpr();
ExprPtr parseGenericTypeExpr(ExprPtr e);

//Statements
std::unique_ptr<LetStmtAST> parseLetStmt();
StmtPtr parseBlockStmt();
StmtPtr parseIfStmt();
StmtPtr parseForStmt();
StmtPtr parseWhileStmt();
StmtPtr parseExprStmt();
StmtPtr parseRetStmt();

//Globals
std::unique_ptr<FunctionPrototypeAST> parseFunctionPrototype();
std::unique_ptr<FunctionPrototypeAST> parseExtern();
std::unique_ptr<FunctionAST> parseFunctionDef();
std::unique_ptr<ClassVarDefAST> parseClassVarDef();
GlobalPtr parseClass();


std::unique_ptr<ProgramAST> parsePgm();
#endif /* PARSER_H_ */
