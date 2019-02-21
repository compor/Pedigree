//
//
//

#include "Pedigree/Config.hpp"

#include "Pedigree/Debug.hpp"

#include "Pedigree/Util.hpp"

#include "Pedigree/Analysis/Passes/CDGraphPass.hpp"

#include "Pedigree/Analysis/Creational/CDGraphBuilder.hpp"

#include "Pedigree/Support/GraphConverter.hpp"

#include "Pedigree/Support/Utils/UnitConverters.hpp"

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
// using llvm::cl::OptionCategory

#include "llvm/Support/raw_ostream.h"
// using llvm::raw_ostream

#include "llvm/Support/Debug.h"
// using LLVM_DEBUG macro
// using llvm::dbgs

#define DEBUG_TYPE "pedigree-cdg"

extern llvm::cl::opt<bool> PedigreeGraphConnectRoot;

namespace llvm {
class Function;
} // namespace llvm

// plugin registration for opt

char pedigree::CDGraphWrapperPass::ID = 0;
static llvm::RegisterPass<pedigree::CDGraphWrapperPass>
    X("pedigree-cdg", PRJ_CMDLINE_DESC("pedigree cdg pass"), false, true);

// plugin registration for clang

// the solution was at the bottom of the header file
// 'llvm/Transforms/IPO/PassManagerBuilder.h'
// create a static free-floating callback that uses the legacy pass manager to
// add an instance of this pass and a static instance of the
// RegisterStandardPasses class

static void
registerPedigreeCDGraphWrapperPass(const llvm::PassManagerBuilder &Builder,
                                   llvm::legacy::PassManagerBase &PM) {
  PM.add(new pedigree::CDGraphWrapperPass());

  return;
}

static llvm::RegisterStandardPasses RegisterPedigreeCDGraphWrapperPass(
    llvm::PassManagerBuilder::EP_EarlyAsPossible,
    registerPedigreeCDGraphWrapperPass);

//

static llvm::cl::OptionCategory
    PedigreeCDGraphPassCategory("Pedigree CDGraph Pass",
                                "Options for Pedigree CDGraph pass");

static llvm::cl::opt<bool> PedigreeCDGraphConvertToInstruction(
    "pedigree-cdg-convert-instruction",
    llvm::cl::desc("adapt cdg to block terminator instructions"),
    llvm::cl::init(false), llvm::cl::cat(PedigreeCDGraphPassCategory));

//

namespace pedigree {

void CDGraphWrapperPass::getAnalysisUsage(llvm::AnalysisUsage &AU) const {
  AU.setPreservesAll();
}

bool CDGraphWrapperPass::runOnFunction(llvm::Function &CurFunc) {
  CDGraphBuilder builder{};
  Graph = builder.setUnit(CurFunc).build();

  if (PedigreeGraphConnectRoot) {
    Graph->connectRootNode();
  }

  if (PedigreeCDGraphConvertToInstruction) {
    InstGraph = std::make_unique<InstCDGraph>();
    Convert(*Graph, *InstGraph, BlockToTerminatorUnitConverter{},
            BlockToInstructionsUnitConverter{});
  }

  return false;
}

} // namespace pedigree
