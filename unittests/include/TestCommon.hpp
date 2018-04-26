//
//
//

#ifndef TESTCOMMON_HPP
#define TESTCOMMON_HPP

#include "llvm/Config/llvm-config.h"

#include "llvm/IR/Function.h"
// using llvm::Function

#include "llvm/IR/Dominators.h"
// using llvm::DominatorTree

#include "llvm/Analysis/LoopInfo.h"
// using llvm::LoopInfo

#ifdef BOOST_NO_EXCEPTIONS

#include <iostream>
// using std::cerr
//
#include <exception>
// using std::exception
// using std::terminate

#endif // BOOST_NO_EXCEPTIONS

#ifdef BOOST_NO_EXCEPTIONS

namespace boost {

[[noreturn]] inline void throw_exception(std::exception const &e) {
  std::cerr << e.what() << '\n';

  std::terminate();
}

} // namespace boost

#endif // BOOST_NO_EXCEPTIONS

namespace pedigree {
namespace testing {

static llvm::LoopInfo calculateLoopInfo(llvm::Function &Func) {
  llvm::DominatorTree DT;
  llvm::LoopInfo LI;

  DT.recalculate(Func);
#if (LLVM_VERSION_MAJOR >= 4) ||                                               \
    (LLVM_VERSION_MAJOR == 3 && LLVM_VERSION_MINOR >= 8)
  LI.analyze(DT);
#else
  LI.Analyze(DT);
#endif

  return LI;
}

} // namespace testing
} // namespace pedigree

#endif // header
