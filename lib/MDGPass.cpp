//
//
//

#include "Utils.hpp"

#include "Pedigree.hpp"

#include "MDGPass.hpp"

#include "MDALocalMDGBuilder.hpp"

#include "DAMDGBuilder.hpp"

#include "llvm/Analysis/MemoryDependenceAnalysis.h"
// using llvm::MemoryDependenceAnalysis

#include "llvm/Analysis/DependenceAnalysis.h"
// using llvm::DependenceAnalysis

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

enum class AnalysisBackendType : uint8_t { MDA, DA };

static llvm::cl::opt<AnalysisBackendType> AnalysisBackendOption(
    "pedigree-mdg-analysis-backend",
    llvm::cl::desc("analysis backend selection"),
    llvm::cl::values(clEnumValN(AnalysisBackendType::MDA, "mda",
                                "Memory Dependency Analysis"),
                     clEnumValN(AnalysisBackendType::DA, "da",
                                "Dependence Analysis"),
                     nullptr),
    llvm::cl::init(AnalysisBackendType::MDA),
    llvm::cl::cat(PedigreeMDGPassCategory));

static llvm::cl::opt<pedigree::AnalysisScope> AnalysisBackendScopeOption(
    "pedigree-mdg-analysis-backend-scope",
    llvm::cl::desc("analysis backend scope"),
    llvm::cl::values(
        clEnumValN(pedigree::AnalysisScope::Block, "block", "basic block"),
        clEnumValN(pedigree::AnalysisScope::Function, "function", "function"),
        clEnumValN(pedigree::AnalysisScope::Interprocedural, "interprocedural",
                   "interprocedural"),
        nullptr),
    llvm::cl::init(pedigree::AnalysisScope::Block),
    llvm::cl::cat(PedigreeMDGPassCategory));

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
  AU.addRequiredTransitive<llvm::DependenceAnalysis>();
  AU.setPreservesAll();
}

bool MDGPass::runOnFunction(llvm::Function &CurFunc) {
  m_Graph = std::make_unique<MDG>();

  if (AnalysisBackendType::DA == AnalysisBackendOption) {
    auto &da = getAnalysis<llvm::DependenceAnalysis>();
    DAMDGBuilder builder{*m_Graph, da};
    builder.build(CurFunc);
  } else {
    auto &mda = getAnalysis<llvm::MemoryDependenceAnalysis>();
    MDALocalMDGBuilder builder{*m_Graph, mda, AnalysisBackendScopeOption};
    builder.build(CurFunc);
  }

  return false;
}

} // namespace pedigree end
