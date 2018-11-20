//
//
//

#include "Config.hpp"

#include "Debug.hpp"

#include "Util.hpp"

#include "Analysis/Passes/MDGraphPass.hpp"

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

  MDGraphPrinterPass() : Base("mdg", ID) {}

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

  MDGraphSimplePrinterPass() : Base("mdg", ID) {}

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
