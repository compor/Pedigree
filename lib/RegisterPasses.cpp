//
//
//

#include "Pedigree/Config.hpp"

#include "Pedigree/Util.hpp"

#include "Pedigree/Analysis/Passes/DDGraphPass.hpp"

#include "Pedigree/Analysis/Passes/CDGraphPass.hpp"

#include "Pedigree/Analysis/Passes/MDGraphPass.hpp"

#include "Pedigree/Analysis/Passes/PDGraphPass.hpp"

#include "llvm/IR/PassManager.h"
// using llvm::FunctionAnalysisManager

#include "llvm/Passes/PassBuilder.h"
// using llvm::PassBuilder

#include "llvm/Passes/PassPlugin.h"
// using llvmGetPassPluginInfo

#include "llvm/ADT/StringRef.h"
// using llvm::StringRef

#include "llvm/Support/Debug.h"
// using LLVM_DEBUG macro
// using llvm::dbgs

#define DEBUG_TYPE "pedigree-plugin-registration"

// plugin registration for opt new passmanager

namespace {

void registerDDGCallbacks(llvm::PassBuilder &PB) {
  PB.registerAnalysisRegistrationCallback(
      [](llvm::FunctionAnalysisManager &FAM) {
        LLVM_DEBUG(llvm::dbgs() << "registering analysis "
                                << PEDIGREE_DDG_PASS_NAME << "\n";);
        FAM.registerPass([]() { return pedigree::DDGraphAnalysis(); });
      });
  PB.registerPipelineParsingCallback(
      [](llvm::StringRef Name, llvm::FunctionPassManager &FPM,
         llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
        if (Name == "require<" PEDIGREE_DDG_PASS_NAME ">") {
          LLVM_DEBUG(llvm::dbgs() << "registering require analysis parser for "
                                  << PEDIGREE_DDG_PASS_NAME << "\n";);

          FPM.addPass(llvm::RequireAnalysisPass<pedigree::DDGraphAnalysis,
                                                llvm::Function>());
          return true;
        }
        if (Name == "invalidate<" PEDIGREE_DDG_PASS_NAME ">") {
          LLVM_DEBUG(llvm::dbgs()
                         << "registering invalidate analysis parser for "
                         << PEDIGREE_DDG_PASS_NAME << "\n";);

          FPM.addPass(
              llvm::InvalidateAnalysisPass<pedigree::DDGraphAnalysis>());
          return true;
        }
        return false;
      });
}

void registerCDGCallbacks(llvm::PassBuilder &PB) {
  PB.registerAnalysisRegistrationCallback(
      [](llvm::FunctionAnalysisManager &FAM) {
        LLVM_DEBUG(llvm::dbgs() << "registering analysis "
                                << PEDIGREE_CDG_PASS_NAME << "\n";);
        FAM.registerPass([]() { return pedigree::CDGraphAnalysis(); });
      });
  PB.registerPipelineParsingCallback(
      [](llvm::StringRef Name, llvm::FunctionPassManager &FPM,
         llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
        if (Name == "require<" PEDIGREE_CDG_PASS_NAME ">") {
          LLVM_DEBUG(llvm::dbgs() << "registering require analysis parser for "
                                  << PEDIGREE_CDG_PASS_NAME << "\n";);

          FPM.addPass(llvm::RequireAnalysisPass<pedigree::CDGraphAnalysis,
                                                llvm::Function>());
          return true;
        }
        if (Name == "invalidate<" PEDIGREE_CDG_PASS_NAME ">") {
          LLVM_DEBUG(llvm::dbgs()
                         << "registering invalidate analysis parser for "
                         << PEDIGREE_CDG_PASS_NAME << "\n";);

          FPM.addPass(
              llvm::InvalidateAnalysisPass<pedigree::CDGraphAnalysis>());
          return true;
        }
        return false;
      });
}

void registerMDGCallbacks(llvm::PassBuilder &PB) {
  PB.registerAnalysisRegistrationCallback(
      [](llvm::FunctionAnalysisManager &FAM) {
        LLVM_DEBUG(llvm::dbgs() << "registering analysis "
                                << PEDIGREE_MDG_PASS_NAME << "\n";);
        FAM.registerPass([]() { return pedigree::MDGraphAnalysis(); });
      });
  PB.registerPipelineParsingCallback(
      [](llvm::StringRef Name, llvm::FunctionPassManager &FPM,
         llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
        if (Name == "require<" PEDIGREE_MDG_PASS_NAME ">") {
          LLVM_DEBUG(llvm::dbgs() << "registering require analysis parser for "
                                  << PEDIGREE_MDG_PASS_NAME << "\n";);

          FPM.addPass(llvm::RequireAnalysisPass<pedigree::MDGraphAnalysis,
                                                llvm::Function>());
          return true;
        }
        if (Name == "invalidate<" PEDIGREE_MDG_PASS_NAME ">") {
          LLVM_DEBUG(llvm::dbgs()
                         << "registering invalidate analysis parser for "
                         << PEDIGREE_MDG_PASS_NAME << "\n";);

          FPM.addPass(
              llvm::InvalidateAnalysisPass<pedigree::MDGraphAnalysis>());
          return true;
        }
        return false;
      });
}

void registerPDGCallbacks(llvm::PassBuilder &PB) {
  PB.registerAnalysisRegistrationCallback(
      [](llvm::FunctionAnalysisManager &FAM) {
        LLVM_DEBUG(llvm::dbgs() << "registering analysis "
                                << PEDIGREE_PDG_PASS_NAME << "\n";);
        FAM.registerPass([]() { return pedigree::PDGraphAnalysis(); });
      });
  PB.registerPipelineParsingCallback(
      [](llvm::StringRef Name, llvm::FunctionPassManager &FPM,
         llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
        if (Name == "require<" PEDIGREE_PDG_PASS_NAME ">") {
          LLVM_DEBUG(llvm::dbgs() << "registering require analysis parser for "
                                  << PEDIGREE_PDG_PASS_NAME << "\n";);

          FPM.addPass(llvm::RequireAnalysisPass<pedigree::PDGraphAnalysis,
                                                llvm::Function>());
          return true;
        }
        if (Name == "invalidate<" PEDIGREE_PDG_PASS_NAME ">") {
          LLVM_DEBUG(llvm::dbgs()
                         << "registering invalidate analysis parser for "
                         << PEDIGREE_PDG_PASS_NAME << "\n";);

          FPM.addPass(
              llvm::InvalidateAnalysisPass<pedigree::PDGraphAnalysis>());
          return true;
        }
        return false;
      });
}

void registerCallbacks(llvm::PassBuilder &PB) {
  registerDDGCallbacks(PB);
  registerCDGCallbacks(PB);
  registerMDGCallbacks(PB);
  registerPDGCallbacks(PB);
}

} // namespace

extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "PedigreePlugin", STRINGIFY(VERSION_STRING),
          registerCallbacks};
}

