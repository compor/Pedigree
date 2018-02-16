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

  explicit DataDependenceVertex(llvm::Instruction *const instruction) noexcept {
    this->instruction = instruction;
  }
};

struct DataDependenceEdge {
  DependenceType type;
  DependenceOrigin origin;
};

using DataDependenceGraph = Graph<DataDependenceVertex, DataDependenceEdge>;

template <typename GraphTy>
GraphTy make_ddg(const llvm::Function &CurFunc) noexcept {
  return {};
}

} // namespace pedigree end

#endif // header
