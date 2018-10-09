//
//
//

#ifndef PEDIGREE_GENERICDEPENDENCEGRAPHEDGEITERATOR_HPP
#define PEDIGREE_GENERICDEPENDENCEGRAPHEDGEITERATOR_HPP

#include "boost/iterator/iterator_facade.hpp"
// using boost::iterator_facade
// using boost::iterator_core_access

#include <iterator>
// using std::iterator_traits
// using std::advance

namespace pedigree {
namespace detail {

// TODO reconsider the choice of traversal tag to something more permitting
// based on the categories of the two iterators

template <typename NodeIteratorT, typename NodeEdgeIteratorT>
class GenericDependenceGraphEdgeIterator
    : public boost::iterator_facade<
          GenericDependenceGraphEdgeIterator<NodeIteratorT, NodeEdgeIteratorT>,
          typename NodeEdgeIteratorT::value_type, std::forward_iterator_tag> {
public:
  using base = boost::iterator_facade<
      GenericDependenceGraphEdgeIterator<NodeIteratorT, NodeEdgeIteratorT>,
      typename NodeEdgeIteratorT::value_type, std::forward_iterator_tag>;

  using difference_type = typename base::difference_type;
  using value_type = typename base::value_type;
  using pointer = typename base::pointer;
  using reference = typename base::reference;
  using iterator_category = typename base::iterator_category;

  GenericDependenceGraphEdgeIterator() = default;

  GenericDependenceGraphEdgeIterator(NodeIteratorT Begin, NodeIteratorT End)
      : CurNI(Begin), EndNI(End) {
    initNodeEdgeIterator();

    if (isNodeEdgesIterationComplete()) {
      advanceToNextValid();
    }
  }

  template <typename U, typename K>
  GenericDependenceGraphEdgeIterator(
      const GenericDependenceGraphEdgeIterator<U, K> &Other)
      : CurNI(Other.NI), EndNI(Other.NI), CurNEI(Other.CurNEI),
        EndNEI(Other.EndNEI) {}

private:
  bool isNodesIterationComplete() { return CurNI == EndNI; }
  bool isNodeEdgesIterationComplete() { return CurNEI == EndNEI; }

  void initNodeEdgeIterator() {
    CurNEI = EndNEI = {};

    if (isNodesIterationComplete()) {
      return;
    }

    CurNEI = (*CurNI)->edges_begin();
    EndNEI = (*CurNI)->edges_end();
  }

  void advanceToNextValid() {
    const auto step = 1;

    while (!isNodesIterationComplete()) {
      if (!isNodeEdgesIterationComplete()) {
        std::advance(CurNEI, step);
      }

      while (isNodeEdgesIterationComplete() && !isNodesIterationComplete()) {
        std::advance(CurNI, step);
        initNodeEdgeIterator();
      }

      if (!isNodeEdgesIterationComplete()) {
        break;
      }
    }
  }

  friend class boost::iterator_core_access;

  reference dereference() const { return *CurNEI; }

  template <typename U, typename K>
  bool equal(const GenericDependenceGraphEdgeIterator<U, K> &Other) const {
    return CurNI == Other.CurNI && EndNI == Other.EndNI &&
           CurNEI == Other.CurNEI && EndNEI == Other.EndNEI;
  }

  void increment() { advanceToNextValid(); }

  NodeIteratorT CurNI;
  NodeIteratorT EndNI;
  NodeEdgeIteratorT CurNEI;
  NodeEdgeIteratorT EndNEI;
};

} // namespace detail
} // namespace pedigree

#endif // header
