//
//
//

#ifndef DEPENDENCE_HPP
#define DEPENDENCE_HPP

#include <cstdint>
// using uint8_t

namespace pedigree {

struct NoEdgeInfo {};

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

} // namespace pedigree end

#endif // header
