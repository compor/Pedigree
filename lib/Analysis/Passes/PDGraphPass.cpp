//
//
//

#include "Config.hpp"

#include "Utils.hpp"

#include "Analysis/Passes/PDGraphPass.hpp"

#include "Analysis/Passes/DDGraphPass.hpp"

#include "Analysis/Passes/CDGraphPass.hpp"

#include "Analysis/Passes/MDGraphPass.hpp"

#include "Analysis/PDGraphBuilder.hpp"

#include "llvm/IR/Type.h"
// using llvm::Type

#include "llvm/IR/Instruction.h"
// using llvm::Instruction

#include "llvm/IR/Function.h"
// using llvm::Function

#include "llvm/Support/Casting.h"
// using llvm::dyn_cast

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

#define DEBUG_TYPE "pedigree-pdg"

// plugin registration for opt

char pedigree::PDGraphPass::ID = 0;
static llvm::RegisterPass<pedigree::PDGraphPass>
    X("pedigree-pdg", PRJ_CMDLINE_DESC("pedigree pdg pass"), false, false);

// plugin registration for clang

// the solution was at the bottom of the header file
// 'llvm/Transforms/IPO/PassManagerBuilder.h'
// create a static free-floating callback that uses the legacy pass manager to
// add an instance of this pass and a static instance of the
// RegisterStandardPasses class

static void registerPDGraphPass(const llvm::PassManagerBuilder &Builder,
                            llvm::legacy::PassManagerBase &PM) {
  PM.add(new pedigree::PDGraphPass());

  return;
}

static llvm::RegisterStandardPasses
    RegisterPDGraphPass(llvm::PassManagerBuilder::EP_EarlyAsPossible,
                    registerPDGraphPass);

//

static llvm::cl::OptionCategory
    PDGraphPassCategory("Pedigree PDGraph Pass", "Options for Pedigree PDGraph pass");

#if PEDIGREE_DEBUG
static llvm::cl::opt<bool, true>
    Debug("pedigree-debug", llvm::cl::desc("debug pedigree pdg pass"),
          llvm::cl::location(pedigree::utility::passDebugFlag),
          llvm::cl::cat(PDGraphPassCategory));

static llvm::cl::opt<LogLevel, true> DebugLevel(
    "pedigree-pdg-debug-level",
    llvm::cl::desc("debug level for pedigree pdg pass"),
    llvm::cl::location(pedigree::utility::passLogLevel),
    llvm::cl::values(
        clEnumValN(LogLevel::info, "info", "informational messages"),
        clEnumValN(LogLevel::notice, "notice", "significant conditions"),
        clEnumValN(LogLevel::warning, "warning", "warning conditions"),
        clEnumValN(LogLevel::error, "error", "error conditions"),
        clEnumValN(LogLevel::debug, "debug", "debug messages"), nullptr),
    llvm::cl::cat(PDGraphPassCategory));
#endif // PEDIGREE_DEBUG

//

namespace pedigree {

void PDGraphPass::getAnalysisUsage(llvm::AnalysisUsage &AU) const {
  AU.addRequired<DDGraphPass>();
  AU.addRequired<CDGraphPass>();
  AU.addRequired<MDGraphPass>();
  AU.setPreservesAll();
}

bool PDGraphPass::runOnFunction(llvm::Function &CurFunc) {
  auto &ddg = getAnalysis<DDGraphPass>().getGraph();
  auto &cdg = getAnalysis<CDGraphPass>().getGraph();
  auto &mdg = getAnalysis<MDGraphPass>().getGraph();

  auto pdg = std::make_unique<PDGraph>();

  return false;
}

} // namespace pedigree end
