//
//
//

#include "Pedigree/Config.hpp"

#include "llvm/Support/CommandLine.h"
// using llvm::cl::opt
// using llvm::cl::desc
// using llvm::cl::Hidden

#include <string>
// using std::string

llvm::cl::opt<bool> PedigreeGraphConnectRoot(
    "pedigree-connect-root", llvm::cl::Hidden,
    llvm::cl::desc("connect virtual root with all other nodes"));

llvm::cl::opt<std::string>
    PedigreeReportsDir("pedigree-reports-dir", llvm::cl::init("."),
                       llvm::cl::desc("output reports directory"));
