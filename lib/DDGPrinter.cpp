
#include "Utils.hpp"

#include "DataDependenceGraphPass.hpp"

#include "llvm/Analysis/DOTGraphTraitsPass.h"
// using llvm::DOTGraphTraitsPrinter

#include "llvm/Pass.h"
// using llvm::RegisterPass

#include "llvm/IR/LegacyPassManager.h"
// using llvm::PassManagerBase

#include "llvm/Transforms/IPO/PassManagerBuilder.h"
// using llvm::PassManagerBuilder
// using llvm::RegisterStandardPasses

#include "llvm/Support/CommandLine.h"
// using llvm::cl::opt

namespace llvm {

using namespace pedigree;

struct AnalysisDependenceGraphPassTraits {
  static DataDependenceGraph *getGraph(DataDependenceGraphPass *P) {
    return &P->getGraph();
  }
};

} // namespace llvm end

namespace pedigree {

template <bool Simple = false>
struct DDGPrinterCommonBase
    : public llvm::DOTGraphTraitsPrinter<
          DataDependenceGraphPass, Simple, DataDependenceGraph *,
          llvm::AnalysisDependenceGraphPassTraits> {
  DDGPrinterCommonBase(char &ID)
      : llvm::DOTGraphTraitsPrinter<DataDependenceGraphPass, Simple,
                                    DataDependenceGraph *,
                                    llvm::AnalysisDependenceGraphPassTraits>(
            "ddg", ID) {}
};

struct DDGPrinter : public DDGPrinterCommonBase<false> {
  static char ID;

  DDGPrinter() : DDGPrinterCommonBase<false>(ID) {}
};

struct DDGPrinterSimple : public DDGPrinterCommonBase<true> {
  static char ID;

  DDGPrinterSimple() : DDGPrinterCommonBase<true>(ID) {}
};

} // namespace pedigree end

char pedigree::DDGPrinter::ID = 0;
static llvm::RegisterPass<pedigree::DDGPrinter>
    X("pedigree-ddg-dot", PRJ_CMDLINE_DESC("pedigree ddg DOT pass"), false,
      false);

//

char pedigree::DDGPrinterSimple::ID = 0;
static llvm::RegisterPass<pedigree::DDGPrinterSimple>
    Y("pedigree-ddg-dot-simple",
      PRJ_CMDLINE_DESC("pedigree ddg simple DOT pass"), false, false);

// plugin registration for clang

// the solution was at the bottom of the header file
// 'llvm/Transforms/IPO/PassManagerBuilder.h'
// create a static free-floating callback that uses the legacy pass manager to
// add an instance of this pass and a static instance of the
// RegisterStandardPasses class

static void registerPedigreeDDGPrinter(const llvm::PassManagerBuilder &Builder,
                                       llvm::legacy::PassManagerBase &PM) {
  PM.add(new pedigree::DDGPrinter());

  return;
}

static llvm::RegisterStandardPasses
    RegisterPedigreeDDGPrinter(llvm::PassManagerBuilder::EP_EarlyAsPossible,
                               registerPedigreeDDGPrinter);

//

static void
registerPedigreeDDGPrinterSimple(const llvm::PassManagerBuilder &Builder,
                                 llvm::legacy::PassManagerBase &PM) {
  PM.add(new pedigree::DDGPrinterSimple());

  return;
}

static llvm::RegisterStandardPasses RegisterPedigreeDDGPrinterSimple(
    llvm::PassManagerBuilder::EP_EarlyAsPossible,
    registerPedigreeDDGPrinterSimple);
