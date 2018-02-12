//
//
//

#ifndef UTILS_HPP
#define UTILS_HPP

#include "Config.hpp"

enum class LogLevel { info, notice, warning, error, debug };

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

namespace pedigree {
namespace utility {

extern bool passDebugFlag;
extern LogLevel passLogLevel;

} // namespace utility end
} // namespace pedigree end

#define DEBUG_MSG(L, STR)                                                      \
  do {                                                                         \
    if (pedigree::utility::passDebugFlag &&                                    \
        L <= pedigree::utility::passLogLevel)                                  \
      llvm::errs() << STR;                                                     \
  } while (false)

#define DEBUG_CMD(L, C)                                                        \
  do {                                                                         \
    if (pedigree::utility::passDebugFlag &&                                    \
        L <= pedigree::utility::passLogLevel)                                  \
      C;                                                                       \
  } while (false)

namespace pedigree {
namespace utility {

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

} // namespace utility end
} // namespace pedigree end

#else

#define DEBUG_MSG(L, S)                                                        \
  do {                                                                         \
  } while (false)

#define DEBUG_CMD(L, C)                                                        \
  do {                                                                         \
  } while (false)

namespace llvm {
class Function;
} // namespace llvm end

namespace pedigree {
namespace utility {

static constexpr bool dumpFunction(const llvm::Function *CurFunc = nullptr) {
  return true;
}

} // namespace utility end
} // namespace pedigree end

#endif // PEDIGREE_DEBUG

#endif // header
