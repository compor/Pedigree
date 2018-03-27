//
//
//

#include "Config.hpp"

#include "Utils.hpp"

#include "Analysis/Passes/PDGraphPass.hpp"

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
    PDGraphDOTSimple("pedigree-pdg-dot-simple", llvm::cl::Hidden,
                     llvm::cl::desc("generate simple PDGraph DOT graph"));

static llvm::cl::list<std::string> PDGraphDOTFunctionWhitelist(
    "pedigree-pdg-dot-func-wl", llvm::cl::Hidden,
    llvm::cl::desc("generate PDGraph DOT graph only for these functions"));

namespace llvm {

struct AnalysisDependenceGraphPassTraits {
  static pedigree::PDGraph *getGraph(pedigree::PDGraphPass *P) {
    return &P->getGraph();
  }
};

} // namespace llvm end

namespace pedigree {

struct PDGraphPrinterPass : public llvm::DOTGraphTraitsPrinter<
                                PDGraphPass, false, pedigree::PDGraph *,
                                llvm::AnalysisDependenceGraphPassTraits> {
  using Base =
      llvm::DOTGraphTraitsPrinter<PDGraphPass, false, pedigree::PDGraph *,
                                  llvm::AnalysisDependenceGraphPassTraits>;
  static char ID;

  PDGraphPrinterPass() : Base("pdg", ID) {}

  bool runOnFunction(llvm::Function &CurFunction) override {
    auto found = std::find(std::begin(PDGraphDOTFunctionWhitelist),
                           std::end(PDGraphDOTFunctionWhitelist),
                           CurFunction.getName().str());

    if (PDGraphDOTFunctionWhitelist.empty() ||
        std::end(PDGraphDOTFunctionWhitelist) != found)
      return Base::runOnFunction(CurFunction);
    else
      return false;
  }
};

} // namespace pedigree end

char pedigree::PDGraphPrinterPass::ID = 0;
static llvm::RegisterPass<pedigree::PDGraphPrinterPass>
    X("pedigree-pdg-dot", PRJ_CMDLINE_DESC("pedigree pdg DOT pass"), false,
      false);

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
