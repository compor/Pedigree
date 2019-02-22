//
//
//

#include "Pedigree/Config.hpp"

#include "Pedigree/Debug.hpp"

#include "Pedigree/Util.hpp"

#include "Pedigree/Analysis/Passes/Printers/Graphviz/DDGraphPrinterPass.hpp"

#include "Pedigree/Analysis/Passes/DDGraphPass.hpp"

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

#include "llvm/Support/GraphWriter.h"
// using llvm::WriteGraph

#include "llvm/Support/Debug.h"
// using LLVM_DEBUG macro
// using llvm::dbgs

#include <string>
// using std::string

#include <algorithm>
// std::find

#include <system_error>
// using std::error_code

#define DEBUG_TYPE "pedigree-ddg-printer"

extern llvm::cl::list<std::string> PedigreeGraphDOTFunctionWhitelist;
extern llvm::cl::opt<std::string> PedigreeReportsDir;

namespace pedigree {

// new passmanager pass

llvm::PreservedAnalyses
DDGraphDOTPrinterPass::run(llvm::Function &F,
                           llvm::FunctionAnalysisManager &FAM) {
  auto graph = std::move(FAM.getResult<DDGraphAnalysis>(F));

  auto dirOrErr = CreateDirectory(PedigreeReportsDir);
  if (std::error_code ec = dirOrErr.getError()) {
    llvm::errs() << "Error: " << ec.message() << '\n';
    llvm::report_fatal_error("Failed to create reports directory" +
                             PedigreeReportsDir);
  }

  PedigreeReportsDir = dirOrErr.get();

  auto found =
      std::find(std::begin(PedigreeGraphDOTFunctionWhitelist),
                std::end(PedigreeGraphDOTFunctionWhitelist), F.getName().str());

  if (PedigreeGraphDOTFunctionWhitelist.empty() ||
      std::end(PedigreeGraphDOTFunctionWhitelist) != found) {
    std::string Filename =
        PedigreeReportsDir + "/" + "ddg" + "." + F.getName().str() + ".dot";
    std::error_code EC;

    llvm::errs() << "Writing '" << Filename << "'...";

    llvm::raw_fd_ostream File(Filename, EC, llvm::sys::fs::F_Text);
    std::string GraphName =
        llvm::DOTGraphTraits<DDGraph *>::getGraphName(graph.get());
    std::string Title = GraphName + " for '" + F.getName().str() + "' function";

    if (!EC) {
      WriteGraph(File, graph.get(), false, Title);
    } else {
      llvm::errs() << "  error opening file for writing!";
    }
    llvm::errs() << "\n";
  }

  return llvm::PreservedAnalyses::all();
}

// legacy passmanager pass

struct DDGraphPrinterWrapperPass
    : public llvm::DOTGraphTraitsPrinter<
          DDGraphWrapperPass, false, DDGraph *,
          LLVMAnalysisStandardDependenceGraphPassTraitsHelperBase<
              DDGraphWrapperPass, DDGraph>> {
  using Base = llvm::DOTGraphTraitsPrinter<
      DDGraphWrapperPass, false, DDGraph *,
      LLVMAnalysisStandardDependenceGraphPassTraitsHelperBase<
          DDGraphWrapperPass, DDGraph>>;

  static char ID;

  DDGraphPrinterWrapperPass() : Base(PedigreeReportsDir + "/" + "ddg", ID) {
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

struct DDGraphSimplePrinterWrapperPass
    : public llvm::DOTGraphTraitsPrinter<
          DDGraphWrapperPass, true, DDGraph *,
          LLVMAnalysisStandardDependenceGraphPassTraitsHelperBase<
              DDGraphWrapperPass, DDGraph>> {
  using Base = llvm::DOTGraphTraitsPrinter<
      DDGraphWrapperPass, true, DDGraph *,
      LLVMAnalysisStandardDependenceGraphPassTraitsHelperBase<
          DDGraphWrapperPass, DDGraph>>;

  static char ID;

  DDGraphSimplePrinterWrapperPass()
      : Base(PedigreeReportsDir + "/" + "ddg", ID) {
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

char pedigree::DDGraphPrinterWrapperPass::ID = 0;
static llvm::RegisterPass<pedigree::DDGraphPrinterWrapperPass>
    X(PEDIGREE_DDG_DOT_PASS_NAME, PRJ_CMDLINE_DESC("pedigree ddg DOT pass"),
      false, false);

//

char pedigree::DDGraphSimplePrinterWrapperPass::ID = 0;
static llvm::RegisterPass<pedigree::DDGraphSimplePrinterWrapperPass>
    Y(PEDIGREE_DDG_DOT_PASS_NAME "-simple",
      PRJ_CMDLINE_DESC("pedigree simple ddg DOT pass"), false, false);

// plugin registration for clang

// the solution was at the bottom of the header file
// 'llvm/Transforms/IPO/PassManagerBuilder.h'
// create a static free-floating callback that uses the legacy pass manager to
// add an instance of this pass and a static instance of the
// RegisterStandardPasses class

static void registerPedigreeDDGraphPrinterWrapperPass(
    const llvm::PassManagerBuilder &Builder,
    llvm::legacy::PassManagerBase &PM) {
  PM.add(new pedigree::DDGraphPrinterWrapperPass());

  return;
}

static llvm::RegisterStandardPasses RegisterPedigreeDDGraphPrinterWrapperPass(
    llvm::PassManagerBuilder::EP_EarlyAsPossible,
    registerPedigreeDDGraphPrinterWrapperPass);

//

static void registerPedigreeDDGraphSimplePrinterWrapperPass(
    const llvm::PassManagerBuilder &Builder,
    llvm::legacy::PassManagerBase &PM) {
  PM.add(new pedigree::DDGraphSimplePrinterWrapperPass());

  return;
}

static llvm::RegisterStandardPasses
    RegisterPedigreeDDGraphSimplePrinterWrapperPass(
        llvm::PassManagerBuilder::EP_EarlyAsPossible,
        registerPedigreeDDGraphSimplePrinterWrapperPass);

