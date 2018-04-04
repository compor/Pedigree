//
//
//

#ifndef PEDIGREE_ANALYSISSCOPE_HPP
#define PEDIGREE_ANALYSISSCOPE_HPP

#include <cstdint>
// using uint8_t

namespace pedigree {

enum class AnalysisScope : uint8_t {
  // Uknown
  Block,
  Function,
  Interprocedural
};

} // namespace pedigree end

#endif // header
