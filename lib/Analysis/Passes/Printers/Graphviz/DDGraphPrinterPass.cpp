//
//
//

#include "Config.hpp"

#include "Debug.hpp"

#include "Util.hpp"

#include "Analysis/Passes/DDGraphPass.hpp"

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

extern llvm::cl::list<std::string> PedigreeGraphDOTFunctionWhitelist;

namespace pedigree {

struct DDGraphPrinterPass
    : public llvm::DOTGraphTraitsPrinter<
          DDGraphPass, false, DDGraph *,
          LLVMAnalysisDependenceGraphPassTraitsHelperBase<DDGraphPass,
                                                          DDGraph>> {
  using Base = llvm::DOTGraphTraitsPrinter<
      DDGraphPass, false, DDGraph *,
      LLVMAnalysisDependenceGraphPassTraitsHelperBase<DDGraphPass, DDGraph>>;

  static char ID;

  DDGraphPrinterPass() : Base("ddg", ID) {}

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

struct DDGraphSimplePrinterPass
    : public llvm::DOTGraphTraitsPrinter<
          DDGraphPass, true, DDGraph *,
          LLVMAnalysisDependenceGraphPassTraitsHelperBase<DDGraphPass,
                                                          DDGraph>> {
  using Base = llvm::DOTGraphTraitsPrinter<
      DDGraphPass, true, DDGraph *,
      LLVMAnalysisDependenceGraphPassTraitsHelperBase<DDGraphPass, DDGraph>>;

  static char ID;

  DDGraphSimplePrinterPass() : Base("ddg", ID) {}

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

char pedigree::DDGraphPrinterPass::ID = 0;
static llvm::RegisterPass<pedigree::DDGraphPrinterPass>
    X("pedigree-ddg-dot", PRJ_CMDLINE_DESC("pedigree ddg DOT pass"), false,
      false);

//

char pedigree::DDGraphSimplePrinterPass::ID = 0;
static llvm::RegisterPass<pedigree::DDGraphSimplePrinterPass>
    Y("pedigree-ddg-simple-dot",
      PRJ_CMDLINE_DESC("pedigree simple ddg DOT pass"), false, false);

// plugin registration for clang

// the solution was at the bottom of the header file
// 'llvm/Transforms/IPO/PassManagerBuilder.h'
// create a static free-floating callback that uses the legacy pass manager to
// add an instance of this pass and a static instance of the
// RegisterStandardPasses class

static void
registerPedigreeDDGraphPrinterPass(const llvm::PassManagerBuilder &Builder,
                                   llvm::legacy::PassManagerBase &PM) {
  PM.add(new pedigree::DDGraphPrinterPass());

  return;
}

static llvm::RegisterStandardPasses RegisterPedigreeDDGraphPrinterPass(
    llvm::PassManagerBuilder::EP_EarlyAsPossible,
    registerPedigreeDDGraphPrinterPass);

//

static void registerPedigreeDDGraphSimplePrinterPass(
    const llvm::PassManagerBuilder &Builder,
    llvm::legacy::PassManagerBase &PM) {
  PM.add(new pedigree::DDGraphSimplePrinterPass());

  return;
}

static llvm::RegisterStandardPasses RegisterPedigreeDDGraphSimplePrinterPass(
    llvm::PassManagerBuilder::EP_EarlyAsPossible,
    registerPedigreeDDGraphSimplePrinterPass);
