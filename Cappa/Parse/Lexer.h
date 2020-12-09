/*
 * Lexer.h
 *
 *  Created on: Nov 20, 2020
 *      Author: chris
 */

#ifndef LEXER_H_
#define LEXER_H_

#include "Lib.h"
#include "SrcInfo.h"
#include <optional>
#include <vector>
//The lexer returns characters [0-255] if it is an unknown character,
//otherwise one of these
enum TokenType {
	tok_uninit = -256, //<-- Special token value for unitialized tokens
	tok_eof = -1,
	//keywords
	tok_def = -255,
	tok_extern,
	tok_let,
	tok_if,
	tok_else,
	tok_return,
	tok_for,
	tok_while,
	tok_bool,
	tok_int,
	tok_class,
	//Valued-tokens
	tok_ident,
	tok_str,
	tok_double,
	tok_binop,
	tok_char
};
//Indicates if the operator can be a left-sided unary, a middle binary op, or a right-sided unary
enum BinopSidedness {
	B_LEFT = 1 << 0,
	B_MID = 1 << 1,
	B_RIGHT = 1 << 2
};
class Binop {
public:
	std::string str;
	int prec;
	int binSided;
	Binop(std::string str, int prec, int binSided): str(str), prec(prec), binSided(binSided) {}
};
class Token {
public:
	SrcLocation startLoc;
	TokenType tokenType;
	std::optional<bool> b;
	std::optional<int> i;
	std::optional<double> d;
	std::optional<char> c;
	std::optional<std::string> str;
	std::optional<Binop> binop;
	Token(): Token(SrcLocation(-1, -1), tok_uninit) {}
	Token(SrcLocation startLoc, TokenType tt): startLoc(startLoc), tokenType(tt) {}
	Token(SrcLocation startLoc, TokenType tt, const std::string& str): startLoc(startLoc), tokenType(tt), str(std::move(str)) {}
	Token(SrcLocation startLoc, TokenType tt, Binop binop): startLoc(startLoc), tokenType(tt), binop(std::move(binop)) {}
	Token(SrcLocation startLoc, double d): tokenType(tok_double), d(d) {}
	Token(SrcLocation startLoc, int i): tokenType(tok_int), i(i) {}
	Token(SrcLocation startLoc, bool b): tokenType(tok_bool), b(b) {}
	Token(SrcLocation startLoc, char c): tokenType(tok_char), c(c) {}
	bool operator ==(TokenType tt) const {return tokenType == tt;}
	bool operator !=(TokenType tt) const {return tokenType != tt;}
	bool operator ==(char ch) const {return tokenType == ch;}
	bool operator !=(char ch) const {return tokenType != ch;}
	explicit operator TokenType() const {return tokenType;}
	explicit operator int() const {return tokenType;}
	explicit operator char() const {return tokenType;}
};
extern SrcLocation curLoc;
extern const std::vector<Binop> BinopPrecedence;
bool isBinopChar(char ch);

extern std::string IdentStr;
extern double NumVal;

Token getTok();

#endif /* LEXER_H_ */
