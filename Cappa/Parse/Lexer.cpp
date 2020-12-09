/*
 * Lexer.cpp
 *
 *  Created on: Nov 20, 2020
 *      Author: chris
 */

#include "Lexer.h"
#include "Lib.h"
#include "stdlib.h"

//To add a new operator, add it here with a precedence
//Lower precedence operators bind more 'widely', or looser
//Higher precedence operators bind more 'narrowly' or tighter
//Actual implementations for the operator can be added in Builtins
const std::vector<Binop> BinopPrecedence = {
		{"=", 01, B_MID},
		{"||", 10, B_MID},
		{"&&", 10, B_MID},
		{"==", 20, B_MID},
		{"!=", 20, B_MID},
		{">", 30, B_MID},
		{"<", 30, B_MID},
		{">=", 30, B_MID},
		{"<=", 30, B_MID},
		{"+", 40, B_MID},
		{"-", 40, B_MID | B_LEFT},
		{"*", 50, B_MID},
		{"/", 50, B_MID},
		{"%", 50, B_MID},
		{"->", 60, B_MID}
};
bool isBinopChar(char ch) {
	for (auto& pair: BinopPrecedence) {
		if (pair.str.find(ch) != -1)
			return true;
	}
	return false;
}
SrcLocation curLoc(1, 0);
static char lastChar = ' ';
static char advance() {
	curLoc.advance(lastChar);
	return getChar();
}
Token _getTok() {
	if (lastChar == EOF)
		return Token(curLoc, tok_eof);
	while (isSpace(lastChar))
		lastChar = advance();
	//Remove a Unix Shebang from parsing at the start of the pgm
	if (curLoc.col == 1 && curLoc.line == 1 && lastChar == '#') {
		do {lastChar = advance();} while(lastChar != '\n' && lastChar != EOF);
		lastChar = advance();
	}
	SrcLocation start = curLoc;
	if (lastChar == EOF)
		return Token(start, tok_eof);
	if (lastChar == '"') {
		std::string str;
		lastChar = advance();
		while(lastChar != '"') {
			if (lastChar == '\\') {
				lastChar = advance();
				if (lastChar == 'n') str += '\n';
				else if (lastChar == '\\') str += '\\';
				else if (lastChar == '"') str += '"';
				//TODO change this to handle hex codes, right now it's decimal
				else if (lastChar >= '0' && lastChar <= '9') {
					char ch = 0;
					do {
						ch *= 10;
						ch += lastChar - '0';
						lastChar = advance();
					} while (lastChar >= '0' && lastChar <= '9');
					str += ch;
					continue;
				}
				else {
					error("Unexpected character '%c'", lastChar);
					str += lastChar;
				}
			} else str += lastChar;
			lastChar = advance();
		}
		lastChar = advance();
		return Token(start, tok_str, str);
	}
	if (lastChar == '\'') {
		lastChar = advance();
		char ch = lastChar;
		if (lastChar == '\\') {
			lastChar = advance();
			switch (lastChar){
			case 'n':
				ch = '\n';
				break;
			case '0':
				ch = 0;
				break;
			case '\\':
				ch = '\\';
				break;
			default:
				error("Unnecessary '\\'");
				ch = lastChar;
			}
		}
		lastChar = advance();
		if (lastChar != '\'') {
			error("Expected '");
		}
		lastChar = advance();
		return Token(start, ch);
	}
	if (isAlpha(lastChar) || lastChar == '_') {
		std::string IdentStr;
		IdentStr = lastChar;
		while (isAlNum(lastChar = advance()))
			IdentStr += lastChar;

		if (IdentStr == "def")
			return Token(start, tok_def, "def");
		else if (IdentStr == "class")
			return Token(start, tok_class, "class");
		else if (IdentStr == "extern")
			return Token(start, tok_extern, "extern");
		else if (IdentStr == "let")
			return Token(start, tok_let, "let");
		else if (IdentStr == "if")
			return Token(start, tok_if, "if");
		else if (IdentStr == "else")
			return Token(start, tok_else, "else");
		else if (IdentStr == "return")
			return Token(start, tok_return, "return");
		else if (IdentStr == "for")
			return Token(start, tok_for, "for");
		else if (IdentStr == "while")
			return Token(start, tok_while, "while");
		else if (IdentStr == "true")
			return Token(start, true);
		else if (IdentStr == "false")
			return Token(start, false);
		return Token(start, tok_ident, IdentStr);
	}
	if (isDigit(lastChar) || lastChar == '.') {
		std::string numStr;
		if (lastChar == '.') lastChar = advance(), numStr += '.';
		if (!isDigit(lastChar)) return Token(start, (TokenType) '.'); //Just a '.' - so return only the '.'
		do {
			numStr += lastChar;
		} while(isDigit(lastChar = advance()));
		if (lastChar == '.') {
			do { //Tack on the '.' and everything after
				numStr += lastChar;
			} while(isDigit(lastChar = advance()));
			return Token(start, strtod(numStr.c_str(), nullptr));
		}
		return Token(start, (int) strtol(numStr.c_str(), nullptr, 10));
	}
	if (isBinopChar(lastChar)) {
		std::string opStr;
		opStr = lastChar;
		while (isBinopChar(lastChar = advance())) {
			opStr += lastChar;
			if (opStr == "//") {
				//We've been duped!
				//We have a comment
				do{lastChar = advance();} while(lastChar != '\n' && lastChar != EOF);
				return getTok();
			} else if (opStr == "/*") {
				//We've been duped!
				//We have a comment
				do {
					do {
						lastChar = advance();
					} while(lastChar != '*' && lastChar != EOF);
					while ((lastChar = advance()) == '*')
					{} //Consume all '*'
					if (lastChar == '/') return lastChar = advance(), getTok();
				} while (lastChar != EOF);
				error("Comment not terminated!");
				return Token(start, tok_eof);
			}
		}
		for (auto& binop: BinopPrecedence) {
			if (binop.str == opStr)
				return Token(start, tok_binop, std::move(binop));
		}
		fatal("Unrecognized operator %s", opStr.c_str());
	}
	char thisChar = lastChar;
	lastChar = advance();
	return Token(start, (TokenType) thisChar);
}
Token getTok() {
	Token t = _getTok();
	//* Debug console info, printing out all tokens TODO add command line switch for debug info like this
	printf("%d:%d: %d ", t.startLoc.line, t.startLoc.col, t.tokenType);
	if (t.tokenType > 0)
		printf("'%c' ", t.tokenType);
	if (t.str)
		printf("\"%s\"", t.str->c_str());
	if (t.binop)
		printf("op: \"%s\"", t.binop->str.c_str());
	if (t.i)
		printf("i: %d", *t.i);
	if (t.d)
		printf("d: %f", *t.d);
	if (t.b)
		printf("b: %s", *t.b ? "true":"false");
	if (t.c)
		printf("c: %c", *t.c);
	printf("\n");
	//*/
	return t;
}
