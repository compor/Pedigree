//
//
//

#include "Pedigree/Config.hpp"

#include "Pedigree/Debug.hpp"

#include "Pedigree/Util.hpp"

#include "Pedigree/Analysis/Passes/Printers/Graphviz/PDGraphPrinterPass.hpp"

#include "Pedigree/Analysis/Passes/PDGraphPass.hpp"

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
// using llvm::cl::opt
// using llvm::cl::list
// using llvm::cl::desc
// using llvm::cl::Hidden

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

#define DEBUG_TYPE "pedigree-pdg-printer"

extern llvm::cl::list<std::string> PedigreeGraphDOTFunctionWhitelist;
extern llvm::cl::opt<std::string> PedigreeReportsDir;

namespace pedigree {

// new passmanager pass

llvm::PreservedAnalyses
PDGraphDOTPrinterPass::run(llvm::Function &F,
                           llvm::FunctionAnalysisManager &FAM) {
  auto graph = std::move(FAM.getResult<PDGraphAnalysis>(F));

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
        PedigreeReportsDir + "/" + "pdg" + "." + F.getName().str() + ".dot";
    std::error_code EC;

    llvm::errs() << "Writing '" << Filename << "'...";

    llvm::raw_fd_ostream File(Filename, EC, llvm::sys::fs::F_Text);
    std::string GraphName =
        llvm::DOTGraphTraits<PDGraph *>::getGraphName(graph.get());
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

struct PDGraphPrinterWrapperPass
    : public llvm::DOTGraphTraitsPrinter<
          PDGraphWrapperPass, false, PDGraph *,
          LLVMAnalysisStandardDependenceGraphPassTraitsHelperBase<
              PDGraphWrapperPass, PDGraph>> {
  using Base = llvm::DOTGraphTraitsPrinter<
      PDGraphWrapperPass, false, PDGraph *,
      LLVMAnalysisStandardDependenceGraphPassTraitsHelperBase<
          PDGraphWrapperPass, PDGraph>>;

  static char ID;

  PDGraphPrinterWrapperPass() : Base(PedigreeReportsDir + "/" + "pdg", ID) {
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

struct PDGraphSimplePrinterWrapperPass
    : public llvm::DOTGraphTraitsPrinter<
          PDGraphWrapperPass, true, PDGraph *,
          LLVMAnalysisStandardDependenceGraphPassTraitsHelperBase<
              PDGraphWrapperPass, PDGraph>> {
  using Base = llvm::DOTGraphTraitsPrinter<
      PDGraphWrapperPass, true, PDGraph *,
      LLVMAnalysisStandardDependenceGraphPassTraitsHelperBase<
          PDGraphWrapperPass, PDGraph>>;

  static char ID;

  PDGraphSimplePrinterWrapperPass()
      : Base(PedigreeReportsDir + "/" + "pdg", ID) {
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

} // namespace pedigree

char pedigree::PDGraphPrinterWrapperPass::ID = 0;
static llvm::RegisterPass<pedigree::PDGraphPrinterWrapperPass>
    X("pedigree-pdg-dot", PRJ_CMDLINE_DESC("pedigree pdg DOT pass"), false,
      false);

//

char pedigree::PDGraphSimplePrinterWrapperPass::ID = 0;
static llvm::RegisterPass<pedigree::PDGraphSimplePrinterWrapperPass>
    Y("pedigree-pdg-simple-dot",
      PRJ_CMDLINE_DESC("pedigree simple pdg DOT pass"), false, false);

// plugin registration for clang

// the solution was at the bottom of the header file
// 'llvm/Transforms/IPO/PassManagerBuilder.h'
// create a static free-floating callback that uses the legacy pass manager to
// add an instance of this pass and a static instance of the
// RegisterStandardPasses class

static void registerPedigreePDGraphPrinterWrapperPass(
    const llvm::PassManagerBuilder &Builder,
    llvm::legacy::PassManagerBase &PM) {
  PM.add(new pedigree::PDGraphPrinterWrapperPass());

  return;
}

static llvm::RegisterStandardPasses RegisterPedigreePDGraphPrinterWrapperPass(
    llvm::PassManagerBuilder::EP_EarlyAsPossible,
    registerPedigreePDGraphPrinterWrapperPass);

//

static void registerPedigreePDGraphSimplePrinterWrapperPass(
    const llvm::PassManagerBuilder &Builder,
    llvm::legacy::PassManagerBase &PM) {
  PM.add(new pedigree::PDGraphSimplePrinterWrapperPass());

  return;
}

static llvm::RegisterStandardPasses
    RegisterPedigreePDGraphSimplePrinterWrapperPass(
        llvm::PassManagerBuilder::EP_EarlyAsPossible,
        registerPedigreePDGraphSimplePrinterWrapperPass);

