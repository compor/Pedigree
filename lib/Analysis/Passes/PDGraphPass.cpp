//
//
//

#include "Config.hpp"

#include "Debug.hpp"

#include "Util.hpp"

#include "Analysis/Passes/PDGraphPass.hpp"

#include "Analysis/Passes/DDGraphPass.hpp"

#include "Analysis/Passes/CDGraphPass.hpp"

#include "Analysis/Passes/MDGraphPass.hpp"

#include "Analysis/Creational/PDGraphBuilder.hpp"

#include "Support/GraphConverter.hpp"

#include "Support/Utils/UnitConverters.hpp"

#include "llvm/Config/llvm-config.h"
// version macros

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
// using llvm::cl::bits
// using llvm::cl::desc
// using llvm::cl::location
// using llvm::cl::cat
// using llvm::cl::CommaSeparated
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
    PedigreePDGraphPassCategory("Pedigree PDGraph Pass",
                                "Options for Pedigree PDGraph pass");

enum class PedigreePDGraphComponent { CDG, DDG, MDG };

static llvm::cl::bits<PedigreePDGraphComponent> GraphComponentOption(
    "pedigree-pdg-components", llvm::cl::desc("component graph selection"),
    llvm::cl::values(clEnumValN(PedigreePDGraphComponent::CDG, "CDG",
                                "Control Dependence Graph"),
                     clEnumValN(PedigreePDGraphComponent::DDG, "DDG",
                                "Data Dependence Graph"),
                     clEnumValN(PedigreePDGraphComponent::MDG, "MDG",
                                "Memory Dependence Graph")
// clang-format off
#if (LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR < 9)
                                , clEnumValEnd
#endif
                     // clang-format on
                     ),
    llvm::cl::CommaSeparated, llvm::cl::cat(PedigreePDGraphPassCategory));

#if PEDIGREE_DEBUG
static llvm::cl::opt<bool, true>
    Debug("pedigree-debug", llvm::cl::desc("debug pedigree pdg pass"),
          llvm::cl::location(pedigree::debug::passDebugFlag),
          llvm::cl::cat(PedigreePDGraphPassCategory));

static llvm::cl::opt<LogLevel, true> DebugLevel(
    "pedigree-pdg-debug-level",
    llvm::cl::desc("debug level for pedigree pdg pass"),
    llvm::cl::location(pedigree::debug::passLogLevel),
    llvm::cl::values(
        clEnumValN(LogLevel::Info, "info", "informational messages"),
        clEnumValN(LogLevel::Notice, "notice", "significant conditions"),
        clEnumValN(LogLevel::Warning, "warning", "warning conditions"),
        clEnumValN(LogLevel::Error, "error", "error conditions"),
        clEnumValN(LogLevel::Debug, "debug", "debug messages")
// clang-format off
#if (LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR < 9)
                                , clEnumValEnd
#endif
        // clang-format on
        ),
    llvm::cl::cat(PedigreePDGraphPassCategory));
#endif // PEDIGREE_DEBUG

//

namespace pedigree {

void PDGraphPass::getAnalysisUsage(llvm::AnalysisUsage &AU) const {
  if (GraphComponentOption.isSet(PedigreePDGraphComponent::CDG)) {
    AU.addRequired<CDGraphPass>();
  }

  if (GraphComponentOption.isSet(PedigreePDGraphComponent::DDG)) {
    AU.addRequired<DDGraphPass>();
  }

  if (GraphComponentOption.isSet(PedigreePDGraphComponent::MDG)) {
    AU.addRequired<MDGraphPass>();
  }

  AU.setPreservesAll();
}

bool PDGraphPass::runOnFunction(llvm::Function &CurFunc) {
  InstructionDependenceGraph instCDG;
  std::vector<std::reference_wrapper<const InstructionDependenceGraph>> graphs;

  if (GraphComponentOption.isSet(PedigreePDGraphComponent::CDG)) {
    Convert(getAnalysis<CDGraphPass>().getGraph(), instCDG,
            BlockToInstructionUnitConverter{});
    graphs.emplace_back(instCDG);
  }

  if (GraphComponentOption.isSet(PedigreePDGraphComponent::DDG)) {
    graphs.emplace_back(getAnalysis<DDGraphPass>().getGraph());
  }

  if (GraphComponentOption.isSet(PedigreePDGraphComponent::MDG)) {
    graphs.emplace_back(getAnalysis<MDGraphPass>().getGraph());
  }

  PDGraphBuilder builder{};
  for (const auto &e : graphs) {
    builder.addGraph(e.get());
  }

  Graph = builder.build();

  return false;
}

} // namespace pedigree
