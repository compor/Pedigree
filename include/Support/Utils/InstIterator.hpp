//
//
//

#ifndef PEDIGREE_INSTITERATOR_HPP
#define PEDIGREE_INSTITERATOR_HPP

#include "llvm/Config/llvm-config.h"
// version macros

#include "llvm/ADT/iterator_range.h"
// llvm::make_range

#include <iterator>
// using std::iterator_traits
// using std::begin
// using std::end

#include <utility>
// using std::declval

#include <cstddef>
// using std::ptrdiff_t

namespace pedigree {

template <typename InstructionBlockIteratorT> struct InstIterator {
  using InstructionBlockT =
      typename std::iterator_traits<InstructionBlockIteratorT>::value_type;

// We're using the member function to obtain the iterator instead of
// std::begin, because that will be bound to the overload that accepts const
// ref as a parameter, thus requiring a lot of juggling around types to detect
// if the originating container is const or not

#if LLVM_VERSION_MAJOR >= 4 && LLVM_VERSION_MINOR >= 0
  using InstructionIteratorT =
      decltype(std::declval<InstructionBlockT>()->begin());
#else
  using InstructionIteratorT =
      decltype(std::declval<InstructionBlockT>().begin());
#endif

  using InstructionT =
      typename std::iterator_traits<InstructionIteratorT>::value_type;

  using difference_type = std::ptrdiff_t;
  using value_type = InstructionT;
  using pointer = typename std::iterator_traits<InstructionIteratorT>::pointer;
  using reference =
      typename std::iterator_traits<InstructionIteratorT>::reference;
  using iterator_category = std::forward_iterator_tag;

  InstIterator() = delete;
  InstIterator(const InstIterator &Other) = default;

  InstIterator(InstructionBlockIteratorT Begin,
               InstructionBlockIteratorT End) noexcept : CurBlockI(Begin),
                                                         EndBlockI(End) {
    if (CurBlockI != EndBlockI) {
      auto &curBlock = ToObj(*CurBlockI);
      CurInstI = std::begin(curBlock);
      EndInstI = std::end(curBlock);
      advance();
    }
  }

  InstIterator(InstructionBlockIteratorT End) : InstIterator(End, End) {}

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
    while (CurInstI == EndInstI) {
      ++CurBlockI;

      if (CurBlockI == EndBlockI) {
        return;
      }

      auto &curBlock = ToObj(*CurBlockI);
      CurInstI = std::begin(curBlock);
      EndInstI = std::end(curBlock);
    }
  }

  InstructionBlockIteratorT CurBlockI;
  InstructionBlockIteratorT EndBlockI;

  InstructionIteratorT CurInstI;
  InstructionIteratorT EndInstI;
};

template <typename T> inline decltype(auto) make_inst_begin(T &Container) {
  using IteratorT = decltype(std::begin(Container));
  return InstIterator<IteratorT>(std::begin(Container), std::end(Container));
}

template <typename T> inline decltype(auto) make_inst_end(T &Container) {
  using IteratorT = decltype(std::end(Container));
  return InstIterator<IteratorT>(std::end(Container));
}

template <typename T> inline decltype(auto) make_inst_range(T &Container) {
  return llvm::make_range(make_inst_begin(Container), make_inst_end(Container));
}

template <typename T> inline decltype(auto) make_inst_range(T Begin, T End) {
  return llvm::make_range(InstIterator<T>(Begin, End), InstIterator<T>(End));
}

} // namespace pedigree

#endif // header
