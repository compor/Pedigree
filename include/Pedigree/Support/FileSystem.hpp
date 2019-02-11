//
//
//

#include "llvm/Support/ErrorOr.h"
// using llvm::ErrorOr

#include <string>
// using std::string

#pragma once

namespace llvm {
class Twine;
} // namespace llvm

namespace pedigree {

llvm::ErrorOr<std::string> CreateDirectory(const llvm::Twine &Path);

} // namespace pedigree

