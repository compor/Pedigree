//
//
//

#ifndef PEDIGREE_LLVMDOTGRAPHTRAITSHELPER_HPP
#define PEDIGREE_LLVMDOTGRAPHTRAITSHELPER_HPP

#include "llvm/Support/DOTGraphTraits.h"
// using llvm::DOTGraphTraits

#include "llvm/Support/raw_ostream.h"
// using llvm::raw_string_ostream

#include <string>
// using std::string

namespace pedigree {

template <typename GraphT> struct LLVMDOTDependenceGraphTraitsBase {};

template <typename GraphT>
struct LLVMDOTDependenceGraphTraitsBase<GraphT *>
    : public llvm::DefaultDOTGraphTraits {
  using GraphType = GraphT;
  using GT = llvm::GraphTraits<GraphType *>;
  using NodeType = typename GT::NodeType;

  LLVMDOTDependenceGraphTraitsBase(bool isSimple)
      : llvm::DefaultDOTGraphTraits(isSimple) {}

  static std::string getGraphName(const GraphType *) {
    return "Dependence Graph";
  }

  static std::string getCompleteNodeLabel(const NodeType *Node,
                                          const GraphType *Graph) {
    std::string s;
    llvm::raw_string_ostream os(s);
    Node->get()->print(os);

    return os.str();
  }

  static std::string getSimpleNodeLabel(const NodeType *Node,
                                        const GraphType *Graph) {
    auto name = Node->get()->getName();

    return name.str();
  }

  static std::string getNodeAttributes(const NodeType *Node,
                                       const GraphType *Graph) {
    std::string attr;

    if (Graph->getEntryNode() == Node)
      attr = "color=grey,style=filled";

    return attr;
  }

  static std::string getEdgeAttributes(const NodeType *Node,
                                       typename GT::ChildIteratorType EI,
                                       const GraphType *Graph) {
    using DIT = pedigree::DependenceInfoTraits<typename NodeType::EdgeInfoType>;
    return DIT::toDOTAttributes(Node->getEdgeInfo(*EI));
  }

  bool isNodeHidden(const NodeType *Node) {
    return isSimple() && !Node->numEdges() && !Node->getDependeeCount();
  }
};

} // namespace pedigree end

#endif // header
