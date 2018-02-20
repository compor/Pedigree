//
//
//

#ifndef DATADEPENDENCEGRAPH_HPP
#define DATADEPENDENCEGRAPH_HPP

#include "llvm/IR/Instruction.h"
// using llvm::Instruction

#include "boost/graph/adjacency_list.hpp"
// using boost::adjacency_list
// using boost::vecS
// using boost::directedS

#include "boost/graph/labeled_graph.hpp"
// using boost::labeled_graph

#include <cstdint>
// using uint8_t

namespace pedigree {

using namespace boost;

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

template <typename VertexPropertyTy, typename EdgePropertyTy>
using UnderlyingGraphTy =
    adjacency_list<vecS, vecS, directedS, VertexPropertyTy, EdgePropertyTy>;

template <typename VertexPropertyTy, typename EdgePropertyTy>
using DependenceGraphTy =
    labeled_graph<UnderlyingGraphTy<VertexPropertyTy, EdgePropertyTy>,
                  const llvm::Instruction *>;

//

struct DataDependenceVertex {
  const llvm::Instruction *instruction;

  explicit DataDependenceVertex(const llvm::Instruction *ins = nullptr) noexcept
      : instruction(ins) {}
};

struct DataDependenceEdge {
  DependenceType type;
  DependenceOrigin origin;
};

using DataDependenceGraphTy =
    DependenceGraphTy<DataDependenceVertex, DataDependenceEdge>;

template <typename Graph>
auto CreateGraph(Graph &G, const llvm::Function &Func) noexcept -> void {
  for (auto &bb : Func)
    for (auto &ins : bb) {
      auto src = add_vertex(&ins, DataDependenceVertex(&ins), G);
      for (auto &u : ins.uses()) {
        // TODO: this adds all instructions encounter as vertices
        // provide decision option to user
        auto *user = llvm::dyn_cast<llvm::Instruction>(u.getUser());
        if (user) {
          auto dst = add_vertex(user, DataDependenceVertex(user), G);
          add_edge(src, dst, G);
        }
      }
    }
}

} // namespace pedigree end

// traits

namespace llvm {

template <> struct GraphTraits<pedigree::DataDependenceGraphTy> {
  using GraphTy = pedigree::DataDependenceGraphTy;
  using bgt = boost::graph_traits<GraphTy>;

  using NodeType = Instruction;
  using ChildIteratorType = bgt::vertex_iterator;

  // TODO think about inline

  static NodeType *getEntryNode(const GraphTy &G) {
    // TODO add assertion for null, hint: use std::tie
    //return *vertices(G).first;
    return nullptr;
  }

  //static ChildIteratorType child_begin(NodeType *Node) {
    //return target(*out_edges(G[Node], G).first, G);
  //}

  //static ChildIteratorType child_end(NodeType *Node) {
    //return target(*out_edges(G[Node], G).second, G);
  //}

  using nodes_iterator = bgt::vertex_iterator;

  static nodes_iterator nodes_begin(GraphTy *G) { return vertices(*G).first; }

  static nodes_iterator nodes_end(GraphTy *G) { return vertices(*G).second; }

  static unsigned size(GraphTy *G) { return num_vertices(*G); }
};

} // namespace llvm end

#endif // header
