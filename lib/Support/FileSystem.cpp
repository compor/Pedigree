//
//
//

#include "llvm/ADT/SmallString.h"
// using llvm::SmallString

#include "llvm/ADT/Twine.h"
// using llvm::Twine

#include "llvm/Support/FileSystem.h"
// using llvm::sys::fs::exists
// using llvm::sys::fs::is_directory
// using llvm::sys::fs::make_absolute
// using llvm::sys::fs::create_directories

#include "llvm/Support/Path.h"
// using llvm::sys::path::is_absolute

#include "llvm/Support/ErrorOr.h"
// using llvm::ErrorOr

#include <string>
// using std::string

#include <system_error>
// using std::error_code
// using std::make_error_code
// using std::ec

namespace pedigree {

llvm::ErrorOr<std::string> CreateDirectory(const llvm::Twine &Path) {
  llvm::SmallString<128> AbsolutePath;
  Path.toVector(AbsolutePath);

  if (!llvm::sys::path::is_absolute(AbsolutePath)) {
    if (std::error_code ec = llvm::sys::fs::make_absolute(AbsolutePath)) {
      return ec;
    }
  }

  if (llvm::sys::fs::exists(AbsolutePath) &&
      !llvm::sys::fs::is_directory(AbsolutePath)) {
    return std::make_error_code(std::errc::file_exists);
  }

  if (std::error_code ec = llvm::sys::fs::create_directories(AbsolutePath)) {
    return ec;
  }

  return AbsolutePath.str();
}

} // namespace pedigree
