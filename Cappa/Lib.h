/*
 * Lib.h
 *
 *  Created on: Nov 20, 2020
 *      Author: chris
 */

#ifndef LIB_H_
#define LIB_H_


#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Verifier.h"
#include <llvm/Support/raw_os_ostream.h>
#include <iostream>
#include <stdio.h>
#include <assert.h>
#include "SrcInfo.h"
#include <signal.h>

#include "Wrapper.h"

extern int g_argc;
extern char** g_argv;


static inline bool isAlNum(char ch) {return isalpha(ch) || isdigit(ch) || ch == '_';}

using namespace llvm;
extern LLVMContext LLContext;
extern IRBuilder<> LLBuilder;
extern std::unique_ptr<Module> LLModule;
extern std::unique_ptr<legacy::FunctionPassManager> LLFPM;


/*
extern DIBuilder *DBuilder;
struct DebugInfo {
	DICompileUnit* TheCU = 0;
	DIType *DblTy = 0;
	std::vector<DIScope*> LexicalBlocks;
	DIType* getDoubleTy();
	void emitLocation(const SrcLocation&);
	void emitLocation(std::nullptr_t);
};
extern DebugInfo KSDbgInfo;

DISubroutineType *CreateDIFunctionType(unsigned argCt, DIFile *Unit);
*/

template<typename T = Value>
void LLPrintToStdErr(T* o) {
	raw_os_ostream outp(std::cerr);
	o->print(outp);
	std::cerr << '\n';
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
#pragma GCC diagnostic ignored "-Wgcc-compat"

#define fatal(...) _fatal(__FILE__, __LINE__, __PRETTY_FUNCTION__,  __VA_ARGS__)
#define error(...) _error(__FILE__, __LINE__, __PRETTY_FUNCTION__,  __VA_ARGS__)

template<typename...Args>
void _fatal(const char* file, unsigned int line,
		const char* function, StringRef msg, Args...args) __attribute__((noreturn)) {
	extern const char* __progname;
	fprintf(stderr, "%s: %s:%u: %s: \n\tFatal Error: ", __progname, file, line, function);
	fprintf(stderr, msg.begin(), args...);
	fprintf(stderr, "\n");
	raise(SIGILL);
	exit(-1);
}
template<typename...Args>
void _error(const char* file, unsigned int line,
		const char* function, StringRef msg, Args...args){
	extern const char* __progname;
	fprintf(stderr, "%s: %s:%u: %s: \n\tError: ", __progname, file, line, function);
	fprintf(stderr, msg.begin(), args...);
	fprintf(stderr, "\n");
}
#pragma GCC diagnostic pop

char getChar();
void initLLModuleAndFPM();
void exportLLModuleBC(StringRef filename);

#endif /* LIB_H_ */
