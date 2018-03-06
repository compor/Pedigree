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
      : m_Module{nullptr}, m_shouldVerify(shouldVerify),
        m_TestDataDir{dataDir} {
#if (LLVM_VERSION_MAJOR >= 4) ||                                               \
    (LLVM_VERSION_MAJOR == 3 && LLVM_VERSION_MINOR >= 9)
    llvm::LLVMContext theContext;
    m_Context = &theContext;
#else
    m_Context = &(llvm::getGlobalContext());
#endif
  }

  void parseAssemblyFile(llvm::StringRef AssemblyHolder) {
    m_Module = llvm::parseAssemblyFile((m_TestDataDir + AssemblyHolder).str(),
                                       m_Diagnostic, *m_Context);

    report();
  }

  void parseAssemblyString(llvm::StringRef AssemblyHolder) {
    m_Module =
        llvm::parseAssemblyString(AssemblyHolder, m_Diagnostic, *m_Context);

    report();
  }

protected:
  void report() {
    std::string msg;
    llvm::raw_string_ostream os(msg);
    m_Diagnostic.print("", os);

    if (m_shouldVerify && llvm::verifyModule(*m_Module, &(llvm::errs())))
      llvm::report_fatal_error("module verification failed\n");

    if (!m_Module)
      llvm::report_fatal_error(os.str().c_str());
  }

  bool m_shouldVerify;
  std::unique_ptr<llvm::Module> m_Module;
  llvm::StringRef m_TestDataDir;
  llvm::LLVMContext *m_Context;
  llvm::SMDiagnostic m_Diagnostic;
};

} // namespace testing end
} // namespace pedigre end

#endif // header
