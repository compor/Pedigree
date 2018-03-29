//
//
//

#include "Utils.hpp"

#include "Analysis/Passes/CDGraphPass.hpp"

#include "Analysis/CDGraphBuilder.hpp"

#include "Support/GraphConverter.hpp"

#include "Support/Utils/UnitConverters.hpp"

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

char pedigree::CDGraphPass::ID = 0;
static llvm::RegisterPass<pedigree::CDGraphPass>
    X("pedigree-cdg", PRJ_CMDLINE_DESC("pedigree cdg pass"), false, false);

// plugin registration for clang

// the solution was at the bottom of the header file
// 'llvm/Transforms/IPO/PassManagerBuilder.h'
// create a static free-floating callback that uses the legacy pass manager to
// add an instance of this pass and a static instance of the
// RegisterStandardPasses class

static void registerPedigreeCDGraphPass(const llvm::PassManagerBuilder &Builder,
                                        llvm::legacy::PassManagerBase &PM) {
  PM.add(new pedigree::CDGraphPass());

  return;
}

static llvm::RegisterStandardPasses
    RegisterPedigreeCDGraphPass(llvm::PassManagerBuilder::EP_EarlyAsPossible,
                                registerPedigreeCDGraphPass);

//

static llvm::cl::OptionCategory
    PedigreeCDGraphPassCategory("Pedigree CDGraph Pass",
                                "Options for Pedigree CDGraph pass");

static llvm::cl::opt<bool> PedigreeCDGraphConvertToInstruction(
    "pedigree-cdg-instruction",
    llvm::cl::desc("adapt cdg to block terminator instructions"),
    llvm::cl::init(false), llvm::cl::cat(PedigreeCDGraphPassCategory));

#if PEDIGREE_DEBUG
static llvm::cl::opt<bool, true>
    Debug("pedigree-cdg-debug", llvm::cl::desc("debug pedigree cdg pass"),
          llvm::cl::location(pedigree::utility::passDebugFlag),
          llvm::cl::cat(PedigreeCDGraphPassCategory));

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
    llvm::cl::cat(PedigreeCDGraphPassCategory));
#endif // PEDIGREE_DEBUG

//

namespace pedigree {

void CDGraphPass::getAnalysisUsage(llvm::AnalysisUsage &AU) const {
  AU.setPreservesAll();
}

bool CDGraphPass::runOnFunction(llvm::Function &CurFunc) {
  Graph = std::make_unique<CDGraph>();
  CDGraphBuilder builder{*Graph};
  builder.build(CurFunc);

  if (PedigreeCDGraphConvertToInstruction) {
    m_InstGraph = std::make_unique<InstCDGraph>();
    Convert(*Graph, *m_InstGraph, BlockToInstructionUnitConverter{});
  }

  return false;
}

} // namespace pedigree end
