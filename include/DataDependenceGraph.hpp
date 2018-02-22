//
//
//

#ifndef DATADEPENDENCEGRAPH_HPP
#define DATADEPENDENCEGRAPH_HPP

#include "llvm/IR/Instruction.h"
// using llvm::Instruction

#include "llvm/IR/InstVisitor.h"
// using llvm::InstVisitor

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

private:
  llvm::Instruction *m_Actual;

  EdgeStorageTy m_Edges;
};

class DataDependenceGraph {
public:
  using NodeMapTy = std::map<llvm::Instruction *, DependenceGraphNode *>;
  using vertices_size_type = NodeMapTy::size_type;
  using edges_size_type = DependenceGraphNode::edges_size_type;

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

#endif // header
