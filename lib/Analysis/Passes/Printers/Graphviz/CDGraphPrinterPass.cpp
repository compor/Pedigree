//
//
//

#include "Pedigree/Config.hpp"

#include "Pedigree/Debug.hpp"

#include "Pedigree/Util.hpp"

#include "Pedigree/Analysis/Passes/CDGraphPass.hpp"

#include "Pedigree/Support/Traits/LLVMAnalysisGraphTraits.hpp"

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

#include <algorithm>
// std::find

extern llvm::cl::list<std::string> PedigreeGraphDOTFunctionWhitelist;

namespace pedigree {

struct CDGraphPrinterPass
    : public llvm::DOTGraphTraitsPrinter<
          CDGraphPass, false, CDGraph *,
          LLVMAnalysisStandardDependenceGraphPassTraitsHelperBase<CDGraphPass,
                                                                  CDGraph>> {
  using Base = llvm::DOTGraphTraitsPrinter<
      CDGraphPass, false, CDGraph *,
      LLVMAnalysisStandardDependenceGraphPassTraitsHelperBase<CDGraphPass,
                                                              CDGraph>>;

  static char ID;

  CDGraphPrinterPass() : Base("cdg", ID) {}

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
          CDGraphPass, true, CDGraph *,
          LLVMAnalysisStandardDependenceGraphPassTraitsHelperBase<CDGraphPass,
                                                                  CDGraph>> {
  using Base = llvm::DOTGraphTraitsPrinter<
      CDGraphPass, true, CDGraph *,
      LLVMAnalysisStandardDependenceGraphPassTraitsHelperBase<CDGraphPass,
                                                              CDGraph>>;

  static char ID;

  CDGraphSimplePrinterPass() : Base("cdg", ID) {}

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
