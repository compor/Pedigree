//
//
//

#ifndef PEDIGREE_LLVMDOTGRAPHTRAITSHELPER_HPP
#define PEDIGREE_LLVMDOTGRAPHTRAITSHELPER_HPP

#include "pedigree/Analysis/Info/NodeInfo/NodeInfoDOTTraits.hpp"

#include "pedigree/Analysis/Info/EdgeInfo/EdgeInfoDOTTraits.hpp"

#include "pedigree/Analysis/Units/UnitDOTTraits.hpp"

#include "llvm/Support/DOTGraphTraits.h"
// using llvm::DOTGraphTraits

#include "llvm/ADT/GraphTraits.h"
// using llvm::GraphTraits

#include <string>
// using std::string

#include <type_traits>
// using std::is_same
// using std::is_pointer
// using std::false_type

namespace pedigree {

template <typename GraphT> struct LLVMDOTDependenceGraphTraitsHelperBase {
  static_assert(
      std::is_same<typename std::is_pointer<GraphT>::type,
                   std::false_type::type>::value,
      "Traits class needs to be partially specialized for pointer types!");
};

template <typename GraphT>
struct LLVMDOTDependenceGraphTraitsHelperBase<GraphT *>
    : public llvm::DefaultDOTGraphTraits {
  using GraphType = GraphT;
  using GT = llvm::GraphTraits<GraphType *>;
  using NodeType = typename GT::NodeType;
  using Base = llvm::DefaultDOTGraphTraits;

  LLVMDOTDependenceGraphTraitsHelperBase(bool isSimple)
      : llvm::DefaultDOTGraphTraits(isSimple) {}

  static std::string getGraphName(const GraphType *) {
    return "Dependence Graph";
  }

  static std::string getCompleteNodeLabel(const NodeType *Node,
                                          const GraphType *Graph) {
    return UnitDOTTraits<typename NodeType::UnitType>::print(Node->unit());
  }

  static std::string getSimpleNodeLabel(const NodeType *Node,
                                        const GraphType *Graph) {
    return UnitDOTTraits<typename NodeType::UnitType>::name(Node->unit());
  }

  static std::string getNodeAttributes(const NodeType *Node,
                                       const GraphType *Graph) {
    std::string attr;

    if (Graph->getEntryNode() == Node) {
      attr = "color=grey, style=filled, ";
    }

    attr += NodeInfoDOTTraits<typename NodeType::NodeInfoType::value_type>::
        toDOTAttributes(Node->info());

    return attr;
  }

  bool isNodeHidden(const NodeType *Node) {
    return isSimple() && !Node->numOutEdges() && !Node->numInEdges();
  }

  std::string getNodeLabel(const NodeType *Node, const GraphType *Graph) {
    return isSimple() ? getSimpleNodeLabel(Node, Graph)
                      : getCompleteNodeLabel(Node, Graph);
  }

  static std::string getEdgeAttributes(const NodeType *Node,
                                       typename GT::ChildIteratorType EI,
                                       const GraphType *Graph) {
    using DIT = EdgeInfoDOTTraits<typename NodeType::EdgeInfoType::value_type>;

    auto info = Node->getEdgeInfo(*EI);
    std::string infoStr{};

    if (info) {
      infoStr = DIT::toDOTAttributes(info.value());
    }

    return infoStr + Base::getEdgeAttributes(Node, EI, Graph);
  }
};

} // namespace pedigree

#endif // header
