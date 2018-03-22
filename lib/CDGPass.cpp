//
//
//

#include "Utils.hpp"

#include "CDGPass.hpp"

#include "CDGBuilder.hpp"

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

#define DEBUG_TYPE "pedigree-cdg"

namespace llvm {
class Function;
} // namespace llvm end

// plugin registration for opt

char pedigree::CDGPass::ID = 0;
static llvm::RegisterPass<pedigree::CDGPass>
    X("pedigree-cdg", PRJ_CMDLINE_DESC("pedigree cdg pass"), false, false);

// plugin registration for clang

// the solution was at the bottom of the header file
// 'llvm/Transforms/IPO/PassManagerBuilder.h'
// create a static free-floating callback that uses the legacy pass manager to
// add an instance of this pass and a static instance of the
// RegisterStandardPasses class

static void registerPedigreeCDGPass(const llvm::PassManagerBuilder &Builder,
                                    llvm::legacy::PassManagerBase &PM) {
  PM.add(new pedigree::CDGPass());

  return;
}

static llvm::RegisterStandardPasses
    RegisterPedigreeCDGPass(llvm::PassManagerBuilder::EP_EarlyAsPossible,
                            registerPedigreeCDGPass);

//

static llvm::cl::OptionCategory
    PedigreeCDGPassCategory("Pedigree CDG Pass",
                            "Options for Pedigree CDG pass");

#if PEDIGREE_DEBUG
static llvm::cl::opt<bool, true>
    Debug("pedigree-cdg-debug", llvm::cl::desc("debug pedigree cdg pass"),
          llvm::cl::location(pedigree::utility::passDebugFlag),
          llvm::cl::cat(PedigreeCDGPassCategory));

static llvm::cl::opt<LogLevel, true> DebugLevel(
    "pedigree-cdg-debug-level",
    llvm::cl::desc("debug level for pedigree cdg pass"),
    llvm::cl::location(pedigree::utility::passLogLevel),
    llvm::cl::values(
        clEnumValN(LogLevel::info, "info", "informational messages"),
        clEnumValN(LogLevel::notice, "notice", "significant conditions"),
        clEnumValN(LogLevel::warning, "warning", "warning conditions"),
        clEnumValN(LogLevel::error, "error", "error conditions"),
        clEnumValN(LogLevel::debug, "debug", "debug messages"), nullptr),
    llvm::cl::cat(PedigreeCDGPassCategory));
#endif // PEDIGREE_DEBUG

//

namespace pedigree {

void CDGPass::getAnalysisUsage(llvm::AnalysisUsage &AU) const {
  AU.setPreservesAll();
}

bool CDGPass::runOnFunction(llvm::Function &CurFunc) {
  m_Graph = std::make_unique<CDG>();
  CDGBuilder builder{*m_Graph};
  builder.build(CurFunc);

  return false;
}

} // namespace pedigree end
