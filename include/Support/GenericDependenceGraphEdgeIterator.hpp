//
//
//

#ifndef PEDIGREE_GENERICDEPENDENCEGRAPHEDGEITERATOR_HPP
#define PEDIGREE_GENERICDEPENDENCEGRAPHEDGEITERATOR_HPP

#include "boost/iterator/iterator_facade.hpp"

#include <iterator>
// using std::iterator_traits
// using std::advance

namespace pedigree {
namespace detail {

// TODO reconsider the choice of traversal tag to something more permitting
// based on the categories of the two iterators

template <typename NodeIteratorT, typename NodeEdgeIteratorT>
class GenericDependenceGraphEdgeIterator
    : public boost::iterator_facade<NodeEdgeIteratorT,
                                    typename NodeEdgeIteratorT::value_type,
                                    std::forward_iterator_tag> {
public:
  using base = boost::iterator_facade<NodeEdgeIteratorT,
                                      typename NodeEdgeIteratorT::value_type,
                                      std::forward_iterator_tag>;

  using difference_type = typename base::difference_type;
  using value_type = typename base::value_type;
  using pointer = typename base::pointer;
  using reference = typename base::reference;
  using iterator_category = typename base::iterator_category;

  GenericDependenceGraphEdgeIterator() : CurEdgeDistance(0) {}

  GenericDependenceGraphEdgeIterator(NodeIteratorT Begin, NodeIteratorT End)
      : CurNI(Begin), EndNI(End), CurEdgeDistance(0) {
    initNodeEdgeIterator();
  }

  template <typename U, typename K>
  GenericDependenceGraphEdgeIterator(
      const GenericDependenceGraphEdgeIterator<U, K> &Other)
      : CurNI(Other.NI), EndNI(Other.NI), CurNEI(Other.CurNEI),
        CurEdgeDistance(Other.CurEdgeDistance) {}

private:
  bool isNodeIterationComplete() { return CurNI == EndNI; }

  void initNodeEdgeIterator() {
    if (isNodeIterationComplete())
      return;

    CurNEI = (*CurNI)->begin();
    CurEdgeDistance = std::distance(CurNEI, (*CurNI)->end());
  }

  void advanceToNextValid() {
    while (!isNodeIterationComplete()) {
      if (CurEdgeDistance) {
        std::advance(CurNEI, 1);
        --CurEdgeDistance;
        break;
      } else {
        ++CurNI;
        initNodeEdgeIterator();
      }
    }
  }

  friend class boost::iterator_core_access;

  reference dereference() const { return *CurNEI; }

  template <typename U, typename K>
  bool equal(const GenericDependenceGraphEdgeIterator<U, K> &Other) const {
    return CurNI == Other.CurNI && EndNI == Other.EndNI &&
           CurNEI == Other.CurNEI && CurEdgeDistance == Other.CurEdgeDistance;
  }

  void increment() { advanceToNextValid(); }

  NodeIteratorT CurNI;
  NodeIteratorT EndNI;
  NodeEdgeIteratorT CurNEI;
  difference_type CurEdgeDistance;
};

} // namespace detail end
} // namespace pedigree end

#endif // header
