//
//
//

#include "TestIRAssemblyParser.hpp"

#include "TestCommon.hpp"

#include "Pedigree/Analysis/PostDominanceFrontier.hpp"

#include "llvm/Config/llvm-config.h"
// version macros

#include "llvm/IR/BasicBlock.h"
// using llvm::BasicBlock

#include "llvm/Analysis/PostDominators.h"
// using llvm::PostDominatorTree

#include "gtest/gtest.h"
// using testing::Test

#include <array>
// using std::array

#include <set>
// using std::set

#include <algorithm>
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

struct PDFConstructionTestData {
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
  for (const auto &e : td.frontier) {
    ss << e << delim;
  }

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
  auto *curFunc = module().getFunction("foo");
  ASSERT_FALSE(nullptr == curFunc);

  llvm::PostDominatorTree curPDT;
  PostDominanceFrontierBase<llvm::BasicBlock> pdf;

#if (LLVM_VERSION_MAJOR >= 5)
  curPDT.recalculate(*curFunc);
  pdf.analyze(curPDT);
#else
  curPDT.DT->recalculate(*curFunc);
  pdf.analyze(*curPDT.DT);
#endif

  auto found =
      std::find_if(std::begin(*curFunc), std::end(*curFunc),
                   [&td](const auto &e) { return e.getName() == td.node; });
  ASSERT_FALSE(found == std::end(*curFunc));

  auto frontier = pdf.find(&*found);
  ASSERT_FALSE(frontier == std::end(pdf));

  std::set<std::string> frontierNames;
  for (const auto &e : frontier->second) {
    frontierNames.insert(e->getName().str());
  }

  EXPECT_EQ(td.frontier, frontierNames);
}

std::array<PDFConstructionTestData, 5> testData1{
    {{"hpc4pc_book_fig37.ll", "j_label", {}},
     {"hpc4pc_book_fig37.ll", "i_label", {}},
     {"hpc4pc_book_fig37.ll", "if.then", {"while.body.3"}},
     {"hpc4pc_book_fig37.ll", "while.cond", {"while.cond"}},
     {"hpc4pc_book_fig37.ll", "while.cond.1", {"while.cond.1", "while.cond"}}}};

std::array<PDFConstructionTestData, 1> testData2{
    {{"exit.ll", "if.then", {"entry"}}}};

std::array<PDFConstructionTestData, 1> testData3{{{"single.ll", "entry", {}}}};

std::array<PDFConstructionTestData, 5> testData4{
    {{"unreachable_return.ll", "entry", {}},
     {"unreachable_return.ll", "for.cond", {"for.cond"}},
     {"unreachable_return.ll", "for.body", {"for.cond"}},
     {"unreachable_return.ll", "for.inc", {"for.cond"}},
     {"unreachable_return.ll", "for.end", {}}}};

INSTANTIATE_TEST_SUITE_P(TestInstance1, PDFConstructionTest,
                         ::testing::ValuesIn(testData1));

INSTANTIATE_TEST_SUITE_P(TestInstance2, PDFConstructionTest,
                         ::testing::ValuesIn(testData2));

INSTANTIATE_TEST_SUITE_P(TestInstance3, PDFConstructionTest,
                         ::testing::ValuesIn(testData3));

INSTANTIATE_TEST_SUITE_P(TestInstance4, PDFConstructionTest,
                         ::testing::ValuesIn(testData4));

} // unnamed namespace
} // namespace testing
} // namespace pedigree
