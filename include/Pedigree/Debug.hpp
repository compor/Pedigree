//
//
//

#ifndef PEDIGREE_UTILS_HPP
#define PEDIGREE_UTILS_HPP

#include "Pedigree/Config.hpp"

#include "Pedigree/Util.hpp"

#define DEFINE_DEBUG_LEVELS                                                    \
  enum class LogLevel { Info, Notice, Warning, Error, Debug }

#ifdef BOOST_NO_EXCEPTIONS

#include <iostream>
// using std::cerr

#include <exception>
// using std::exception
// using std::terminate

namespace boost {

[[noreturn]] inline void throw_exception(std::exception const &e) {
  std::cerr << e.what() << '\n';

  std::terminate();
}

} // namespace boost

#endif // BOOST_NO_EXCEPTIONS

//

#if PEDIGREE_DEBUG

#include "llvm/IR/Function.h"
// using llvm::Function

#include "llvm/Support/FileSystem.h"
// using llvm::sys::fs::OpenFlags

#include "llvm/Support/raw_ostream.h"
// using llvm::errs
// using llvm::raw_fd_ostream

#include <cstdio>
// using std::tmpnam

#include <system_error>
// using std::error_code

DEFINE_DEBUG_LEVELS;

namespace pedigree {
namespace debug {

extern bool passDebugFlag;
extern LogLevel passLogLevel;

} // namespace debug
} // namespace pedigree

#define DEBUG_MSG(L, STR)                                                      \
  do {                                                                         \
    if (pedigree::debug::passDebugFlag && L <= pedigree::debug::passLogLevel)  \
      llvm::errs() << STR;                                                     \
  } while (false)

#define DEBUG_CMD(L, C)                                                        \
  do {                                                                         \
    if (pedigree::debug::passDebugFlag && L <= pedigree::debug::passLogLevel)  \
      C;                                                                       \
  } while (false)

namespace pedigree {
namespace debug {

static bool dumpFunction(const llvm::Function *CurFunc = nullptr) {
  if (!CurFunc)
    return false;

  std::error_code ec;
  char filename[L_tmpnam];
  std::tmpnam(filename);

  llvm::raw_fd_ostream dbgFile(filename, ec, llvm::sys::fs::F_Text);

  llvm::errs() << "\nfunction dumped at: " << filename << "\n";
  CurFunc->print(dbgFile);

  return false;
}

} // namespace debug
} // namespace pedigree

#else

#define DEBUG_MSG(L, S)                                                        \
  do {                                                                         \
  } while (false)

#define DEBUG_CMD(L, C)                                                        \
  do {                                                                         \
  } while (false)

namespace llvm {
class Function;
} // namespace llvm

DEFINE_DEBUG_LEVELS;

namespace pedigree {
namespace debug {

static constexpr bool dumpFunction(const llvm::Function *CurFunc = nullptr) {
  return true;
}

} // namespace debug
} // namespace pedigree

#endif // PEDIGREE_DEBUG

#endif // header
