//
//
//

#include "Pedigree/Config.hpp"

#include "Pedigree/Debug.hpp"

#include "Pedigree/Util.hpp"

#include "Pedigree/Analysis/AnalysisScope.hpp"

#include "Pedigree/Analysis/Passes/MDGraphPass.hpp"

#include "Pedigree/Analysis/Creational/MDAMDGraphBuilder.hpp"

#include "Pedigree/Analysis/Creational/DAMDGraphBuilder.hpp"

#include "Pedigree/Analysis/Operations/AssignNodeUID.hpp"

#include "Pedigree/Support/Utils/InstIterator.hpp"

#include "llvm/Config/llvm-config.h"
// version macros

#include "llvm/IR/BasicBlock.h"
// using llvm::BasicBlock

#include "llvm/IR/CFG.h"
// using llvm::graph_traits

#include "llvm/ADT/DepthFirstIterator.h"
// using llvm::df_begin
// using llvm::df_end

#include "llvm/Analysis/MemoryDependenceAnalysis.h"
// using llvm::MemoryDependenceAnalysis

#include "llvm/Analysis/DependenceAnalysis.h"
// using llvm::DependenceAnalysis

#if LLVM_VERSION_MAJOR >= 4 && LLVM_VERSION_MINOR >= 0
#include "llvm/Analysis/MemorySSA.h"
#include "llvm/Analysis/MemorySSAUpdater.h"
#endif

#include "llvm/Pass.h"
// using llvm::RegisterPass

#include "llvm/IR/LegacyPassManager.h"
// using llvm::PassManagerBase

#include "llvm/Transforms/IPO/PassManagerBuilder.h"
// using llvm::PassManagerBuilder
// using llvm::RegisterStandardPasses

#include "llvm/Support/CommandLine.h"
// using llvm::cl::opt
// using llvm::cl::list
// using llvm::cl::desc
// using llvm::cl::location
// using llvm::cl::cat
// using llvm::cl::OptionCategory

#include "llvm/Support/raw_ostream.h"
// using llvm::raw_ostream

#include "llvm/Support/Debug.h"
// using LLVM_DEBUG macro
// using llvm::dbgs

#include "boost/range/any_range.hpp"
// using boost::any_range

#include <cassert>
// using assert

#define DEBUG_TYPE "pedigree-mdg"

extern llvm::cl::opt<bool> PedigreeGraphConnectRoot;

namespace llvm {
class Function;
} // namespace llvm

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

enum class AnalysisTraversalType { Naive, DepthFirst };

static llvm::cl::opt<AnalysisTraversalType> AnalysisTraversalOption(
    "pedigree-mdg-traversal", llvm::cl::desc("analysis traversal selection"),
    llvm::cl::values(clEnumValN(AnalysisTraversalType::Naive, "naive",
                                "naive traversal (LLVM API dependent)"),
                     clEnumValN(AnalysisTraversalType::DepthFirst, "df",
                                "depth first traversal")
// clang-format off
#if (LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR < 9)
                                , clEnumValEnd
#endif
                     // clang-format on
                     ),
    llvm::cl::init(AnalysisTraversalType::Naive),
    llvm::cl::cat(PedigreeMDGraphPassCategory));

enum class AnalysisBackendType { MDA, DA, MemorySSA };

static llvm::cl::opt<AnalysisBackendType> AnalysisBackendOption(
    "pedigree-mdg-backend", llvm::cl::desc("analysis backend selection"),
    llvm::cl::values(
        clEnumValN(AnalysisBackendType::MDA, "mda",
                   "Memory Dependence Analysis"),
        clEnumValN(AnalysisBackendType::DA, "da", "Dependence Analysis"),
        clEnumValN(AnalysisBackendType::MemorySSA, "memssa", "MemorySSA")
// clang-format off
#if (LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR < 9)
                                , clEnumValEnd
#endif
        // clang-format on
        ),
    llvm::cl::init(AnalysisBackendType::MDA),
    llvm::cl::cat(PedigreeMDGraphPassCategory));

static llvm::cl::opt<pedigree::AnalysisScope> AnalysisBackendScopeOption(
    "pedigree-mdg-backend-scope", llvm::cl::desc("analysis backend scope"),
    llvm::cl::values(
        clEnumValN(pedigree::AnalysisScope::Block, "block", "basic block"),
        clEnumValN(pedigree::AnalysisScope::Function, "function", "function"),
        clEnumValN(pedigree::AnalysisScope::Interprocedural, "interprocedural",
                   "interprocedural")
// clang-format off
#if (LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR < 9)
                                , clEnumValEnd
#endif
        // clang-format on
        ),
    llvm::cl::cat(PedigreeMDGraphPassCategory));

static llvm::cl::list<pedigree::AnalysisMode> AnalysisBackendModeOption(
    "pedigree-mdg-backend-mode", llvm::cl::desc("analysis backend mode"),
    llvm::cl::values(clEnumValN(pedigree::AnalysisMode::MemDefs, "defs",
                                "mem defs"),
                     clEnumValN(pedigree::AnalysisMode::MemClobbers, "clobbers",
                                "mem clobbers")
// clang-format off
#if (LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR < 9)
                                , clEnumValEnd
#endif
                     // clang-format on
                     ),
    llvm::cl::cat(PedigreeMDGraphPassCategory));

static llvm::cl::opt<bool> EnumerateWithDFS(
    "pedigree-mdg-dfs-enumerate",
    llvm::cl::desc(
        "enumerate graph nodes with DFS numbers from originating IR traversal"),
    llvm::cl::cat(PedigreeMDGraphPassCategory));

static void checkCmdLineOptions() {
  // set default initial value in the absence of an explicitly provided option
  // depending on the backend type
  if (!AnalysisBackendScopeOption.getPosition()) {
    if (AnalysisBackendType::DA == AnalysisBackendOption) {
      AnalysisBackendScopeOption.setValue(pedigree::AnalysisScope::Function);
    } else if (AnalysisBackendType::MDA == AnalysisBackendOption) {
      AnalysisBackendScopeOption.setValue(pedigree::AnalysisScope::Block);
    } else if (AnalysisBackendType::MemorySSA == AnalysisBackendOption) {
      // TODO to be determined
      AnalysisBackendScopeOption.setValue(pedigree::AnalysisScope::Block);
    } else {
      AnalysisBackendScopeOption.setValue(pedigree::AnalysisScope::Block);
    }
  }

  //

  if (AnalysisBackendType::DA == AnalysisBackendOption &&
      AnalysisBackendScopeOption.getPosition()) {
    assert(pedigree::AnalysisScope::Function == AnalysisBackendScopeOption &&
           "Analysis scope is not supported by analysis backend!");
  }

#if LLVM_VERSION_MAJOR >= 4 && LLVM_VERSION_MINOR >= 0
  ;
#else
  assert(AnalysisBackendType::MemorySSA != AnalysisBackendOption &&
         "MemorySSA is not part of this LLVM version!");
#endif

  if (!AnalysisBackendModeOption.size()) {
    AnalysisBackendModeOption.addValue(pedigree::AnalysisMode::MemDefs);
    AnalysisBackendModeOption.addValue(pedigree::AnalysisMode::MemClobbers);
  }
}

//

namespace pedigree {

void MDGraphPass::getAnalysisUsage(llvm::AnalysisUsage &AU) const {
#if (LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR < 9)
  AU.addRequiredTransitive<llvm::MemoryDependenceAnalysis>();
  AU.addRequiredTransitive<llvm::DependenceAnalysis>();
#else
  AU.addRequiredTransitive<llvm::MemoryDependenceWrapperPass>();
  AU.addRequiredTransitive<llvm::DependenceAnalysisWrapperPass>();
#endif
  AU.setPreservesAll();
}

bool MDGraphPass::runOnFunction(llvm::Function &CurFunc) {
  checkCmdLineOptions();

  Graph = std::make_unique<MDGraph>();

  using instruction_range =
      boost::any_range<llvm::Instruction &, boost::forward_traversal_tag>;

  // auto instructions = instruction_range(make_inst_range(CurFunc));
  auto instructions = instruction_range(boost::make_iterator_range(
      make_inst_begin(CurFunc), make_inst_end(CurFunc)));

  if (AnalysisBackendType::DA == AnalysisBackendOption) {
#if (LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR < 9)
    auto &da = getAnalysis<llvm::DependenceAnalysis>();
#else
    auto &da = getAnalysis<llvm::DependenceAnalysisWrapperPass>().getDI();
#endif

    DAMDGraphBuilder builder{};

    Graph = builder.setAnalysis(da).setUnit(CurFunc).build();
  } else {
#if (LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR < 9)
    auto &mda = getAnalysis<llvm::MemoryDependenceAnalysis>();
#else
    auto &mda = getAnalysis<llvm::MemoryDependenceWrapperPass>().getMemDep();
#endif

    MDAMDGraphBuilder builder{};

    for (auto i = 0; i < AnalysisBackendModeOption.size(); ++i) {
      builder.turnOnMode(AnalysisBackendModeOption[i]);
    }

    Graph = builder.setScope(AnalysisBackendScopeOption)
                .setAnalysis(mda)
                .build(instructions.begin(), instructions.end());
  }

  if (PedigreeGraphConnectRoot) {
    Graph->connectRootNode();
  }

  if (EnumerateWithDFS && Graph) {
    auto instructions =
        make_inst_range(llvm::df_begin(&CurFunc), llvm::df_end(&CurFunc));

    AssignNodeUID(*Graph, instructions.begin(), instructions.end());
  }

  return false;
}

} // namespace pedigree
