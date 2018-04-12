//
//
//

#ifndef PEDIGREE_LLVMANALYSISGRAPHTRAITS_HPP
#define PEDIGREE_LLVMANALYSISGRAPHTRAITS_HPP

#include <memory>
// using std::addressof

namespace pedigree {

template <typename AnalysisT, typename GraphT>
struct LLVMAnalysisDependenceGraphPassTraitsHelperBase {
  static GraphT *getGraph(AnalysisT *A) {
    return std::addressof(A->getGraph());
  }
};

} // namespace pedigree

#endif // header
