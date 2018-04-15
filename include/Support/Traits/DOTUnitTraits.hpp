//
//
//

#ifndef PEDIGREE_DOTUNITTRAITS_HPP
#define PEDIGREE_DOTUNITTRAITS_HPP

#include "Config.hpp"

#include "Support/Traits/TypeTraits.hpp"

#include "Support/Utils/UniformObjectAccess.hpp"

#include "llvm/IR/Instruction.h"
// using llvm::Instruction

#include "llvm/IR/InstrTypes.h"
// using llvm::TerminatorInst

#include "llvm/Support/Casting.h"
// using llvm::dyn_cast

#include "llvm/Support/raw_ostream.h"
// using llvm::raw_string_ostream

#include <type_traits>
// using std::integral_constant
// using std::enable_if

#include <string>
// using std::string

namespace llvm {
class BasicBlock;
class Function;
} // namespace llvm

namespace pedigree {

template <typename UnderlyingT, typename _ = void> struct DOTUnitTraits {
  // static std::string name(const T &);
  // static std::string print(const T &);
};

//

struct DefaultDOTUnitTraits {
  template <typename T> static std::string name(const T &Unit) {
    return ToObj(Unit).getName();
  }

  template <typename T> static std::string print(const T &Unit) {
    std::string s;
    llvm::raw_string_ostream os(s);
    ToObj(Unit).print(os);

    return os.str();
  }
};

//

template <typename UnderlyingT>
struct DOTUnitTraits<
    UnderlyingT,
    typename std::enable_if_t<is_unqual_same_v<llvm::Instruction, UnderlyingT>>>
    : DefaultDOTUnitTraits {
  using Base = DefaultDOTUnitTraits;

  template <typename T> static std::string name(const T &Unit) {
    auto *term = llvm::dyn_cast<llvm::TerminatorInst>(ToPtr(Unit));

    return term ? "term@" + ToObj(Unit).getParent()->getName().str()
                : ToObj(Unit).getName().str();
  }

  using Base::print;
};

template <typename UnderlyingT>
struct DOTUnitTraits<
    UnderlyingT,
    typename std::enable_if_t<is_unqual_same_v<llvm::BasicBlock, UnderlyingT>>>
    : DefaultDOTUnitTraits {
  using Base = DefaultDOTUnitTraits;

  using Base::name;
  using Base::print;
};

template <typename UnderlyingT>
struct DOTUnitTraits<
    UnderlyingT,
    typename std::enable_if_t<is_unqual_same_v<llvm::Function, UnderlyingT>>>
    : DefaultDOTUnitTraits {
  using Base = DefaultDOTUnitTraits;

  using Base::name;
  using Base::print;
};

} // namespace pedigree

#endif // header
