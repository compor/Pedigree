//
//
//

#include "Config.hpp"

#include "llvm/Support/CommandLine.h"
// using llvm::cl::opt
// using llvm::cl::list
// using llvm::cl::desc
// using llvm::cl::Hidden

#include <string>
// using std::string

llvm::cl::list<std::string> GraphDOTFunctionWhitelist(
    "pedigree-dot-func-wl", llvm::cl::Hidden,
    llvm::cl::desc("generate dependence DOT graph only for these functions"));
