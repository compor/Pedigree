//
//
//

#pragma once

#include <cstdint>
// using std::uint8_t

namespace pedigree {

enum class AnalysisScope : std::uint8_t {
  // Unknown
  Block,
  Function,
  Interprocedural
};

} // namespace pedigree

