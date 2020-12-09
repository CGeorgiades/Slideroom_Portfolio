/*
 * Kaleid.cpp
 *
 *  Created on: Nov 20, 2020
 *      Author: chris
 */

#include <Builtins.h>
#include "Parse/Lexer.h"
#include "Parse/Parser.h"
#include "Lib.h"
#include <stdio.h>
#include <Visitors/CodegenVisitor.h>
#include <Visitors/TypeDefVisitor.h>
#include <Visitors/SigGenVisitor.h>
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "include/KaleidoscopeJIT.h"
#include <iostream>
FILE* inp;
char getChar() {
	return fgetc(inp);
}
void test() {
	printf("Hello!!");
}
int main(int argc, char* argv[]) {
	g_argc = argc;
	g_argv = argv;
	if (argc < 2) {
		fatal("Must provide input file!\n");
	} else {
		inp = fopen(argv[1], "r");
		if (inp == nullptr) {
			fatal("Unable to open input file");
		}
		/*InitializeNativeTarget();
		InitializeNativeTargetAsmParser();
		InitializeNativeTargetAsmPrinter();
		LLJIT = std::make_unique<KaleidoscopeJIT>();*/
		initLLModuleAndFPM();
		initBuiltins();
		auto pgm = parsePgm();
		TypeDefVisitor tdv;
		tdv.visit(*pgm);
		SigGenVisitor sgv;
		sgv.visit(*pgm);
		CodegenVisitor cgv;
		cgv.visit(*pgm);
		exportLLModuleBC("outp.ll_");
	}
	return 0;
}

