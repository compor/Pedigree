
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
// using llvm::cl::desc
// using llvm::cl::Hidden

#include "llvm/Support/raw_ostream.h"
// using llvm::raw_string_ostream

#include <string>
// using std::string

static llvm::cl::opt<std::string>
    DDGEdgeAttributes("pedigree-ddg-edge-attrs", llvm::cl::Hidden,
                      llvm::cl::desc("DOT edge attributes"));

static llvm::cl::opt<bool>
    DDGSimple("pedigree-ddg-simple", llvm::cl::Hidden,
              llvm::cl::desc("generate simple DOT graph"));

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

struct DDGPrinter : public llvm::DOTGraphTraitsPrinter<
                        DataDependenceGraphPass, false, DataDependenceGraph *,
                        llvm::AnalysisDependenceGraphPassTraits> {
  static char ID;

  DDGPrinter()
      : llvm::DOTGraphTraitsPrinter<DataDependenceGraphPass, false,
                                    DataDependenceGraph *,
                                    llvm::AnalysisDependenceGraphPassTraits>(
            "ddg", ID) {}
};

} // namespace pedigree end

char pedigree::DDGPrinter::ID = 0;
static llvm::RegisterPass<pedigree::DDGPrinter>
    X("pedigree-ddg-dot", PRJ_CMDLINE_DESC("pedigree ddg DOT pass"), false,
      false);

// plugin registration for clang

// the solution was at the bottom of the header file
// 'llvm/Transforms/IPO/PassManagerBuilder.h'
// create a static free-floating callback that uses the legacy pass manager to
// add an instance of this pass and a static instance of the
// RegisterStandardPasses class

static void registerPedigreeDDGPrinter(const llvm::PassManagerBuilder &Builder,
                                       llvm::legacy::PassManagerBase &PM) {
  PM.add(new pedigree::DDGPrinter());

  return;
}

static llvm::RegisterStandardPasses
    RegisterPedigreeDDGPrinter(llvm::PassManagerBuilder::EP_EarlyAsPossible,
                               registerPedigreeDDGPrinter);
