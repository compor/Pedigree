//
//
//

#include "Config.hpp"

#include "Utils.hpp"

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

#include <string>
// using std::string

static llvm::cl::opt<bool>
    MDGraphDOTSimple("pedigree-mdg-dot-simple", llvm::cl::Hidden,
                     llvm::cl::desc("generate simple MDGraph DOT graph"));

static llvm::cl::list<std::string> MDGraphDOTFunctionWhitelist(
    "pedigree-mdg-dot-func-wl", llvm::cl::Hidden,
    llvm::cl::desc("generate MDGraph DOT graph only for these functions"));

namespace pedigree {

struct MDGraphPrinterPass
    : public llvm::DOTGraphTraitsPrinter<
          MDGraphPass, false, MDGraph *,
          LLVMAnalysisDependenceGraphPassTraitsHelperBase<MDGraphPass,
                                                          MDGraph>> {
  using Base = llvm::DOTGraphTraitsPrinter<
      MDGraphPass, false, MDGraph *,
      LLVMAnalysisDependenceGraphPassTraitsHelperBase<MDGraphPass, MDGraph>>;

  static char ID;

  MDGraphPrinterPass() : Base("mdg", ID) {}

  bool runOnFunction(llvm::Function &CurFunction) override {
    auto found = std::find(std::begin(MDGraphDOTFunctionWhitelist),
                           std::end(MDGraphDOTFunctionWhitelist),
                           CurFunction.getName().str());

    if (MDGraphDOTFunctionWhitelist.empty() ||
        std::end(MDGraphDOTFunctionWhitelist) != found)
      return Base::runOnFunction(CurFunction);
    else
      return false;
  }
};

} // namespace pedigree end

char pedigree::MDGraphPrinterPass::ID = 0;
static llvm::RegisterPass<pedigree::MDGraphPrinterPass>
    X("pedigree-mdg-dot", PRJ_CMDLINE_DESC("pedigree mdg DOT pass"), false,
      false);

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