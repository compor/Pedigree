//
//
//

#pragma once

#include <memory>
// using std::addressof

namespace pedigree {

template <typename AnalysisT, typename GraphT>
struct LLVMAnalysisStandardDependenceGraphPassTraitsHelperBase {
  static GraphT *getGraph(AnalysisT *A) {
    return std::addressof(A->getGraph());
  }
};

template <typename AnalysisT, typename GraphT>
struct LLVMAnalysisInstructionDependenceGraphPassTraitsHelperBase {
  static GraphT *getGraph(AnalysisT *A) {
    return std::addressof(A->getInstGraph());
  }
};

} // namespace pedigree

