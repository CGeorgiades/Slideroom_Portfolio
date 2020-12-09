/*
 * Parser.cpp
 *
 *  Created on: Nov 20, 2020
 *      Author: chris
 */

#include "Parser.h"

/**
 * A few notes:
 * 	It is the "job" of the called to consume tokens
 * 	It is the "job" of the callee to ensure the called will be happy with the the next token
 * 	curToken should be used when looking at token properties (e.g. 'str', etc.)
 * 	nextToken should be (typically) used when looking at token tokenType
 */
Token curToken;
Token nextToken;
Token& accept() {
	curToken = nextToken;
	nextToken = getTok();
	return curToken;
}
void readyTokens() {
	while (nextToken.tokenType == tok_uninit)
		accept();
}
//accept to nextToken iff nextToken == tt
bool accept(TokenType tt) {
	if (nextToken == tt) {
		accept();
		return true;
	}
	return false;
}
//accept to nextToken iff nextToken == ch
bool accept(char ch) {
	if (nextToken == ch) {
		accept();
		return true;
	} else if (nextToken == tok_binop && nextToken.binop->str.size() == 1 && nextToken.binop->str[0] == ch) {
		accept();
		return true;
	}
	return false;
}
//accept nextToken iff nextToken == tt, else fail
void expect(TokenType tt) {
	if (!accept(tt))
		fatal("Expected TokenType %d", tt);
}
//accept nextToken iff nextToken == ch, else fail
void expect(char ch) {
	if (!accept(ch))
		fatal("Expected '%c'", ch);
}

/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
/* == == == == ==  Expr Parsing Functions == == == == == */
/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
//charexpr ::= char
ExprPtr parseCharExpr() {
	expect(tok_char);
	return std::make_unique<CharExprAST>(*curToken.c);
}
// numberexpr ::= number
ExprPtr parseNumberExpr() {
	if (accept(tok_int))
		return std::make_unique<IntExprAST>(*curToken.i);
	expect(tok_double);
	return std::make_unique<DoubleExprAST>(*curToken.d);
}
ExprPtr parseBoolExpr() {
	expect(tok_bool);
	return std::make_unique<BoolExprAST>(*curToken.b);
}
// parenexpr ::= '(' parenExpr ')'
ExprPtr parseParenExpr() {
	expect('(');
	auto V = parseExpr();
	if (!V) return nullptr;
	expect(')');
	return V;
}
// identexpr ::= Ident
ExprPtr parseIdentExpr() {
	expect(tok_ident);
	return std::make_unique<VarRefExprAST>(*curToken.str);
}
// callExpr ::= Expr '(' Expr* ')'
ExprPtr parseCallExpr(ExprPtr callee) {
	std::vector<ExprPtr> args;
	expect('(');
	if (!accept(')')) {
		do {
			args.push_back(parseExpr());
		} while(accept(','));
		expect(')');
	}
	return std::make_unique<CallExprAST>(std::move(callee), std::move(args));
}
// uses callExpr if there is a '(', else returns callee as-is
ExprPtr tryCallExpr(ExprPtr callee) {
	if (nextToken == '(')
		return parseCallExpr(std::move(callee));
	return callee;
}
//lower-precedence operators are "wider"
//e.g.:   a + b * c + d
//		= a + (b*c) + d
// '+' has lower precedence
// '*' has higher precedence
ExprPtr parseBinOpRHSExpr(int maxPrec, ExprPtr LHS) {
	while (accept(tok_binop)) { //accept binop, else return
		int binopPrec = curToken.binop->prec;
		//If this binop is lower precedence than here (wider), return what we have.
		if (binopPrec < maxPrec)
			return LHS;
		std::string opStr = curToken.binop->str;
		auto RHS = parsePrimaryExpr();
		//If next binop is higher-precedence (narrower), enter new binop parsing context, which will exit when the operators become wider
		if (nextToken == tok_binop && binopPrec < nextToken.binop->prec) {
			RHS = parseBinOpRHSExpr(binopPrec, std::move(RHS));
		}
		LHS = std::make_unique<OpExprAST>(std::move(opStr), std::move(LHS), std::move(RHS));
	}
	return LHS;
}
//ArrayList ::= '{' Expr* '}'
ExprPtr parseArrayListExpr() {
	expect('{');
	std::vector<ExprPtr> exps;
	do {
		exps.push_back(parseExpr());
	} while(accept(','));
	expect('}');
	return std::make_unique<ArrayListExprAST>(std::move(exps));
}
//bracketexpr ::= expr '[' Expr* ']'
ExprPtr parseBracketExpr(ExprPtr lexp) {
	expect('[');
	std::vector<ExprPtr> list;
	do {
		list.push_back(parseExpr());
	} while(accept(','));
	expect(']');
	return std::make_unique<BracketExprAST>(std::move(lexp), std::move(list));
}
// primary
//	::= IdentExpr
//	::= NumberExpr
//	::= parenExpr
//	::= boolExpr
ExprPtr parsePrimaryExpr() {
	ExprPtr e;
	if (accept(tok_binop)) {
		//Left-handed unary
		if (!(curToken.binop->binSided & B_LEFT)) {
			fatal("'%s' not a left-handed unary", curToken.binop->str.c_str());
		}
		return std::make_unique<OpExprAST>(curToken.binop->str, parsePrimaryExpr(), std::optional<ExprPtr>());
	} else if (nextToken == tok_ident) {
		e = tryCallExpr(parseIdentExpr());
	} else if (nextToken == tok_int || nextToken == tok_double)
		e = parseNumberExpr();
	else if (nextToken == tok_bool)
		e = parseBoolExpr();
	else if (nextToken == tok_char)
		e = parseCharExpr();
	else if (nextToken == '(')
		e = parseParenExpr();
	else if (nextToken == '{')
		e = parseArrayListExpr();
	else if (accept(tok_str))
		e = std::make_unique<StringExprAST>(*curToken.str);
	else fatal("Expected Identifier, Number, or '('");
	while (true)
		if (accept('.'))
			e = std::make_unique<DotExprAST>(std::move(e), (expect(tok_ident), *curToken.str));
		else if (nextToken == '[')
			e = parseBracketExpr(std::move(e));
		else if (nextToken == '(')
			e = parseCallExpr(std::move(e));
		else break;

	return e;
}
/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
/* == == == == == TypeExpr Parsing Functions == == == == == */
/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
ExprPtr parseIdentTypeExpr() {
	return parseIdentExpr();
}
//gentypeexpr ::= typeExpr '(' typeExpr* ')'
//Functionally identical to regular callexpr
ExprPtr parseGenericTypeExpr(ExprPtr e) {
	expect('(');
	std::vector<ExprPtr> gens;
	do {
		gens.push_back(parseTypeExpr());
	} while(accept(','));
	expect(')');
	return std::make_unique<CallExprAST>(std::move(e), std::move(gens));
}

/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
/* == == == == ==  Stmt Parsing Functions == == == == == */
/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */

//block ::= '{' Stmt* '}'
StmtPtr parseBlockStmt() {
	expect('{');
	std::vector<StmtPtr> contents;
	while (!accept('}'))
		contents.push_back(parseStmt());
	return std::make_unique<BlockStmtAST>(std::move(contents));
}
//varDecl ::= "let" Ident = Expr ';'
//		  ::= "let" Ident ':' TypeExpr ';'
//		  ::= "let" Ident ':' TypeExpr '[' Int* ']' ';'
//		  ::= "let" Ident ':' TypeExpr = Expr ';'
//		  ::= "let" Ident ':' TypeExpr '[' Int* ']' = Expr ';'
std::unique_ptr<LetStmtAST> parseLetStmt() {
	expect(tok_let);
	expect(tok_ident);
	std::string name = *curToken.str;
	if (accept(':')) {
		ExprPtr typeExpr = parseTypeExpr();
		std::vector<ExprPtr> arrSpec;
		if (accept('[')) {
			do {
				arrSpec.push_back(parseExpr());
			} while(accept(','));
			expect(']');
		}
		if (accept(';')) {
			return std::make_unique<LetStmtAST>(std::move(typeExpr), std::move(arrSpec), name);
		}
		expect(tok_binop);
		assert("Expected '='!" && curToken.binop->str == "=");
		ExprPtr expr = parseExpr();
		expect(';');
		return std::make_unique<LetStmtAST>(std::move(typeExpr), std::move(arrSpec),  name, std::move(expr));
	} else { //No specified type
		expect(tok_binop);
		assert("Expected '='!" && curToken.binop->str == "=");
		ExprPtr expr = parseExpr();
		expect(';');
		return std::make_unique<LetStmtAST>(name, std::move(expr));
	}
}
//ifStmt ::= "if" Expr Block  ("else" Block )
// TODO: ::= "if" '(' Expr ')' Stmt ("else" Stmt)
// 'else' should be made to always be followed by just Stmt regardless, not specifically a block
// But, while "if" requires one, so will the "else"
StmtPtr parseIfStmt() {
	expect(tok_if);
	auto Cond = parseExpr();
	auto Then = parseBlockStmt();
	if (accept(tok_else)) {
		auto Else = parseBlockStmt();
		return std::make_unique<IfStmtAST>(std::move(Cond), std::move(Then), std::move(Else));
	}
	return std::make_unique<IfStmtAST>(std::move(Cond), std::move(Then));
}
//forStmt ::= "for" '(' (LetStmt | ';') (Expr | ';') (Stmt | ';') ')' Stmt
StmtPtr parseForStmt() {
	expect(tok_for);
	expect('(');
	std::optional<std::unique_ptr<LetStmtAST>> varDef = {};
	std::optional<ExprPtr> cond = {};
	std::optional<ExprPtr> iter = {};
	StmtPtr loop;
	if (accept(';'))
	{}
	else varDef = parseLetStmt();
	if (accept(';'))
	{}
	else {
		cond = parseExpr();
		expect(';');
	}
	if (accept(')'))
	{}
	else {
		iter = parseExpr();
		expect(')');
	}
	loop = parseStmt();
	return std::make_unique<ForStmtAST>(std::move(varDef), std::move(cond), std::move(iter), std::move(loop));
}
//whileStmt ::= "while" Expr Block
StmtPtr parseWhileStmt() {
	expect(tok_while);
	ExprPtr cond = parseExpr();
	StmtPtr block = parseBlockStmt();
	return std::make_unique<WhileStmtAST>(std::move(cond), std::move(block));
}
StmtPtr parseRetStmt() {
	expect(tok_return);
	if (nextToken == ';')
		return std::make_unique<RetStmtAST>();
	else {
		auto E = std::make_unique<RetStmtAST>(parseExpr());
		expect(';');
		return E;
	}
}



/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
/* == == == == ==  Global Parsing Functions  == == == == == */
/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */

std::unique_ptr<FunctionPrototypeAST> parseFunctionPrototype() {
	expect(tok_ident);
	std::string fnName = *curToken.str;
	expect('(');
	//TODO switch to VarDeclExpr
	std::vector<std::pair<ExprPtr, std::string>> args;
	if (!accept(')')) {
		do {
			auto TE = parseTypeExpr();
			expect(tok_ident);
			//TODO add syntax to allow a user-defined function to take by ref
			args.push_back(std::pair<ExprPtr, std::string>(std::move(TE), *curToken.str));
		} while(accept(','));
		expect(')');
	}
	std::string retName = "";
	if (nextToken == tok_binop && nextToken.binop->str == "->") {
		expect(tok_binop);
		ExprPtr retTypeExpr = parseTypeExpr();
		return std::make_unique<FunctionPrototypeAST>(fnName, std::move(args), std::move(retTypeExpr));
	}
	return std::make_unique<FunctionPrototypeAST>(fnName, std::move(args));
}
std::unique_ptr<FunctionAST> parseFunctionDef() {
	expect(tok_def);
	auto proto = parseFunctionPrototype();
	StmtPtr body = parseBlockStmt();
	return std::make_unique<FunctionAST>(std::move(proto),
			std::move(body));
}
std::unique_ptr<FunctionPrototypeAST> parseExtern() {
	expect(tok_extern);
	expect(tok_def);
	auto F = parseFunctionPrototype();
	expect(';');
	return F;
}
std::unique_ptr<ClassVarDefAST> parseClassVarDef() {
	ExprPtr typeExpr = parseTypeExpr();
	expect(tok_ident);
	std::string name = *curToken.str;
	expect(';');
	return std::make_unique<ClassVarDefAST>(std::move(name), std::move(typeExpr));
}
GlobalPtr parseClass() {
	expect(tok_class);
	expect(tok_ident);
	std::string name = *curToken.str;
	std::vector<std::unique_ptr<ClassVarDefAST>> vars;
	std::vector<std::unique_ptr<FunctionAST>> funcs;
	std::vector<std::string> generics;
	if (accept('<')) {
		do {
			expect(tok_ident);
			generics.push_back(*curToken.str);
		} while (accept(','));
		expect('>');
	}
	expect('{');
	while (!accept('}')) {
		if (nextToken == tok_ident)
			vars.push_back(parseClassVarDef());
		else if (nextToken == tok_def) {
			funcs.push_back(parseFunctionDef());
		}
	}
	return std::make_unique<ClassAST>(std::move(name), std::move(vars), std::move(funcs), std::move(generics));
}
/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
/* == == == == ==  General Parsing Functions == == == == == */
/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */

ExprPtr parseExpr() {
	// Figure out expr
	ExprPtr LHS = parsePrimaryExpr();
	return parseBinOpRHSExpr(0, std::move(LHS));
}
ExprPtr parseTypeExpr() {
	ExprPtr e = parseIdentTypeExpr();
	while (nextToken == '(')
		e = parseGenericTypeExpr(std::move(e));
	return e;
}
StmtPtr parseStmt() {
	while (accept(';'))
	{}
	if (nextToken == tok_if)
		return parseIfStmt();
	else if (nextToken == tok_return)
		return parseRetStmt();
	else if (nextToken == tok_let)
		return parseLetStmt();
	else if (nextToken == tok_for)
		return parseForStmt();
	else if (nextToken == tok_while)
		return parseWhileStmt();
	else if (nextToken == '{')
		return parseBlockStmt();
	else {
		auto E = std::make_unique<ExprStmtAST>(parseExpr());
		expect(';');
		return E;
	}
}

std::unique_ptr<ProgramAST> parsePgm() {
	readyTokens();
	std::vector<GlobalPtr> glbls;
	while (1) {
		if (nextToken == tok_eof) {
			accept();
			return std::make_unique<ProgramAST>(std::move(glbls));
		} else if (accept(';'))
			continue;
		else if (nextToken == tok_def) {
			glbls.push_back(parseFunctionDef());
		} else if (nextToken == tok_extern) {
			glbls.push_back(parseExtern());
		} else if (nextToken == tok_class) {
			glbls.push_back(parseClass());
		} else {
			error("Unexpected token!");
			accept();
		}
	}
}
