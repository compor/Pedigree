
#include "Utils.hpp"

#include "DataDependenceGraphPass.hpp"

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

namespace {

static llvm::cl::opt<std::string>
    DDGEdgeAttributes("pedigree-ddg-dot-edge-attrs", llvm::cl::Hidden,
                      llvm::cl::desc("DDG DOT edge attributes"));

static llvm::cl::opt<bool>
    DDGSimple("pedigree-ddg-dot-simple", llvm::cl::Hidden,
              llvm::cl::desc("generate simple DDG DOT graph"));

static llvm::cl::list<std::string> DDGDOTFunctionWhitelist(
    "pedigree-ddg-dot-func-wl", llvm::cl::Hidden,
    llvm::cl::desc("generate DDG DOT graph only for these functions"));

} // anonymous namespace end

namespace llvm {

template <>
struct DOTGraphTraits<DataDependenceGraph *> : public DefaultDOTGraphTraits {
  using GraphTy = DataDependenceGraph;

  DOTGraphTraits(bool isSimple = false) : DefaultDOTGraphTraits(isSimple) {}

  static std::string getGraphName(const GraphTy *) { return "DDG"; }

  std::string getNodeLabel(const DependenceGraphNode *Node,
                           const GraphTy *Graph) {

    if (isSimple() || DDGSimple)
      return getSimpleNodeLabel(Node, Graph);
    else
      return getCompleteNodeLabel(Node, Graph);
  }

  static std::string getCompleteNodeLabel(const DependenceGraphNode *Node,
                                          const GraphTy *Graph) {
    std::string s;
    llvm::raw_string_ostream os(s);
    Node->getActual()->print(os);

    return os.str();
  }

  static std::string getSimpleNodeLabel(const DependenceGraphNode *Node,
                                        const GraphTy *Graph) {
    auto name = Node->getActual()->getName();

    if (name.empty())
      return Node->getActual()->getOpcodeName();
    else
      return name.str();
  }

  static std::string getNodeAttributes(const DependenceGraphNode *Node,
                                       const GraphTy *Graph) {
    std::string attr;

    if (Graph->getEntryNode() == Node)
      attr = "color=grey,style=filled";

    return attr;
  }

  static std::string
  getEdgeAttributes(const DependenceGraphNode *Node,
                    GraphTraits<GraphTy *>::ChildIteratorType EI,
                    const GraphTy *Graph) {
    if (DDGEdgeAttributes.empty())
      return "color=blue";
    else
      return DDGEdgeAttributes;
  }

  bool isNodeHidden(const DependenceGraphNode *Node) {
    return isSimple() && !Node->numEdges() && !Node->getDependeeCount();
  }
};

using namespace pedigree;

struct AnalysisDependenceGraphPassTraits {
  static DataDependenceGraph *getGraph(DataDependenceGraphPass *P) {
    return &P->getGraph();
  }
};

} // namespace llvm end

namespace pedigree {

struct DDGPrinterPass
    : public llvm::DOTGraphTraitsPrinter<
          DataDependenceGraphPass, false, DataDependenceGraph *,
          llvm::AnalysisDependenceGraphPassTraits> {
  using Base =
      llvm::DOTGraphTraitsPrinter<DataDependenceGraphPass, false,
                                  DataDependenceGraph *,
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
