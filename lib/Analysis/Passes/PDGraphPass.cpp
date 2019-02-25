//
//
//

#include "Pedigree/Config.hpp"

#include "Pedigree/Debug.hpp"

#include "Pedigree/Util.hpp"

#include "Pedigree/Analysis/Passes/PDGraphPass.hpp"

#include "Pedigree/Analysis/Passes/DDGraphPass.hpp"

#include "Pedigree/Analysis/Passes/CDGraphPass.hpp"

#include "Pedigree/Analysis/Passes/MDGraphPass.hpp"

#include "Pedigree/Analysis/Creational/PDGraphBuilder.hpp"

#include "Pedigree/Support/GraphConverter.hpp"

#include "Pedigree/Support/Utils/UnitConverters.hpp"

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

#include <string>
// using std::string

#include <utility>
// using std::move

#define DEBUG_TYPE PEDIGREE_PDG_PASS_NAME

extern llvm::cl::opt<bool> PedigreeGraphConnectRoot;

// plugin registration for opt

char pedigree::PDGraphWrapperPass::ID = 0;
static llvm::RegisterPass<pedigree::PDGraphWrapperPass>
    X(PEDIGREE_PDG_PASS_NAME, PRJ_CMDLINE_DESC("pedigree pdg pass"), false,
      true);

// plugin registration for clang

// the solution was at the bottom of the header file
// 'llvm/Transforms/IPO/PassManagerBuilder.h'
// create a static free-floating callback that uses the legacy pass manager to
// add an instance of this pass and a static instance of the
// RegisterStandardPasses class

static void registerPDGraphWrapperPass(const llvm::PassManagerBuilder &Builder,
                                       llvm::legacy::PassManagerBase &PM) {
  PM.add(new pedigree::PDGraphWrapperPass());

  return;
}

static llvm::RegisterStandardPasses
    RegisterPDGraphWrapperPass(llvm::PassManagerBuilder::EP_EarlyAsPossible,
                               registerPDGraphWrapperPass);

//

static llvm::cl::OptionCategory
    PedigreePDGraphPassCategory("Pedigree PDGraph Pass",
                                "Options for Pedigree PDGraph pass");

static llvm::cl::opt<std::string> MetadataFilterInclusion(
    "pedigree-pdg-metadata-filter-inclusion", llvm::cl::Hidden,
    llvm::cl::desc("mark as filtered nodes with this metadata key"));

static llvm::cl::opt<std::string> MetadataFilterExclusion(
    "pedigree-pdg-metadata-filter-exclusion", llvm::cl::Hidden,
    llvm::cl::desc("mark as filtered nodes without this metadata key"));

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

static void checkAndSetCmdLineOptions() {
  if (!GraphComponentOption.getBits()) {
    GraphComponentOption.addValue(PedigreePDGraphComponent::CDG);
    GraphComponentOption.addValue(PedigreePDGraphComponent::DDG);
    GraphComponentOption.addValue(PedigreePDGraphComponent::MDG);
  }
}

//

llvm::AnalysisKey pedigree::PDGraphAnalysis::Key;

namespace pedigree {

// new passmanager pass

bool PDGraphAnalysis::invalidate(
    llvm::Function &F, const llvm::PreservedAnalyses &PA,
    llvm::FunctionAnalysisManager::Invalidator &Inv) {
  auto PAC = PA.getChecker<PDGraphAnalysis>();

  return !(PAC.preserved() ||
           PAC.preservedSet<llvm::AllAnalysesOn<llvm::Function>>()) ||
         Inv.invalidate<DDGraphAnalysis>(F, PA) ||
         Inv.invalidate<CDGraphAnalysis>(F, PA) ||
         Inv.invalidate<MDGraphAnalysis>(F, PA);
}

PDGraphAnalysis::Result
PDGraphAnalysis::run(llvm::Function &F, llvm::FunctionAnalysisManager &FAM) {
  checkAndSetCmdLineOptions();

  std::unique_ptr<InstCDGraph> instCDG = std::make_unique<InstCDGraph>();
  std::vector<std::unique_ptr<InstructionDependenceGraph>> graphs;

  if (GraphComponentOption.isSet(PedigreePDGraphComponent::CDG)) {
    Convert(*FAM.getResult<CDGraphAnalysis>(F), *instCDG,
            BlockToTerminatorUnitConverter{},
            BlockToInstructionsUnitConverter{});
    graphs.emplace_back(std::move(instCDG));
  }

  if (GraphComponentOption.isSet(PedigreePDGraphComponent::DDG)) {
    graphs.emplace_back(std::move(FAM.getResult<DDGraphAnalysis>(F)));
  }

  if (GraphComponentOption.isSet(PedigreePDGraphComponent::MDG)) {
    graphs.emplace_back(std::move(FAM.getResult<MDGraphAnalysis>(F)));
  }

  PDGraphBuilder builder{};

  // TODO refactor filters to use lists and their absolute position in the
  // command line to chain and apply them
  if (!MetadataFilterExclusion.empty()) {
    MetadataAnnotationReader mdar;

    builder.registerPostInsertionCallback(
        [&mdar](PDGraphBuilder::PostInsertionFuncFirstArgTy i,
                PDGraphBuilder::PostInsertionFuncSecondArgTy u) {
          if (u && mdar.has(u, MetadataFilterExclusion)) {
            i.filtered = true;
          }
        });
  }

  if (!MetadataFilterInclusion.empty()) {
    MetadataAnnotationReader mdar;

    builder.registerPostInsertionCallback(
        [&mdar](PDGraphBuilder::PostInsertionFuncFirstArgTy i,
                PDGraphBuilder::PostInsertionFuncSecondArgTy u) {
          if (u && !mdar.has(u, MetadataFilterInclusion)) {
            i.filtered = true;
          }
        });
  }

  for (const auto &e : graphs) {
    builder.addGraph(*e);
  }

  auto graph = builder.build();

  if (PedigreeGraphConnectRoot) {
    graph->connectRootNode();
  }

  LLVM_DEBUG(llvm::dbgs() << "pdg built with: " << graph->numVertices()
                          << " vertices and " << graph->numOutEdges()
                          << " out edges\n";);

  return std::move(graph);
}

// legacy passmanager pass

PDGraphWrapperPass::PDGraphWrapperPass() : llvm::FunctionPass(ID) {
  checkAndSetCmdLineOptions();
}

void PDGraphWrapperPass::getAnalysisUsage(llvm::AnalysisUsage &AU) const {
  if (GraphComponentOption.isSet(PedigreePDGraphComponent::CDG)) {
    AU.addRequired<CDGraphWrapperPass>();
  }

  if (GraphComponentOption.isSet(PedigreePDGraphComponent::DDG)) {
    AU.addRequired<DDGraphWrapperPass>();
  }

  if (GraphComponentOption.isSet(PedigreePDGraphComponent::MDG)) {
    AU.addRequired<MDGraphWrapperPass>();
  }

  AU.setPreservesAll();
}

bool PDGraphWrapperPass::runOnFunction(llvm::Function &F) {
  InstructionDependenceGraph instCDG;
  std::vector<std::reference_wrapper<const InstructionDependenceGraph>> graphs;

  if (GraphComponentOption.isSet(PedigreePDGraphComponent::CDG)) {
    Convert(getAnalysis<CDGraphWrapperPass>().getGraph(), instCDG,
            BlockToTerminatorUnitConverter{},
            BlockToInstructionsUnitConverter{});
    graphs.emplace_back(instCDG);
  }

  if (GraphComponentOption.isSet(PedigreePDGraphComponent::DDG)) {
    graphs.emplace_back(getAnalysis<DDGraphWrapperPass>().getGraph());
  }

  if (GraphComponentOption.isSet(PedigreePDGraphComponent::MDG)) {
    graphs.emplace_back(getAnalysis<MDGraphWrapperPass>().getGraph());
  }

  PDGraphBuilder builder{};

  // TODO refactor filters to use lists and their absolute position in the
  // command line to chain and apply them
  if (!MetadataFilterExclusion.empty()) {
    MetadataAnnotationReader mdar;

    builder.registerPostInsertionCallback(
        [&mdar](PDGraphBuilder::PostInsertionFuncFirstArgTy i,
                PDGraphBuilder::PostInsertionFuncSecondArgTy u) {
          if (u && mdar.has(u, MetadataFilterExclusion)) {
            i.filtered = true;
          }
        });
  }

  if (!MetadataFilterInclusion.empty()) {
    MetadataAnnotationReader mdar;

    builder.registerPostInsertionCallback(
        [&mdar](PDGraphBuilder::PostInsertionFuncFirstArgTy i,
                PDGraphBuilder::PostInsertionFuncSecondArgTy u) {
          if (u && !mdar.has(u, MetadataFilterInclusion)) {
            i.filtered = true;
          }
        });
  }

  for (const auto &e : graphs) {
    builder.addGraph(e.get());
  }

  Graph = builder.build();

  if (PedigreeGraphConnectRoot) {
    Graph->connectRootNode();
  }

  return false;
}

} // namespace pedigree
