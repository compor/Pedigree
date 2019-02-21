//
//
//

#include "Pedigree/Config.hpp"

#include "Pedigree/Debug.hpp"

#include "Pedigree/Util.hpp"

#include "Pedigree/Analysis/Passes/CDGraphPass.hpp"

#include "Pedigree/Support/Traits/LLVMAnalysisGraphTraits.hpp"

#include "Pedigree/Support/FileSystem.hpp"

#include "llvm/Pass.h"
// using llvm::RegisterPass

#include "llvm/IR/LegacyPassManager.h"
// using llvm::PassManagerBase

#include "llvm/Transforms/IPO/PassManagerBuilder.h"
// using llvm::PassManagerBuilder
// using llvm::RegisterStandardPasses

#include "llvm/Analysis/DOTGraphTraitsPass.h"
// using llvm::DOTGraphTraitsPrinter

#include "llvm/Support/CommandLine.h"
// using llvm::cl::list

#include "llvm/Support/ErrorHandling.h"
// using llvm::report_fatal_error

#include "llvm/Support/Debug.h"
// using LLVM_DEBUG macro
// using llvm::dbgs

#include <string>
// using std::string

#include <algorithm>
// std::find

#include <system_error>
// using std::error_code

#define DEBUG_TYPE "pedigree-cdg-printer"

extern llvm::cl::list<std::string> PedigreeGraphDOTFunctionWhitelist;
extern llvm::cl::opt<std::string> PedigreeReportsDir;

namespace pedigree {

struct CDGraphPrinterPass
    : public llvm::DOTGraphTraitsPrinter<
          CDGraphWrapperPass, false, CDGraph *,
          LLVMAnalysisStandardDependenceGraphPassTraitsHelperBase<
              CDGraphWrapperPass, CDGraph>> {
  using Base = llvm::DOTGraphTraitsPrinter<
      CDGraphWrapperPass, false, CDGraph *,
      LLVMAnalysisStandardDependenceGraphPassTraitsHelperBase<
          CDGraphWrapperPass, CDGraph>>;

  static char ID;

  CDGraphPrinterPass() : Base(PedigreeReportsDir + "/" + "cdg", ID) {
    auto dirOrErr = CreateDirectory(PedigreeReportsDir);
    if (std::error_code ec = dirOrErr.getError()) {
      llvm::errs() << "Error: " << ec.message() << '\n';
      llvm::report_fatal_error("Failed to create reports directory" +
                               PedigreeReportsDir);
    }

    PedigreeReportsDir = dirOrErr.get();
  }

  bool runOnFunction(llvm::Function &CurFunction) override {
    auto found = std::find(std::begin(PedigreeGraphDOTFunctionWhitelist),
                           std::end(PedigreeGraphDOTFunctionWhitelist),
                           CurFunction.getName().str());
    auto hasChanged = false;

    if (PedigreeGraphDOTFunctionWhitelist.empty() ||
        std::end(PedigreeGraphDOTFunctionWhitelist) != found) {
      hasChanged |= Base::runOnFunction(CurFunction);
    }

    return hasChanged;
  }
};

//

struct CDGraphSimplePrinterPass
    : public llvm::DOTGraphTraitsPrinter<
          CDGraphWrapperPass, true, CDGraph *,
          LLVMAnalysisStandardDependenceGraphPassTraitsHelperBase<
              CDGraphWrapperPass, CDGraph>> {
  using Base = llvm::DOTGraphTraitsPrinter<
      CDGraphWrapperPass, true, CDGraph *,
      LLVMAnalysisStandardDependenceGraphPassTraitsHelperBase<
          CDGraphWrapperPass, CDGraph>>;

  static char ID;

  CDGraphSimplePrinterPass() : Base(PedigreeReportsDir + "/" + "cdg", ID) {
    auto dirOrErr = CreateDirectory(PedigreeReportsDir);
    if (std::error_code ec = dirOrErr.getError()) {
      llvm::errs() << "Error: " << ec.message() << '\n';
      llvm::report_fatal_error("Failed to create reports directory" +
                               PedigreeReportsDir);
    }

    PedigreeReportsDir = dirOrErr.get();
  }

  bool runOnFunction(llvm::Function &CurFunction) override {
    auto found = std::find(std::begin(PedigreeGraphDOTFunctionWhitelist),
                           std::end(PedigreeGraphDOTFunctionWhitelist),
                           CurFunction.getName().str());
    auto hasChanged = false;

    if (PedigreeGraphDOTFunctionWhitelist.empty() ||
        std::end(PedigreeGraphDOTFunctionWhitelist) != found) {
      hasChanged = Base::runOnFunction(CurFunction);
    }

    return hasChanged;
  }
};

} // namespace pedigree

char pedigree::CDGraphPrinterPass::ID = 0;
static llvm::RegisterPass<pedigree::CDGraphPrinterPass>
    X("pedigree-cdg-dot", PRJ_CMDLINE_DESC("pedigree cdg DOT pass"), false,
      false);

//

char pedigree::CDGraphSimplePrinterPass::ID = 0;
static llvm::RegisterPass<pedigree::CDGraphSimplePrinterPass>
    Y("pedigree-cdg-simple-dot",
      PRJ_CMDLINE_DESC("pedigree simple cdg DOT pass"), false, false);

// plugin registration for clang

// the solution was at the bottom of the header file
// 'llvm/Transforms/IPO/PassManagerBuilder.h'
// create a static free-floating callback that uses the legacy pass manager to
// add an instance of this pass and a static instance of the
// RegisterStandardPasses class

static void
registerPedigreeCDGraphPrinterPass(const llvm::PassManagerBuilder &Builder,
                                   llvm::legacy::PassManagerBase &PM) {
  PM.add(new pedigree::CDGraphPrinterPass());

  return;
}

static llvm::RegisterStandardPasses RegisterPedigreeCDGraphPrinterPass(
    llvm::PassManagerBuilder::EP_EarlyAsPossible,
    registerPedigreeCDGraphPrinterPass);

//

static void registerPedigreeCDGraphSimplePrinterPass(
    const llvm::PassManagerBuilder &Builder,
    llvm::legacy::PassManagerBase &PM) {
  PM.add(new pedigree::CDGraphSimplePrinterPass());

  return;
}

static llvm::RegisterStandardPasses RegisterPedigreeCDGraphSimplePrinterPass(
    llvm::PassManagerBuilder::EP_EarlyAsPossible,
    registerPedigreeCDGraphSimplePrinterPass);
