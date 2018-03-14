//
//
//

#include "Utils.hpp"

#include "MDGPass.hpp"

#include "llvm/Analysis/DOTGraphTraitsPass.h"
// using llvm::DOTGraphTraitsPrinter

#include "llvm/Pass.h"
// using llvm::RegisterPass

#include "llvm/IR/LegacyPassManager.h"
// using llvm::PassManagerBase

#include "llvm/Transforms/IPO/PassManagerBuilder.h"
// using llvm::PassManagerBuilder
// using llvm::RegisterStandardPasses

#include "llvm/Support/DOTGraphTraits.h"
// using llvm::DOTGraphTraits

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

template <> struct DOTGraphTraits<MDG *> : public DefaultDOTGraphTraits {
  using GraphTy = MDG;

  DOTGraphTraits(bool isSimple = false) : DefaultDOTGraphTraits(isSimple) {}

  static std::string getGraphName(const GraphTy *) { return "MDG"; }

  std::string getNodeLabel(const MemoryDependenceNode *Node, const GraphTy *Graph) {

    if (isSimple() || MDGDOTSimple)
      return getSimpleNodeLabel(Node, Graph);
    else
      return getCompleteNodeLabel(Node, Graph);
  }

  static std::string getCompleteNodeLabel(const MemoryDependenceNode *Node,
                                          const GraphTy *Graph) {
    std::string s;
    llvm::raw_string_ostream os(s);
    Node->getUnderlying()->print(os);

    return os.str();
  }

  static std::string getSimpleNodeLabel(const MemoryDependenceNode *Node,
                                        const GraphTy *Graph) {
    auto name = Node->getUnderlying()->getName();

    if (name.empty())
      return Node->getUnderlying()->getOpcodeName();
    else
      return name.str();
  }

  static std::string getNodeAttributes(const MemoryDependenceNode *Node,
                                       const GraphTy *Graph) {
    std::string attr;

    if (Graph->getEntryNode() == Node)
      attr = "color=grey,style=filled";

    return attr;
  }

  static std::string
  getEdgeAttributes(const MemoryDependenceNode *Node,
                    GraphTraits<GraphTy *>::ChildIteratorType EI,
                    const GraphTy *Graph) {
    if (MDGDOTEdgeAttributes.empty())
      return "color=blue";
    else
      return MDGDOTEdgeAttributes;
  }

  bool isNodeHidden(const MemoryDependenceNode *Node) {
    return isSimple() && !Node->numEdges() && !Node->getDependeeCount();
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
