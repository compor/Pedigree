//
//
//

#include "pedigree/Config.hpp"

#include "llvm/Support/CommandLine.h"
// using llvm::cl::opt
// using llvm::cl::desc
// using llvm::cl::Hidden

llvm::cl::opt<bool> PedigreeGraphConnectRoot(
    "pedigree-connect-root", llvm::cl::Hidden,
    llvm::cl::desc("connect virtual root with all other nodes"));
