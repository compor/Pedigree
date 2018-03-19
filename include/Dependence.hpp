//
//
//

#ifndef DEPENDENCE_HPP
#define DEPENDENCE_HPP

#include <bitset>
// using std::bitset

#include <cstdint>
// using uint8_t

#include <type_traits>
// using std::underlying_type

namespace pedigree {

struct NoDependenceInfo {};

//

enum class DependenceType : uint8_t {
  unknown = 0,
  flow,
  anti,
  out,
};

enum class DependenceOrigin : uint8_t {
  unknown = 0,
  data,
  memory,
  control,
};

struct BasicDependenceInfo {
  std::bitset<sizeof(std::underlying_type<DependenceType>::type)> type;
  std::bitset<sizeof(std::underlying_type<DependenceOrigin>::type)> origin;
};

} // namespace pedigree end

#endif // header
