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
using Graph =
    adjacency_list<vecS, vecS, directedS, VertexPropertyTy, EdgePropertyTy>;

struct DataDependenceVertex {
  llvm::Instruction *instruction;

  explicit DataDependenceVertex(const llvm::Instruction *instruction) noexcept {
    this->instruction = const_cast<llvm::Instruction *>(instruction);
  }
};

struct DataDependenceEdge {
  DependenceType type;
  DependenceOrigin origin;
};

using DataDependenceGraph = Graph<DataDependenceVertex, DataDependenceEdge>;

template <typename GraphTy>
GraphTy make_ddg(const llvm::Function &CurFunc) noexcept {
  GraphTy g{};

  for (auto &bb : CurFunc)
    for (auto &ins : bb) {
      auto src = add_vertex(DataDependenceVertex(&ins), g);
      for (auto &u : ins.uses()) {
        auto *user = llvm::dyn_cast<llvm::Instruction>(u.getUser());
        if (user)
          auto dst = add_vertex(DataDependenceVertex(user), g);
      }
    }

  return g;
}

} // namespace pedigree end

#endif // header
