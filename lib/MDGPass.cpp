//
//
//

#include "Utils.hpp"

#include "MDGPass.hpp"

#include "MDASimpleMDGBuilder.hpp"

#include "llvm/Analysis/MemoryDependenceAnalysis.h"
// using llvm::MemoryDependenceAnalysis

#include "llvm/Analysis/AliasAnalysis.h"
// using llvm::AliasAnalysis

#include "llvm/IR/Function.h"
// using llvm::Function

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
// using DEBUG macro
// using llvm::dbgs

#define DEBUG_TYPE "pedigree-mdg"

// plugin registration for opt

char pedigree::MDGPass::ID = 0;
static llvm::RegisterPass<pedigree::MDGPass>
    X("pedigree-mdg", PRJ_CMDLINE_DESC("pedigree mdg pass"), false, false);

// plugin registration for clang

// the solution was at the bottom of the header file
// 'llvm/Transforms/IPO/PassManagerBuilder.h'
// create a static free-floating callback that uses the legacy pass manager to
// add an instance of this pass and a static instance of the
// RegisterStandardPasses class

static void registerPedigreeMDGPass(const llvm::PassManagerBuilder &Builder,
                                    llvm::legacy::PassManagerBase &PM) {
  PM.add(new pedigree::MDGPass());

  return;
}

static llvm::RegisterStandardPasses
    RegisterPedigreeMDGPass(llvm::PassManagerBuilder::EP_EarlyAsPossible,
                            registerPedigreeMDGPass);

//

static llvm::cl::OptionCategory
    PedigreeMDGPassCategory("Pedigree MDG Pass",
                            "Options for Pedigree MDG pass");

#if PEDIGREE_DEBUG
static llvm::cl::opt<bool, true>
    Debug("pedigree-mdg-debug", llvm::cl::desc("debug pedigree mdg pass"),
          llvm::cl::location(pedigree::utility::passDebugFlag),
          llvm::cl::cat(PedigreeMDGPassCategory));

static llvm::cl::opt<LogLevel, true> DebugLevel(
    "pedigree-mdg-debug-level",
    llvm::cl::desc("debug level for pedigree mdg pass"),
    llvm::cl::location(pedigree::utility::passLogLevel),
    llvm::cl::values(
        clEnumValN(LogLevel::info, "info", "informational messages"),
        clEnumValN(LogLevel::notice, "notice", "significant conditions"),
        clEnumValN(LogLevel::warning, "warning", "warning conditions"),
        clEnumValN(LogLevel::error, "error", "error conditions"),
        clEnumValN(LogLevel::debug, "debug", "debug messages"), nullptr),
    llvm::cl::cat(PedigreeMDGPassCategory));
#endif // PEDIGREE_DEBUG

//

namespace pedigree {

void MDGPass::getAnalysisUsage(llvm::AnalysisUsage &AU) const {
  AU.addRequiredTransitive<llvm::AliasAnalysis>();
  AU.addRequiredTransitive<llvm::MemoryDependenceAnalysis>();
  AU.setPreservesAll();
}

bool MDGPass::runOnFunction(llvm::Function &CurFunc) {
  m_Graph = std::make_unique<MDG>();
  auto &mda = getAnalysis<llvm::MemoryDependenceAnalysis>();
  MDASimpleMDGBuilder builder{*m_Graph, mda};
  builder.build(CurFunc);

  return false;
}

} // namespace pedigree end
