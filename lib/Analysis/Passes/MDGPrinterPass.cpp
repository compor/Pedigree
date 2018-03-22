//
//
//

#include "Config.hpp"

#include "Utils.hpp"

#include "Analysis/Passes/MDGPass.hpp"

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
    MDGDOTEdgeAttributes("pedigree-mdg-dot-edge-attrs", llvm::cl::Hidden,
                         llvm::cl::desc("MDG DOT edge attributes"));

static llvm::cl::opt<bool>
    MDGDOTSimple("pedigree-mdg-dot-simple", llvm::cl::Hidden,
                 llvm::cl::desc("generate simple MDG DOT graph"));

static llvm::cl::list<std::string> MDGDOTFunctionWhitelist(
    "pedigree-mdg-dot-func-wl", llvm::cl::Hidden,
    llvm::cl::desc("generate MDG DOT graph only for these functions"));

namespace llvm {

template <>
struct DOTGraphTraits<pedigree::MDG *>
    : public pedigree::LLVMDOTDependenceGraphTraitsBase<pedigree::MDG *> {
  using Base = pedigree::LLVMDOTDependenceGraphTraitsBase<pedigree::MDG *>;

  DOTGraphTraits(bool isSimple = false) : Base(isSimple) {}

  static std::string getGraphName(const GraphType *) { return "MDG"; }

  std::string getNodeLabel(const NodeType *Node, const GraphType *Graph) {
    return isSimple() || MDGDOTSimple ? getSimpleNodeLabel(Node, Graph)
                                      : getCompleteNodeLabel(Node, Graph);
  }

  static std::string getEdgeAttributes(const NodeType *Node,
                                       typename GT::ChildIteratorType EI,
                                       const GraphType *Graph) {
    return MDGDOTEdgeAttributes.empty()
               ? Base::getEdgeAttributes(Node, EI, Graph)
               : MDGDOTEdgeAttributes.getValue();
  }
};

struct AnalysisDependenceGraphPassTraits {
  static pedigree::MDG *getGraph(pedigree::MDGPass *P) {
    return &P->getGraph();
  }
};

} // namespace llvm end

namespace pedigree {

struct MDGPrinterPass
    : public llvm::DOTGraphTraitsPrinter<
          MDGPass, false, MDG *, llvm::AnalysisDependenceGraphPassTraits> {
  using Base =
      llvm::DOTGraphTraitsPrinter<MDGPass, false, MDG *,
                                  llvm::AnalysisDependenceGraphPassTraits>;
  static char ID;

  MDGPrinterPass() : Base("mdg", ID) {}

  bool runOnFunction(llvm::Function &CurFunction) override {
    auto found = std::find(std::begin(MDGDOTFunctionWhitelist),
                           std::end(MDGDOTFunctionWhitelist),
                           CurFunction.getName().str());

    if (MDGDOTFunctionWhitelist.empty() ||
        std::end(MDGDOTFunctionWhitelist) != found)
      return Base::runOnFunction(CurFunction);
    else
      return false;
  }
};

} // namespace pedigree end

char pedigree::MDGPrinterPass::ID = 0;
static llvm::RegisterPass<pedigree::MDGPrinterPass>
    X("pedigree-mdg-dot", PRJ_CMDLINE_DESC("pedigree mdg DOT pass"), false,
      false);

// plugin registration for clang

// the solution was at the bottom of the header file
// 'llvm/Transforms/IPO/PassManagerBuilder.h'
// create a static free-floating callback that uses the legacy pass manager to
// add an instance of this pass and a static instance of the
// RegisterStandardPasses class

static void
registerPedigreeMDGPrinterPass(const llvm::PassManagerBuilder &Builder,
                               llvm::legacy::PassManagerBase &PM) {
  PM.add(new pedigree::MDGPrinterPass());

  return;
}

static llvm::RegisterStandardPasses
    RegisterPedigreeMDGPrinterPass(llvm::PassManagerBuilder::EP_EarlyAsPossible,
                                   registerPedigreeMDGPrinterPass);
