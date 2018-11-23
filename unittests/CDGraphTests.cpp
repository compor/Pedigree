//
//
//

#include "TestIRAssemblyParser.hpp"

#include "TestCommon.hpp"

#include "pedigree/Analysis/Graphs/CDGraph.hpp"

#include "pedigree/Analysis/Creational/CDGraphBuilder.hpp"

#include "pedigree/Support/GraphConverter.hpp"

#include "pedigree/Support/Utils/UnitConverters.hpp"

#include "llvm/IR/BasicBlock.h"
// using llvm::BasicBlock

#include "llvm/IR/Instruction.h"
// using llvm::Instruction

#include "gtest/gtest.h"
// using testing::Test

#include <array>
// using std::array

#include <string>
// using std::string

#include <iostream>
// using std::ostream

namespace pedigree {
namespace testing {
namespace {

struct CDGraphTestData {
  CDGraphTestData() = delete;

  std::string assemblyFile;
  CDGraph::VerticesSizeType numVertices;
  CDGraph::OutEdgesSizeType numOutEdges;
};

std::ostream &operator<<(std::ostream &os, const CDGraphTestData &td) {
  auto delim = ' ';
  return os << delim << "assembly file: " << td.assemblyFile << delim
            << "vertices num: " << td.numVertices << delim
            << "out edges num: " << td.numOutEdges << delim;
}

//

class CDGraphConstructionTest
    : public TestIRAssemblyParser,
      public ::testing::TestWithParam<CDGraphTestData> {};

class CDGraphConstructionTest2
    : public TestIRAssemblyParser,
      public ::testing::TestWithParam<CDGraphTestData> {};

//

TEST_P(CDGraphConstructionTest, CDGraphConstruction) {
  auto td = GetParam();

  parseAssemblyFile(td.assemblyFile);
  auto *curFunc = module().getFunction("foo");
  ASSERT_FALSE(nullptr == curFunc);

  CDGraphBuilder cdgBuilder{};
  auto cdg = cdgBuilder.setUnit(*curFunc).build();

  EXPECT_EQ(td.numVertices, cdg->numVertices());
  EXPECT_EQ(td.numOutEdges, cdg->numOutEdges());
}

TEST_P(CDGraphConstructionTest, CDGraphConvertionToTerminatorInst) {
  auto td = GetParam();

  parseAssemblyFile(td.assemblyFile);
  auto *curFunc = module().getFunction("foo");
  ASSERT_FALSE(nullptr == curFunc);

  CDGraphBuilder cdgBuilder{};
  auto cdg = cdgBuilder.setUnit(*curFunc).build();

  InstCDGraph cdg2;
  Convert(*cdg, cdg2, BlockToTerminatorUnitConverter{},
          BlockToTerminatorUnitConverter{});

  EXPECT_EQ(td.numVertices, cdg2.numVertices());
  EXPECT_EQ(td.numOutEdges, cdg2.numOutEdges());
}

TEST_P(CDGraphConstructionTest2, CDGraphConvertionToInstructions) {
  auto td = GetParam();

  parseAssemblyFile(td.assemblyFile);
  auto *curFunc = module().getFunction("foo");
  ASSERT_FALSE(nullptr == curFunc);

  CDGraphBuilder cdgBuilder{};
  auto cdg = cdgBuilder.setUnit(*curFunc).build();

  InstCDGraph cdg2;
  Convert(*cdg, cdg2, BlockToTerminatorUnitConverter{},
          BlockToInstructionsUnitConverter{});

  EXPECT_EQ(td.numVertices, cdg2.numVertices());
  EXPECT_EQ(td.numOutEdges, cdg2.numOutEdges());
}

std::array<CDGraphTestData, 5> testData1{{{"whalebook_fig81.ll", 5, 3},
                                          {"whalebook_fig85.ll", 5, 4},
                                          {"whalebook_fig821.ll", 6, 4},
                                          {"hpc4pc_book_fig37.ll", 9, 9},
                                          {"hpc4pc_book_fig321.ll", 10, 9}}};

std::array<CDGraphTestData, 5> testData2{{{"whalebook_fig81.ll", 10, 8},
                                          {"whalebook_fig85.ll", 9, 10},
                                          {"whalebook_fig821.ll", 14, 12},
                                          {"hpc4pc_book_fig37.ll", 20, 22},
                                          {"hpc4pc_book_fig321.ll", 26, 27}}};

INSTANTIATE_TEST_CASE_P(DefaultInstance, CDGraphConstructionTest,
                        ::testing::ValuesIn(testData1), );

INSTANTIATE_TEST_CASE_P(DefaultInstance2, CDGraphConstructionTest2,
                        ::testing::ValuesIn(testData2), );

} // unnamed namespace
} // namespace testing
} // namespace pedigree
