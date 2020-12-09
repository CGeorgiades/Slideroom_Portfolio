/*
 * Lib.cpp
 *
 *  Created on: Nov 20, 2020
 *      Author: chris
 */

#include <llvm/LinkAllPasses.h>
#include "Lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <llvm/Support/raw_os_ostream.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <llvm/Transforms/Scalar.h>
LLVMContext LLContext;
IRBuilder<> LLBuilder(LLContext);
std::unique_ptr<Module> LLModule;
std::unique_ptr<legacy::FunctionPassManager> LLFPM;
/*
DIBuilder *DBuilder;
DebugInfo KSDbgInfo;

DIType* DebugInfo::getDoubleTy() {
	if(DblTy)
		return DblTy;
	return DblTy = DBuilder->createBasicType("double", 64, dwarf::DW_ATE_float);
}
void DebugInfo::emitLocation(const SrcLocation& loc) {
	DIScope* S;
	if (LexicalBlocks.empty())
		S = TheCU;
	else
		S = LexicalBlocks.back();
	LLBuilder.SetCurrentDebugLocation(DebugLoc::get(loc.line, loc.col, S));
}
void DebugInfo::emitLocation(std::nullptr_t) {
	LLBuilder.SetCurrentDebugLocation(DebugLoc());
}
DISubroutineType *CreateDIFunctionType(unsigned argCt, DIFile *Unit) {
  SmallVector<Metadata *, 8> EltTys;
  DIType *DblTy = KSDbgInfo.getDoubleTy();

  // Add the result type.
  EltTys.push_back(DblTy);

  for (unsigned i = 0; i != argCt; i++)
    EltTys.push_back(DblTy);

  return DBuilder->createSubroutineType(DBuilder->getOrCreateTypeArray(EltTys));
}
*/
void initLLModuleAndFPM() {
	static bool hasInit = false;
	if (hasInit) return;
	hasInit = true;
	LLModule = std::make_unique<Module>("Module", LLContext);
	//LLModule->setDataLayout(LLJIT->getTargetMachine().createDataLayout());
	//DBuilder = new DIBuilder(*LLModule);
	//KSDbgInfo.TheCU = DBuilder->createCompileUnit(dwarf::DW_LANG_C, DBuilder->createFile("inp", "."), "Cappa Compiler", 0, "", 0);
	LLFPM = std::make_unique<legacy::FunctionPassManager>(LLModule.get());
//*
	LLFPM->add(createReassociatePass());
	LLFPM->add(createGVNPass());
	LLFPM->add(createCFGSimplificationPass());
	LLFPM->add(createAggressiveDCEPass());
	LLFPM->add(createPromoteMemoryToRegisterPass());
	LLFPM->add(createInstructionCombiningPass());
	//LLFPM->add(createTailCallEliminationPass());//*/
	LLFPM->doInitialization();
}

void exportLLModuleBC(StringRef filename) {
	//DBuilder->finalize();
	std::error_code ec;
	raw_fd_ostream tx(filename, ec);
	LLModule->print(tx, nullptr);
}

int g_argc;
char** g_argv;
