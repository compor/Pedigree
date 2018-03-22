//
//
//

#include "Config.hpp"

#include "Utils.hpp"

#include "Analysis/Passes/CDGPass.hpp"

#include "Support/Traits/LLVMDOTGraphTraitsHelper.hpp"

#include "llvm/Pass.h"
// using llvm::RegisterPass

#include "llvm/IR/LegacyPassManager.h"
// using llvm::PassManagerBase

#include "llvm/Transforms/IPO/PassManagerBuilder.h"
// using llvm::PassManagerBuilder
// using llvm::RegisterStandardPasses

#include "llvm/Analysis/DOTGraphTraitsPass.h"
// using llvm::DOTGraphTraitsPrinter

#include "llvm/Support/CommandLine.h"
// using llvm::cl::opt
// using llvm::cl::list
// using llvm::cl::desc
// using llvm::cl::Hidden

#include "llvm/Support/raw_ostream.h"
// using llvm::raw_string_ostream

#include <algorithm>
// std::find

#include <string>
// using std::string

static llvm::cl::opt<std::string>
    CDGDOTEdgeAttributes("pedigree-cdg-dot-edge-attrs", llvm::cl::Hidden,
                         llvm::cl::desc("CDG DOT edge attributes"));

static llvm::cl::opt<bool>
    CDGDOTSimple("pedigree-cdg-dot-simple", llvm::cl::Hidden,
                 llvm::cl::desc("generate simple CDG DOT graph"));

static llvm::cl::list<std::string> CDGDOTFunctionWhitelist(
    "pedigree-cdg-dot-func-wl", llvm::cl::Hidden,
    llvm::cl::desc("generate CDG DOT graph only for these functions"));

namespace llvm {

template <>
struct DOTGraphTraits<pedigree::CDG *>
    : public pedigree::LLVMDOTDependenceGraphTraitsBase<pedigree::CDG *> {
  using Base = pedigree::LLVMDOTDependenceGraphTraitsBase<pedigree::CDG *>;

  DOTGraphTraits(bool isSimple = false) : Base(isSimple) {}

  static std::string getGraphName(const GraphType *) { return "CDG"; }

  std::string getNodeLabel(const NodeType *Node, const GraphType *Graph) {
    return isSimple() || CDGDOTSimple ? getSimpleNodeLabel(Node, Graph)
                                      : getCompleteNodeLabel(Node, Graph);
  }

  static std::string getEdgeAttributes(const NodeType *Node,
                                       typename GT::ChildIteratorType EI,
                                       const GraphType *Graph) {
    return CDGDOTEdgeAttributes.empty()
               ? Base::getEdgeAttributes(Node, EI, Graph)
               : CDGDOTEdgeAttributes.getValue();
  }
};

struct AnalysisDependenceGraphPassTraits {
  static pedigree::CDG *getGraph(pedigree::CDGPass *P) {
    return &P->getGraph();
  }
};

} // namespace llvm end

namespace pedigree {

struct CDGPrinterPass
    : public llvm::DOTGraphTraitsPrinter<
          CDGPass, false, CDG *, llvm::AnalysisDependenceGraphPassTraits> {
  using Base =
      llvm::DOTGraphTraitsPrinter<CDGPass, false, CDG *,
                                  llvm::AnalysisDependenceGraphPassTraits>;
  static char ID;

  CDGPrinterPass() : Base("cdg", ID) {}

  bool runOnFunction(llvm::Function &CurFunction) override {
    auto found = std::find(std::begin(CDGDOTFunctionWhitelist),
                           std::end(CDGDOTFunctionWhitelist),
                           CurFunction.getName().str());

    if (CDGDOTFunctionWhitelist.empty() ||
        std::end(CDGDOTFunctionWhitelist) != found)
      return Base::runOnFunction(CurFunction);
    else
      return false;
  }
};

} // namespace pedigree end

char pedigree::CDGPrinterPass::ID = 0;
static llvm::RegisterPass<pedigree::CDGPrinterPass>
    X("pedigree-cdg-dot", PRJ_CMDLINE_DESC("pedigree cdg DOT pass"), false,
      false);

// plugin registration for clang

// the solution was at the bottom of the header file
// 'llvm/Transforms/IPO/PassManagerBuilder.h'
// create a static free-floating callback that uses the legacy pass manager to
// add an instance of this pass and a static instance of the
// RegisterStandardPasses class

static void
registerPedigreeCDGPrinterPass(const llvm::PassManagerBuilder &Builder,
                               llvm::legacy::PassManagerBase &PM) {
  PM.add(new pedigree::CDGPrinterPass());

  return;
}

static llvm::RegisterStandardPasses
    RegisterPedigreeCDGPrinterPass(llvm::PassManagerBuilder::EP_EarlyAsPossible,
                                   registerPedigreeCDGPrinterPass);
