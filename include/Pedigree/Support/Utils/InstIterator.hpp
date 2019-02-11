//
//
//

#pragma once

#include "llvm/Config/llvm-config.h"
// version macros

#include "llvm/ADT/iterator_range.h"
// llvm::make_range

#include "boost/type_traits/detected.hpp"
// using boost::detected_t

#include <iterator>
// using std::iterator_traits
// using std::begin
// using std::end

#include <utility>
// using std::declval

#include <cstddef>
// using std::ptrdiff_t

namespace pedigree {

namespace detail {
template <typename T>
using inner_iterator_of_val = decltype(
    std::declval<typename std::iterator_traits<T>::value_type>().begin());

template <typename T>
using inner_iterator_of_ptr = decltype(
    std::declval<typename std::iterator_traits<T>::value_type>()->begin());

} // namespace detail

template <typename InstructionBlockIteratorT> struct InstIterator {
  using InstructionBlockT =
      typename std::iterator_traits<InstructionBlockIteratorT>::value_type;

  using InstructionIteratorT =
      std::conditional_t<std::is_pointer<InstructionBlockT>::value,
                         boost::detected_t<detail::inner_iterator_of_ptr,
                                           InstructionBlockIteratorT>,
                         boost::detected_t<detail::inner_iterator_of_val,
                                           InstructionBlockIteratorT>>;

  using InstructionT =
      typename std::iterator_traits<InstructionIteratorT>::value_type;

  using difference_type = std::ptrdiff_t;
  using value_type = InstructionT;
  using pointer = typename std::iterator_traits<InstructionIteratorT>::pointer;
  using reference =
      typename std::iterator_traits<InstructionIteratorT>::reference;
  using iterator_category = std::forward_iterator_tag;

  InstIterator() = default;
  InstIterator(const InstIterator &Other) = default;

  InstIterator(InstructionBlockIteratorT Begin,
               InstructionBlockIteratorT End) noexcept
      : CurBlockI(Begin), EndBlockI(End) {
    using std::begin;
    using std::end;

    if (CurBlockI != EndBlockI) {
      auto &curBlock = ToObj(*CurBlockI);
      CurInstI = begin(curBlock);
      EndInstI = end(curBlock);
      advance();
    }
  }

  explicit InstIterator(InstructionBlockIteratorT End)
      : InstIterator(End, End) {}

  reference operator*() const { return *CurInstI; }
  pointer operator->() const { return &operator*(); }

  InstIterator &operator++() {
    ++CurInstI;
    advance();

    return *this;
  }

  InstIterator operator++(int) {
    auto tmp = *this;
    operator++();

    return tmp;
  }

  bool operator==(const InstIterator &Other) const {
    auto cond = (CurBlockI == Other.CurBlockI && EndBlockI == Other.EndBlockI);

    return CurBlockI == EndBlockI ? cond
                                  : (cond && CurInstI == Other.CurInstI &&
                                     EndInstI == Other.EndInstI);
  }

  bool operator!=(const InstIterator &Other) const {
    return !operator==(Other);
  }

private:
  void advance() noexcept {
    using std::begin;
    using std::end;

    while (CurInstI == EndInstI) {
      ++CurBlockI;

      if (CurBlockI == EndBlockI) {
        return;
      }

      auto &curBlock = ToObj(*CurBlockI);
      CurInstI = begin(curBlock);
      EndInstI = end(curBlock);
    }
  }

  InstructionBlockIteratorT CurBlockI;
  InstructionBlockIteratorT EndBlockI;

  InstructionIteratorT CurInstI;
  InstructionIteratorT EndInstI;
};

template <typename T> inline decltype(auto) make_inst_begin(T &Container) {
  using std::begin;
  using std::end;
  using IteratorT = decltype(begin(Container));
  return InstIterator<IteratorT>(begin(Container), end(Container));
}

template <typename T> inline decltype(auto) make_inst_end(T &Container) {
  using std::end;
  using IteratorT = decltype(end(Container));
  return InstIterator<IteratorT>(end(Container));
}

template <typename T> inline decltype(auto) make_inst_range(T &Container) {
  return llvm::make_range(make_inst_begin(Container), make_inst_end(Container));
}

template <typename T> inline decltype(auto) make_inst_range(T Begin, T End) {
  return llvm::make_range(InstIterator<T>(Begin, End), InstIterator<T>(End));
}

} // namespace pedigree

