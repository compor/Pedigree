//
//
//

#include "Pedigree/Config.hpp"

#include "Pedigree/Debug.hpp"

#include "Pedigree/Util.hpp"

#include "Pedigree/Analysis/Passes/MDGraphPass.hpp"

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

#include "llvm/Support/raw_ostream.h"
// using llvm::raw_string_ostream

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

#define DEBUG_TYPE "pedigree-mdg-printer"

extern llvm::cl::list<std::string> PedigreeGraphDOTFunctionWhitelist;
extern llvm::cl::opt<std::string> PedigreeReportsDir;

namespace pedigree {

struct MDGraphPrinterPass
    : public llvm::DOTGraphTraitsPrinter<
          MDGraphPass, false, MDGraph *,
          LLVMAnalysisStandardDependenceGraphPassTraitsHelperBase<MDGraphPass,
                                                                  MDGraph>> {
  using Base = llvm::DOTGraphTraitsPrinter<
      MDGraphPass, false, MDGraph *,
      LLVMAnalysisStandardDependenceGraphPassTraitsHelperBase<MDGraphPass,
                                                              MDGraph>>;

  static char ID;

  MDGraphPrinterPass() : Base(PedigreeReportsDir + "/" + "mdg", ID) {
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

struct MDGraphSimplePrinterPass
    : public llvm::DOTGraphTraitsPrinter<
          MDGraphPass, true, MDGraph *,
          LLVMAnalysisStandardDependenceGraphPassTraitsHelperBase<MDGraphPass,
                                                                  MDGraph>> {
  using Base = llvm::DOTGraphTraitsPrinter<
      MDGraphPass, true, MDGraph *,
      LLVMAnalysisStandardDependenceGraphPassTraitsHelperBase<MDGraphPass,
                                                              MDGraph>>;

  static char ID;

  MDGraphSimplePrinterPass() : Base(PedigreeReportsDir + "/" + "mdg", ID) {
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

char pedigree::MDGraphPrinterPass::ID = 0;
static llvm::RegisterPass<pedigree::MDGraphPrinterPass>
    X("pedigree-mdg-dot", PRJ_CMDLINE_DESC("pedigree mdg DOT pass"), false,
      false);

//

char pedigree::MDGraphSimplePrinterPass::ID = 0;
static llvm::RegisterPass<pedigree::MDGraphSimplePrinterPass>
    Y("pedigree-mdg-simple-dot",
      PRJ_CMDLINE_DESC("pedigree simple mdg DOT pass"), false, false);

// plugin registration for clang

// the solution was at the bottom of the header file
// 'llvm/Transforms/IPO/PassManagerBuilder.h'
// create a static free-floating callback that uses the legacy pass manager to
// add an instance of this pass and a static instance of the
// RegisterStandardPasses class

static void
registerPedigreeMDGraphPrinterPass(const llvm::PassManagerBuilder &Builder,
                                   llvm::legacy::PassManagerBase &PM) {
  PM.add(new pedigree::MDGraphPrinterPass());

  return;
}

static llvm::RegisterStandardPasses RegisterPedigreeMDGraphPrinterPass(
    llvm::PassManagerBuilder::EP_EarlyAsPossible,
    registerPedigreeMDGraphPrinterPass);

//

static void registerPedigreeMDGraphSimplePrinterPass(
    const llvm::PassManagerBuilder &Builder,
    llvm::legacy::PassManagerBase &PM) {
  PM.add(new pedigree::MDGraphSimplePrinterPass());

  return;
}

static llvm::RegisterStandardPasses RegisterPedigreeMDGraphSimplePrinterPass(
    llvm::PassManagerBuilder::EP_EarlyAsPossible,
    registerPedigreeMDGraphSimplePrinterPass);
