//
//
//

#include "Config.hpp"

#include "Utils.hpp"

#include "Analysis/Passes/CDGraphPass.hpp"

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
    CDGraphDOTEdgeAttributes("pedigree-cdg-dot-edge-attrs", llvm::cl::Hidden,
                         llvm::cl::desc("CDGraph DOT edge attributes"));

static llvm::cl::opt<bool>
    CDGraphDOTSimple("pedigree-cdg-dot-simple", llvm::cl::Hidden,
                 llvm::cl::desc("generate simple CDGraph DOT graph"));

static llvm::cl::list<std::string> CDGraphDOTFunctionWhitelist(
    "pedigree-cdg-dot-func-wl", llvm::cl::Hidden,
    llvm::cl::desc("generate CDGraph DOT graph only for these functions"));

namespace llvm {

template <>
struct DOTGraphTraits<pedigree::CDGraph *>
    : public pedigree::LLVMDOTDependenceGraphTraitsBase<pedigree::CDGraph *> {
  using Base = pedigree::LLVMDOTDependenceGraphTraitsBase<pedigree::CDGraph *>;

  DOTGraphTraits(bool isSimple = false) : Base(isSimple) {}

  static std::string getGraphName(const GraphType *) { return "CDGraph"; }

  std::string getNodeLabel(const NodeType *Node, const GraphType *Graph) {
    return isSimple() || CDGraphDOTSimple ? getSimpleNodeLabel(Node, Graph)
                                      : getCompleteNodeLabel(Node, Graph);
  }

  static std::string getEdgeAttributes(const NodeType *Node,
                                       typename GT::ChildIteratorType EI,
                                       const GraphType *Graph) {
    return CDGraphDOTEdgeAttributes.empty()
               ? Base::getEdgeAttributes(Node, EI, Graph)
               : CDGraphDOTEdgeAttributes.getValue();
  }
};

struct AnalysisDependenceGraphPassTraits {
  static pedigree::CDGraph *getGraph(pedigree::CDGraphPass *P) {
    return &P->getGraph();
  }
};

} // namespace llvm end

namespace pedigree {

struct CDGraphPrinterPass
    : public llvm::DOTGraphTraitsPrinter<
          CDGraphPass, false, CDGraph *, llvm::AnalysisDependenceGraphPassTraits> {
  using Base =
      llvm::DOTGraphTraitsPrinter<CDGraphPass, false, CDGraph *,
                                  llvm::AnalysisDependenceGraphPassTraits>;
  static char ID;

  CDGraphPrinterPass() : Base("cdg", ID) {}

  bool runOnFunction(llvm::Function &CurFunction) override {
    auto found = std::find(std::begin(CDGraphDOTFunctionWhitelist),
                           std::end(CDGraphDOTFunctionWhitelist),
                           CurFunction.getName().str());

    if (CDGraphDOTFunctionWhitelist.empty() ||
        std::end(CDGraphDOTFunctionWhitelist) != found)
      return Base::runOnFunction(CurFunction);
    else
      return false;
  }
};

} // namespace pedigree end

char pedigree::CDGraphPrinterPass::ID = 0;
static llvm::RegisterPass<pedigree::CDGraphPrinterPass>
    X("pedigree-cdg-dot", PRJ_CMDLINE_DESC("pedigree cdg DOT pass"), false,
      false);

// plugin registration for clang

// the solution was at the bottom of the header file
// 'llvm/Transforms/IPO/PassManagerBuilder.h'
// create a static free-floating callback that uses the legacy pass manager to
// add an instance of this pass and a static instance of the
// RegisterStandardPasses class

static void
registerPedigreeCDGraphPrinterPass(const llvm::PassManagerBuilder &Builder,
                               llvm::legacy::PassManagerBase &PM) {
  PM.add(new pedigree::CDGraphPrinterPass());

  return;
}

static llvm::RegisterStandardPasses
    RegisterPedigreeCDGraphPrinterPass(llvm::PassManagerBuilder::EP_EarlyAsPossible,
                                   registerPedigreeCDGraphPrinterPass);
