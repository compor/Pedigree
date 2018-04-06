//
//
//

#include "Config.hpp"

#include "Utils.hpp"

#include "Analysis/AnalysisScope.hpp"

#include "Analysis/Passes/MDGraphPass.hpp"

#include "Analysis/Creational/MDALocalMDGraphBuilder.hpp"

#include "Analysis/Creational/DAMDGraphBuilder.hpp"

#include "llvm/Config/llvm-config.h"

#include "llvm/Analysis/MemoryDependenceAnalysis.h"
// using llvm::MemoryDependenceAnalysis

#include "llvm/Analysis/DependenceAnalysis.h"
// using llvm::DependenceAnalysis

#if LLVM_VERSION_MAJOR >= 4 && LLVM_VERSION_MINOR >= 0
#include "llvm/Analysis/MemorySSA.h"
#include "llvm/Analysis/MemorySSAUpdater.h"
#endif

#include "llvm/Analysis/AliasAnalysis.h"
// using llvm::AliasAnalysis

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

#include <cassert>
// using assert

#define DEBUG_TYPE "pedigree-mdg"

namespace llvm {
class Function;
} // namespace llvm end

// plugin registration for opt

char pedigree::MDGraphPass::ID = 0;
static llvm::RegisterPass<pedigree::MDGraphPass>
    X("pedigree-mdg", PRJ_CMDLINE_DESC("pedigree mdg pass"), false, false);

// plugin registration for clang

// the solution was at the bottom of the header file
// 'llvm/Transforms/IPO/PassManagerBuilder.h'
// create a static free-floating callback that uses the legacy pass manager to
// add an instance of this pass and a static instance of the
// RegisterStandardPasses class

static void registerPedigreeMDGraphPass(const llvm::PassManagerBuilder &Builder,
                                        llvm::legacy::PassManagerBase &PM) {
  PM.add(new pedigree::MDGraphPass());

  return;
}

static llvm::RegisterStandardPasses
    RegisterPedigreeMDGraphPass(llvm::PassManagerBuilder::EP_EarlyAsPossible,
                                registerPedigreeMDGraphPass);

//

static llvm::cl::OptionCategory
    PedigreeMDGraphPassCategory("Pedigree MDGraph Pass",
                                "Options for Pedigree MDGraph pass");

enum class AnalysisBackendType : uint8_t { MDA, DA, MemorySSA };

static llvm::cl::opt<AnalysisBackendType> AnalysisBackendOption(
    "pedigree-mdg-backend", llvm::cl::desc("analysis backend selection"),
    llvm::cl::values(clEnumValN(AnalysisBackendType::MDA, "mda",
                                "Memory Dependence Analysis"),
                     clEnumValN(AnalysisBackendType::DA, "da",
                                "Dependence Analysis"),
                     clEnumValN(AnalysisBackendType::MemorySSA, "memssa", ""),
                     nullptr),
    llvm::cl::init(AnalysisBackendType::MDA),
    llvm::cl::cat(PedigreeMDGraphPassCategory));

static llvm::cl::opt<pedigree::AnalysisScope> AnalysisBackendScopeOption(
    "pedigree-mdg-backend-scope", llvm::cl::desc("analysis backend scope"),
    llvm::cl::values(
        clEnumValN(pedigree::AnalysisScope::Block, "block", "basic block"),
        clEnumValN(pedigree::AnalysisScope::Function, "function", "function"),
        clEnumValN(pedigree::AnalysisScope::Interprocedural, "interprocedural",
                   "interprocedural"),
        nullptr),
    llvm::cl::init(pedigree::AnalysisScope::Block),
    llvm::cl::cat(PedigreeMDGraphPassCategory));

#if PEDIGREE_DEBUG
static llvm::cl::opt<bool, true>
    Debug("pedigree-mdg-debug", llvm::cl::desc("debug pedigree mdg pass"),
          llvm::cl::location(pedigree::utility::passDebugFlag),
          llvm::cl::cat(PedigreeMDGraphPassCategory));

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
    llvm::cl::cat(PedigreeMDGraphPassCategory));
#endif // PEDIGREE_DEBUG

static void checkCmdLineOptions() {
  assert(AnalysisBackendType::DA != AnalysisBackendOption &&
         pedigree::AnalysisScope::Function != AnalysisBackendScopeOption &&
         "Analysis scope is not supported by analysis backend!");

#if LLVM_VERSION_MAJOR >= 4 && LLVM_VERSION_MINOR >= 0
  ;
#else
  assert(AnalysisBackendType::MemorySSA != AnalysisBackendOption &&
         "MemorySSA is not part of this LLVM version!");
#endif
}

//

namespace pedigree {

void MDGraphPass::getAnalysisUsage(llvm::AnalysisUsage &AU) const {
  AU.addRequiredTransitive<llvm::AliasAnalysis>();
  AU.addRequiredTransitive<llvm::MemoryDependenceAnalysis>();
  AU.addRequiredTransitive<llvm::DependenceAnalysis>();
  AU.setPreservesAll();
}

bool MDGraphPass::runOnFunction(llvm::Function &CurFunc) {
  checkCmdLineOptions();

  Graph = std::make_unique<MDGraph>();

  if (AnalysisBackendType::DA == AnalysisBackendOption) {
    auto &da = getAnalysis<llvm::DependenceAnalysis>();
    DAMDGraphBuilder builder{};

    Graph = builder.setAnalysis(da).setUnit(CurFunc).build();
  } else {
    auto &mda = getAnalysis<llvm::MemoryDependenceAnalysis>();
    MDALocalMDGraphBuilder builder{};

    Graph = builder.setScope(AnalysisBackendScopeOption)
                .setAnalysis(mda)
                .setUnit(CurFunc)
                .build();
  }

  return false;
}

} // namespace pedigree end
