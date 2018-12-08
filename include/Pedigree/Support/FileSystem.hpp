//
//
//

#include "llvm/Support/ErrorOr.h"
// using llvm::ErrorOr

#include <string>
// using std::string

#ifndef PEDIGREE_FILESYSTEM_HPP
#define PEDIGREE_FILESYSTEM_HPP

namespace llvm {
class Twine;
} // namespace llvm

namespace pedigree {

llvm::ErrorOr<std::string> CreateDirectory(const llvm::Twine &Path);

} // namespace pedigree

#endif // header
