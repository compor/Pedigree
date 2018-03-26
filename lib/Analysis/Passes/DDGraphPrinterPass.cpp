//
//
//

#include "Config.hpp"

#include "Utils.hpp"

#include "Analysis/Passes/DDGraphPass.hpp"

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
    DDGraphDOTEdgeAttributes("pedigree-ddg-dot-edge-attrs", llvm::cl::Hidden,
                         llvm::cl::desc("DDGraph DOT edge attributes"));

static llvm::cl::opt<bool>
    DDGraphDOTSimple("pedigree-ddg-dot-simple", llvm::cl::Hidden,
                 llvm::cl::desc("generate simple DDGraph DOT graph"));

static llvm::cl::list<std::string> DDGraphDOTFunctionWhitelist(
    "pedigree-ddg-dot-func-wl", llvm::cl::Hidden,
    llvm::cl::desc("generate DDGraph DOT graph only for these functions"));

namespace llvm {

template <>
struct DOTGraphTraits<pedigree::DDGraph *>
    : public pedigree::LLVMDOTDependenceGraphTraitsBase<pedigree::DDGraph *> {
  using Base = pedigree::LLVMDOTDependenceGraphTraitsBase<pedigree::DDGraph *>;

  DOTGraphTraits(bool isSimple = false) : Base(isSimple) {}

  static std::string getGraphName(const GraphType *) { return "LOL DDGraph"; }

  std::string getNodeLabel(const NodeType *Node, const GraphType *Graph) {
    return isSimple() || DDGraphDOTSimple ? getSimpleNodeLabel(Node, Graph)
                                      : getCompleteNodeLabel(Node, Graph);
  }

  static std::string getEdgeAttributes(const NodeType *Node,
                                       typename GT::ChildIteratorType EI,
                                       const GraphType *Graph) {
    return DDGraphDOTEdgeAttributes.empty()
               ? Base::getEdgeAttributes(Node, EI, Graph)
               : DDGraphDOTEdgeAttributes.getValue();
  }
};

struct AnalysisDependenceGraphPassTraits {
  static pedigree::DDGraph *getGraph(pedigree::DDGraphPass *P) {
    return &P->getGraph();
  }
};

} // namespace llvm end

namespace pedigree {

struct DDGraphPrinterPass : public llvm::DOTGraphTraitsPrinter<
                            DDGraphPass, false, pedigree::DDGraph *,
                            llvm::AnalysisDependenceGraphPassTraits> {
  using Base =
      llvm::DOTGraphTraitsPrinter<DDGraphPass, false, pedigree::DDGraph *,
                                  llvm::AnalysisDependenceGraphPassTraits>;
  static char ID;

  DDGraphPrinterPass() : Base("ddg", ID) {}

  bool runOnFunction(llvm::Function &CurFunction) override {
    auto found = std::find(std::begin(DDGraphDOTFunctionWhitelist),
                           std::end(DDGraphDOTFunctionWhitelist),
                           CurFunction.getName().str());

    if (DDGraphDOTFunctionWhitelist.empty() ||
        std::end(DDGraphDOTFunctionWhitelist) != found)
      return Base::runOnFunction(CurFunction);
    else
      return false;
  }
};

} // namespace pedigree end

char pedigree::DDGraphPrinterPass::ID = 0;
static llvm::RegisterPass<pedigree::DDGraphPrinterPass>
    X("pedigree-ddg-dot", PRJ_CMDLINE_DESC("pedigree ddg DOT pass"), false,
      false);

// plugin registration for clang

// the solution was at the bottom of the header file
// 'llvm/Transforms/IPO/PassManagerBuilder.h'
// create a static free-floating callback that uses the legacy pass manager to
// add an instance of this pass and a static instance of the
// RegisterStandardPasses class

static void
registerPedigreeDDGraphPrinterPass(const llvm::PassManagerBuilder &Builder,
                               llvm::legacy::PassManagerBase &PM) {
  PM.add(new pedigree::DDGraphPrinterPass());

  return;
}

static llvm::RegisterStandardPasses
    RegisterPedigreeDDGraphPrinterPass(llvm::PassManagerBuilder::EP_EarlyAsPossible,
                                   registerPedigreeDDGraphPrinterPass);
