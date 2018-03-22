//
//
//

#include "Config.hpp"

#include "Utils.hpp"

#include "Analysis/Passes/DDGPass.hpp"

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
    DDGDOTEdgeAttributes("pedigree-ddg-dot-edge-attrs", llvm::cl::Hidden,
                         llvm::cl::desc("DDG DOT edge attributes"));

static llvm::cl::opt<bool>
    DDGDOTSimple("pedigree-ddg-dot-simple", llvm::cl::Hidden,
                 llvm::cl::desc("generate simple DDG DOT graph"));

static llvm::cl::list<std::string> DDGDOTFunctionWhitelist(
    "pedigree-ddg-dot-func-wl", llvm::cl::Hidden,
    llvm::cl::desc("generate DDG DOT graph only for these functions"));

namespace llvm {

template <>
struct DOTGraphTraits<pedigree::DDG *>
    : public pedigree::LLVMDOTDependenceGraphTraitsBase<pedigree::DDG *> {
  using Base = pedigree::LLVMDOTDependenceGraphTraitsBase<pedigree::DDG *>;

  DOTGraphTraits(bool isSimple = false) : Base(isSimple) {}

  static std::string getGraphName(const GraphType *) { return "DDG"; }

  std::string getNodeLabel(const NodeType *Node, const GraphType *Graph) {
    return isSimple() || DDGDOTSimple ? getSimpleNodeLabel(Node, Graph)
                                      : getCompleteNodeLabel(Node, Graph);
  }

  static std::string getEdgeAttributes(const NodeType *Node,
                                       typename GT::ChildIteratorType EI,
                                       const GraphType *Graph) {
    return DDGDOTEdgeAttributes.empty()
               ? Base::getEdgeAttributes(Node, EI, Graph)
               : DDGDOTEdgeAttributes.getValue();
  }
};

struct AnalysisDependenceGraphPassTraits {
  static pedigree::DDG *getGraph(pedigree::DDGPass *P) {
    return &P->getGraph();
  }
};

} // namespace llvm end

namespace pedigree {

struct DDGPrinterPass : public llvm::DOTGraphTraitsPrinter<
                            DDGPass, false, pedigree::DDG *,
                            llvm::AnalysisDependenceGraphPassTraits> {
  using Base =
      llvm::DOTGraphTraitsPrinter<DDGPass, false, pedigree::DDG *,
                                  llvm::AnalysisDependenceGraphPassTraits>;
  static char ID;

  DDGPrinterPass() : Base("ddg", ID) {}

  bool runOnFunction(llvm::Function &CurFunction) override {
    auto found = std::find(std::begin(DDGDOTFunctionWhitelist),
                           std::end(DDGDOTFunctionWhitelist),
                           CurFunction.getName().str());

    if (DDGDOTFunctionWhitelist.empty() ||
        std::end(DDGDOTFunctionWhitelist) != found)
      return Base::runOnFunction(CurFunction);
    else
      return false;
  }
};

} // namespace pedigree end

char pedigree::DDGPrinterPass::ID = 0;
static llvm::RegisterPass<pedigree::DDGPrinterPass>
    X("pedigree-ddg-dot", PRJ_CMDLINE_DESC("pedigree ddg DOT pass"), false,
      false);

// plugin registration for clang

// the solution was at the bottom of the header file
// 'llvm/Transforms/IPO/PassManagerBuilder.h'
// create a static free-floating callback that uses the legacy pass manager to
// add an instance of this pass and a static instance of the
// RegisterStandardPasses class

static void
registerPedigreeDDGPrinterPass(const llvm::PassManagerBuilder &Builder,
                               llvm::legacy::PassManagerBase &PM) {
  PM.add(new pedigree::DDGPrinterPass());

  return;
}

static llvm::RegisterStandardPasses
    RegisterPedigreeDDGPrinterPass(llvm::PassManagerBuilder::EP_EarlyAsPossible,
                                   registerPedigreeDDGPrinterPass);
