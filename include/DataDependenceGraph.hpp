//
//
//

#ifndef DATADEPENDENCEGRAPH_HPP
#define DATADEPENDENCEGRAPH_HPP

#include "llvm/IR/Instruction.h"
// using llvm::Instruction

#include "llvm/IR/InstVisitor.h"
// using llvm::InstVisitor

#include "llvm/ADT/STLExtras.h"
// using llvm::mapped_iterator

#include <vector>
// using std::vector

#include <map>
// using std::map

#include <utility>
// using std::pair

#include <algorithm>
// using std::for_each

#include <cstdint>
// using uint8_t

namespace pedigree {

enum class DependenceType : uint8_t {
  unknown = 0,
  flow = 1,
  anti = 2,
  out = 4,
};

enum class DependenceOrigin : uint8_t {
  unknown = 0,
  data = 1,
  memory = 2,
  control = 4,
};

struct DataDependenceInfo {
  DependenceType type;
  DependenceOrigin origin;
};

class DependenceGraphNode {
public:
  using DependenceRecordTy =
      std::pair<DependenceGraphNode *, DataDependenceInfo>;
  using EdgeStorageTy = std::vector<DependenceRecordTy>;
  using edges_size_type = EdgeStorageTy::size_type;

  DependenceGraphNode(llvm::Instruction *CurInstruction)
      : m_Actual(CurInstruction) {}

  llvm::Instruction *getActual() { return m_Actual; }

  void addDependentNode(DependenceGraphNode *Node) {
    m_Edges.emplace_back(
        Node, DataDependenceInfo{DependenceType::flow, DependenceOrigin::data});
  }

  edges_size_type numEdges() { return m_Edges.size(); }

  using iterator = EdgeStorageTy::iterator;
  using const_iterator = EdgeStorageTy::const_iterator;

  inline decltype(auto) begin() { return m_Edges.begin(); }
  inline decltype(auto) end() { return m_Edges.end(); }
  inline decltype(auto) begin() const { return m_Edges.begin(); }
  inline decltype(auto) end() const { return m_Edges.end(); }

private:
  llvm::Instruction *m_Actual;

  EdgeStorageTy m_Edges;
};

class DataDependenceGraph {
public:
  using NodeMapTy = std::map<llvm::Instruction *, DependenceGraphNode *>;
  using vertices_size_type = NodeMapTy::size_type;
  using edges_size_type = DependenceGraphNode::edges_size_type;

  using iterator = NodeMapTy::iterator;
  using const_iterator = NodeMapTy::const_iterator;

  DataDependenceGraph() = default;
  ~DataDependenceGraph() {
    for (auto &e : m_NodeMap)
      delete e.second;
  }

  DependenceGraphNode *
  getOrInsertNode(const llvm::Instruction *CurInstruction) {
    auto *curNode = const_cast<llvm::Instruction *>(CurInstruction);
    auto &node = m_NodeMap[curNode];
    if (node)
      return node;

    return node = new DependenceGraphNode(curNode);
  }

  vertices_size_type numVertices() const { return m_NodeMap.size(); }

  edges_size_type numEdges() const {
    NodeMapTy::size_type n{};
    std::for_each(std::begin(m_NodeMap), std::end(m_NodeMap),
                  [&n](const auto &e) { n += e.second->numEdges(); });
    return n;
  }

  inline decltype(auto) begin() { return m_NodeMap.begin(); }
  inline decltype(auto) end() { return m_NodeMap.end(); }
  inline decltype(auto) begin() const { return m_NodeMap.begin(); }
  inline decltype(auto) end() const { return m_NodeMap.end(); }

  const DependenceGraphNode *getEntryNode() const { return begin()->second; }
  DependenceGraphNode *getEntryNode() {
    return const_cast<DependenceGraphNode *>(getEntryNode());
  }

private:
  NodeMapTy m_NodeMap;
};

//

class DataDependenceGraphBuilder
    : public llvm::InstVisitor<DataDependenceGraphBuilder> {
  DataDependenceGraph &m_Graph;

public:
  DataDependenceGraphBuilder(DataDependenceGraph &Graph) : m_Graph(Graph) {}

  void visitInstruction(llvm::Instruction &CurInstruction) {
    auto src = m_Graph.getOrInsertNode(&CurInstruction);
    for (auto &u : CurInstruction.uses()) {
      auto *user = llvm::dyn_cast<llvm::Instruction>(u.getUser());
      if (user) {
        auto dst = m_Graph.getOrInsertNode(user);
        src->addDependentNode(dst);
      }
    }
  }
};

} // namespace pedigree end

namespace llvm {

using namespace pedigree;

template <> struct GraphTraits<DependenceGraphNode *> {
  using NodeType = DependenceGraphNode;

  using ChildPairTy = DependenceGraphNode::DependenceRecordTy;
  using ChildDerefFuncTy = std::function<DependenceGraphNode *(ChildPairTy)>;

  using ChildIteratorType =
      llvm::mapped_iterator<NodeType::iterator, ChildDerefFuncTy>;

  static NodeType *getEntryNode(NodeType *G) { return G; }

  static ChildIteratorType child_begin(NodeType *G) {
    return llvm::map_iterator(G->begin(), ChildDerefFuncTy(ChildDeref));
  }
  static ChildIteratorType child_end(NodeType *G) {
    return llvm::map_iterator(G->end(), ChildDerefFuncTy(ChildDeref));
  }

  static DependenceGraphNode *ChildDeref(ChildPairTy P) { return P.first; }
};

template <>
struct GraphTraits<DataDependenceGraph *>
    : public GraphTraits<DependenceGraphNode *> {
  using GraphTy = DataDependenceGraph;

  using GraphPairTy = std::pair<llvm::Instruction *, DependenceGraphNode *>;
  using DerefFuncTy = std::function<DependenceGraphNode &(GraphPairTy)>;

  using nodes_iterator = llvm::mapped_iterator<GraphTy::iterator, DerefFuncTy>;

  static NodeType *getEntryNode(GraphTy *G) { return G->getEntryNode(); }

  static nodes_iterator nodes_begin(GraphTy *G) {
    return llvm::map_iterator(G->begin(), DerefFuncTy(GraphDeref));
  }
  static nodes_iterator nodes_end(GraphTy *G) {
    return llvm::map_iterator(G->end(), DerefFuncTy(GraphDeref));
  }

  static DependenceGraphNode &GraphDeref(GraphPairTy P) { return *P.second; }
};

} // namespace llvm end

#endif // header
