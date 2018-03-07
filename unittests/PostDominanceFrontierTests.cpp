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

#include <set>
// using std::set

#include <algorithm>
// using std::for_each
// using std::find_if

#include <string>
// using std::string

#include <sstream>
// using std::stringstream

#include <iostream>
// using std::ostream

namespace pedigree {
namespace testing {
namespace {

struct PDFTraversalTestData {
  PDFTraversalTestData() = delete;

  std::string assemblyFile;
  std::vector<std::string> traversalOrder;
};

std::ostream &operator<<(std::ostream &os, const PDFTraversalTestData &td) {
  auto delim = ' ';
  std::stringstream ss;

  ss << delim << "assembly file: " << td.assemblyFile << delim;

  ss << "block names: ";
  for (const auto &e : td.traversalOrder)
    ss << e << delim;

  return os << ss.str();
}

//

struct PDFTraversalTest
    : public TestIRAssemblyParser,
      public ::testing::TestWithParam<PDFTraversalTestData> {};

//

TEST_P(PDFTraversalTest, DFSPostOrderTraversal) {
  auto td = GetParam();

  parseAssemblyFile(td.assemblyFile);
  auto *curFunc = m_Module->getFunction("foo");
  ASSERT_FALSE(nullptr == curFunc);

  llvm::PostDominatorTree curPDT;
  curPDT.DT->recalculate(*curFunc);

  PostDominanceFrontierBase<llvm::BasicBlock> pdf;

  llvm::SmallVector<llvm::BasicBlock *, 32> traversal;
  pdf.traverseDFSPostOrder(traversal, *curPDT.DT, curPDT.DT->getRootNode());

  decltype(td.traversalOrder) traversalOrder;
  std::for_each(traversal.begin(), traversal.end(),
                [&traversalOrder](const auto &e) {
                  traversalOrder.push_back(e->getName().str());
                });

  EXPECT_EQ(traversalOrder, td.traversalOrder);
}

std::array<PDFTraversalTestData, 1> testData1 = {
    "hpc4pc_book_fig37.ll",
    {"entry", "b_label", "while.body", "while.body.3", "if.then", "if.else",
     "if.end", "while.cond.1", "while.end", "while.cond", "while.end.8",
     "i_label", "j_label"}};

INSTANTIATE_TEST_CASE_P(DefaultInstance, PDFTraversalTest,
                        ::testing::ValuesIn(testData1));

//
//
//

struct PDFConstructionTestData {
  PDFConstructionTestData() = delete;

  std::string assemblyFile;
  std::string node;
  std::set<std::string> frontier;
};

std::ostream &operator<<(std::ostream &os, const PDFConstructionTestData &td) {
  auto delim = ' ';
  std::stringstream ss;

  ss << delim << "assembly file: " << td.assemblyFile << delim
     << "node: " << td.node << delim;

  ss << "frontier: ";
  for (const auto &e : td.frontier)
    ss << e << delim;

  return os << ss.str();
}

//

struct PDFConstructionTest
    : public TestIRAssemblyParser,
      public ::testing::TestWithParam<PDFConstructionTestData> {};

//

TEST_P(PDFConstructionTest, PDFConstruction) {
  auto td = GetParam();

  parseAssemblyFile(td.assemblyFile);
  auto *curFunc = m_Module->getFunction("foo");
  ASSERT_FALSE(nullptr == curFunc);

  llvm::PostDominatorTree curPDT;
  curPDT.DT->recalculate(*curFunc);

  PostDominanceFrontierBase<llvm::BasicBlock> pdf;
  pdf.analyze(*(curPDT.DT));

  auto found =
      std::find_if(std::begin(*curFunc), std::end(*curFunc),
                   [&td](const auto &e) { return e.getName() == td.node; });
  ASSERT_FALSE(found == std::end(*curFunc));

  auto frontier = pdf.find(&*found);
  ASSERT_FALSE(frontier == std::end(pdf));

  std::set<std::string> frontierNames;
  for (const auto &e : frontier->second)
    frontierNames.insert(e->getName().str());

  EXPECT_EQ(frontierNames, td.frontier);
}

std::array<PDFConstructionTestData, 1> testData2 = {
    "hpc4pc_book_fig37.ll", "j_label", {}};

INSTANTIATE_TEST_CASE_P(DefaultInstance, PDFConstructionTest,
                        ::testing::ValuesIn(testData2));

} // unnamed namespace end
} // namespace testing end
} // namespace pedigree end
