//
//
//

#include "TestIRAssemblyParser.hpp"

#include "TestCommon.hpp"

#include "PostDominanceFrontier.hpp"

#include "llvm/IR/BasicBlock.h"
// using llvm::BasicBlock

#include "llvm/Analysis/PostDominators.h"
// using llvm::PostDominatorTree

#include "llvm/ADT/SmallVector.h"
// using llvm::SmallVector

#include "gtest/gtest.h"
// using testing::Test

#include <array>
// using std::array

#include <vector>
// using std::vector

#include <string>
// using std::string

#include <iostream>
// using std::ostream

namespace pedigree {
namespace testing {
namespace {

struct PDFTestData {
  PDFTestData() = delete;

  std::string assemblyFile;
  std::vector<std::string> blockNames;
};

std::ostream &operator<<(std::ostream &os, const PDFTestData &td) {
  auto delim = ' ';
  return os << delim << "assembly file: " << td.assemblyFile << delim;
}

//

class PDFConstructionTest : public TestIRAssemblyParser,
                            public ::testing::TestWithParam<PDFTestData> {};

//

TEST_P(PDFConstructionTest, PDFConstruction) {
  auto td = GetParam();

  parseAssemblyFile(td.assemblyFile);
  auto *curFunc = m_Module->getFunction("foo");
  ASSERT_FALSE(nullptr == curFunc);

  llvm::PostDominatorTree curPDT;
  curPDT.DT->recalculate(*curFunc);

  PostDominanceFrontierBase<llvm::BasicBlock> pdf;

  llvm::SmallVector<llvm::BasicBlock *, 32> traversal;
  pdf.traverseBottomUp(traversal, *curPDT.DT, curPDT.DT->getRootNode());

  // pdf.analyze(*curPDT.DT);
}

std::array<PDFTestData, 3> testData1 = {"hpc4pc_book_fig73.ll"};

INSTANTIATE_TEST_CASE_P(DefaultInstance, PDFConstructionTest,
                        ::testing::ValuesIn(testData1));

} // unnamed namespace end
} // namespace testing end
} // namespace pedigree end
