//
//
//

#include "Pedigree/Debug.hpp"

#include "Pedigree/Util.hpp"

#include "Pedigree/Analysis/Passes/DDGraphPass.hpp"

#include "Pedigree/Analysis/Creational/DDGraphBuilder.hpp"

#include "llvm/Config/llvm-config.h"
// version macros

#include "llvm/Pass.h"
// using llvm::RegisterPass

#include "llvm/IR/LegacyPassManager.h"
// using llvm::PassManagerBase

#include "llvm/Transforms/IPO/PassManagerBuilder.h"
// using llvm::PassManagerBuilder
// using llvm::RegisterStandardPasses

#include "llvm/Support/CommandLine.h"
// using llvm::cl::opt
// using llvm::cl::desc
// using llvm::cl::location
// using llvm::cl::cat
// using llvm::cl::init
// using llvm::cl::Hidden
// using llvm::cl::OptionCategory

#include "llvm/Support/raw_ostream.h"
// using llvm::raw_ostream

#include "llvm/Support/Debug.h"
// using DEBUG macro
// using llvm::dbgs

#define DEBUG_TYPE "pedigree-ddg"

extern llvm::cl::opt<bool> PedigreeGraphConnectRoot;

namespace llvm {
class Function;
} // namespace llvm

// plugin registration for opt

char pedigree::DDGraphPass::ID = 0;
static llvm::RegisterPass<pedigree::DDGraphPass>
    X("pedigree-ddg", PRJ_CMDLINE_DESC("pedigree ddg pass"), false, false);

// plugin registration for clang

// the solution was at the bottom of the header file
// 'llvm/Transforms/IPO/PassManagerBuilder.h'
// create a static free-floating callback that uses the legacy pass manager to
// add an instance of this pass and a static instance of the
// RegisterStandardPasses class

static void registerPedigreeDDGraphPass(const llvm::PassManagerBuilder &Builder,
                                        llvm::legacy::PassManagerBase &PM) {
  PM.add(new pedigree::DDGraphPass());

  return;
}

static llvm::RegisterStandardPasses
    RegisterPedigreeDDGraphPass(llvm::PassManagerBuilder::EP_EarlyAsPossible,
                                registerPedigreeDDGraphPass);

//

static llvm::cl::OptionCategory
    PedigreeDDGraphPassCategory("Pedigree DDGraph Pass",
                                "Options for Pedigree DDGraph pass");

static llvm::cl::opt<bool> IgnoreConstantPHINodes(
    "pedigree-ddg-ignore-constant-phi", llvm::cl::init(true), llvm::cl::Hidden,
    llvm::cl::desc("ignore PHI nodes with constant values as data dependences"),
    llvm::cl::cat(PedigreeDDGraphPassCategory));

//

namespace pedigree {

void DDGraphPass::getAnalysisUsage(llvm::AnalysisUsage &AU) const {
  AU.setPreservesAll();
}

bool DDGraphPass::runOnFunction(llvm::Function &CurFunc) {
  DDGraphBuilder builder{};
  Graph = builder.setUnit(CurFunc)
              .ignoreConstantPHINodes(IgnoreConstantPHINodes)
              .build();

  if (PedigreeGraphConnectRoot) {
    Graph->connectRootNode();
  }

  return false;
}

} // namespace pedigree
