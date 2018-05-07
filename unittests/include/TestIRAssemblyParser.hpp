//
//
//

#ifndef TESTIRASSEMBLYPARSER_HPP
#define TESTIRASSEMBLYPARSER_HPP

#include <memory>
// using std::unique_ptr

#include "llvm/Config/llvm-config.h"

#include "llvm/IR/LLVMContext.h"
// using llvm::LLVMContext

#include "llvm/IR/Module.h"
// using llvm::Module

#include "llvm/Support/SourceMgr.h"
// using llvm::SMDiagnostic

#include "llvm/AsmParser/Parser.h"
// using llvm::parseAssemblyFile
// using llvm::parseAssemblyString

#include "llvm/IR/Verifier.h"
// using llvm::verifyModule

#include "llvm/Support/ErrorHandling.h"
// using llvm::report_fatal_error

#include "llvm/Support/raw_ostream.h"
// using llvm::raw_string_ostream

#include "llvm/ADT/StringRef.h"
// using llvm::StringRef

namespace pedigree {
namespace testing {

class TestIRAssemblyParser {
public:
  TestIRAssemblyParser(bool shouldVerify = true,
                       llvm::StringRef dataDir = "./unittests/data/")
      : m_shouldVerify(shouldVerify), TestModule{nullptr},
        TestDataDir{dataDir} {
#if (LLVM_VERSION_MAJOR >= 4) ||                                               \
    (LLVM_VERSION_MAJOR == 3 && LLVM_VERSION_MINOR >= 9)
    TestContext = &theContext;
#else
    TestContext = &(llvm::getGlobalContext());
#endif
  }

  llvm::Module &module() {
    if (!TestModule)
      llvm::report_fatal_error("Module pointer is null!");

    return *TestModule;
  }

  void parseAssemblyFile(llvm::StringRef AssemblyHolder) {
    TestModule = llvm::parseAssemblyFile((TestDataDir + AssemblyHolder).str(),
                                         TestDiagnostic, *TestContext);

    report();
  }

  void parseAssemblyString(llvm::StringRef AssemblyHolder) {
    TestModule =
        llvm::parseAssemblyString(AssemblyHolder, TestDiagnostic, *TestContext);

    report();
  }

protected:
  void report() {
    std::string msg;
    llvm::raw_string_ostream os(msg);
    TestDiagnostic.print("", os);

    if (!TestModule)
      llvm::report_fatal_error(os.str().c_str());

    if (m_shouldVerify && llvm::verifyModule(*TestModule, &(llvm::errs())))
      llvm::report_fatal_error("module verification failed\n");
  }

#if (LLVM_VERSION_MAJOR >= 4) ||                                               \
    (LLVM_VERSION_MAJOR == 3 && LLVM_VERSION_MINOR >= 9)
  llvm::LLVMContext theContext;
#endif

  bool m_shouldVerify;
  std::unique_ptr<llvm::Module> TestModule;
  llvm::StringRef TestDataDir;
  llvm::LLVMContext *TestContext;
  llvm::SMDiagnostic TestDiagnostic;
};

} // namespace testing
} // namespace pedigre

#endif // header
