//
//
//

#include "Config.hpp"

#include "Utils.hpp"

#include "Analysis/Passes/PDGraphPass.hpp"

#include "Support/Traits/LLVMAnalysisGraphTraits.hpp"

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

#include <algorithm>
// std::find

extern llvm::cl::list<std::string> GraphDOTFunctionWhitelist;

namespace pedigree {

struct PDGraphPrinterPass
    : public llvm::DOTGraphTraitsPrinter<
          PDGraphPass, false, PDGraph *,
          LLVMAnalysisDependenceGraphPassTraitsHelperBase<PDGraphPass,
                                                          PDGraph>> {
  using Base = llvm::DOTGraphTraitsPrinter<
      PDGraphPass, false, PDGraph *,
      LLVMAnalysisDependenceGraphPassTraitsHelperBase<PDGraphPass, PDGraph>>;

  static char ID;

  PDGraphPrinterPass() : Base("pdg", ID) {}

  bool runOnFunction(llvm::Function &CurFunction) override {
    auto found = std::find(std::begin(GraphDOTFunctionWhitelist),
                           std::end(GraphDOTFunctionWhitelist),
                           CurFunction.getName().str());
    auto hasChanged = false;

    if (GraphDOTFunctionWhitelist.empty() ||
        std::end(GraphDOTFunctionWhitelist) != found) {
      hasChanged |= Base::runOnFunction(CurFunction);
    }

    return hasChanged;
  }
};

//

struct PDGraphSimplePrinterPass
    : public llvm::DOTGraphTraitsPrinter<
          PDGraphPass, true, PDGraph *,
          LLVMAnalysisDependenceGraphPassTraitsHelperBase<PDGraphPass,
                                                          PDGraph>> {
  using Base = llvm::DOTGraphTraitsPrinter<
      PDGraphPass, true, PDGraph *,
      LLVMAnalysisDependenceGraphPassTraitsHelperBase<PDGraphPass, PDGraph>>;

  static char ID;

  PDGraphSimplePrinterPass() : Base("pdg", ID) {}

  bool runOnFunction(llvm::Function &CurFunction) override {
    auto found = std::find(std::begin(GraphDOTFunctionWhitelist),
                           std::end(GraphDOTFunctionWhitelist),
                           CurFunction.getName().str());
    auto hasChanged = false;

    if (GraphDOTFunctionWhitelist.empty() ||
        std::end(GraphDOTFunctionWhitelist) != found) {
      hasChanged |= Base::runOnFunction(CurFunction);
    }

    return hasChanged;
  }
};

} // namespace pedigree

char pedigree::PDGraphPrinterPass::ID = 0;
static llvm::RegisterPass<pedigree::PDGraphPrinterPass>
    X("pedigree-pdg-dot", PRJ_CMDLINE_DESC("pedigree pdg DOT pass"), false,
      false);

//

char pedigree::PDGraphSimplePrinterPass::ID = 0;
static llvm::RegisterPass<pedigree::PDGraphSimplePrinterPass>
    Y("pedigree-pdg-simple-dot",
      PRJ_CMDLINE_DESC("pedigree simple pdg DOT pass"), false, false);

// plugin registration for clang

// the solution was at the bottom of the header file
// 'llvm/Transforms/IPO/PassManagerBuilder.h'
// create a static free-floating callback that uses the legacy pass manager to
// add an instance of this pass and a static instance of the
// RegisterStandardPasses class

static void
registerPedigreePDGraphPrinterPass(const llvm::PassManagerBuilder &Builder,
                                   llvm::legacy::PassManagerBase &PM) {
  PM.add(new pedigree::PDGraphPrinterPass());

  return;
}

static llvm::RegisterStandardPasses RegisterPedigreePDGraphPrinterPass(
    llvm::PassManagerBuilder::EP_EarlyAsPossible,
    registerPedigreePDGraphPrinterPass);

//

static void registerPedigreePDGraphSimplePrinterPass(
    const llvm::PassManagerBuilder &Builder,
    llvm::legacy::PassManagerBase &PM) {
  PM.add(new pedigree::PDGraphSimplePrinterPass());

  return;
}

static llvm::RegisterStandardPasses RegisterPedigreePDGraphSimplePrinterPass(
    llvm::PassManagerBuilder::EP_EarlyAsPossible,
    registerPedigreePDGraphSimplePrinterPass);
