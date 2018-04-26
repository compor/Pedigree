//
//
//

#include "TestIRAssemblyParser.hpp"

#include "TestCommon.hpp"

#include "Analysis/Graphs/CDGraph.hpp"

#include "Analysis/Creational/CDGraphBuilder.hpp"

#include "Support/GraphConverter.hpp"

#include "Support/Utils/UnitConverters.hpp"

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
  CDGraph::EdgesSizeType numEdges;
};

std::ostream &operator<<(std::ostream &os, const CDGraphTestData &td) {
  auto delim = ' ';
  return os << delim << "assembly file: " << td.assemblyFile << delim
            << "vertices num: " << td.numVertices << delim
            << "edges num: " << td.numEdges << delim;
}

//

class CDGraphConstructionTest
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
  EXPECT_EQ(td.numEdges, cdg->numEdges());
}

TEST_P(CDGraphConstructionTest, CDGraphConvertion) {
  auto td = GetParam();

  parseAssemblyFile(td.assemblyFile);
  auto *curFunc = module().getFunction("foo");
  ASSERT_FALSE(nullptr == curFunc);

  CDGraphBuilder cdgBuilder{};
  auto cdg = cdgBuilder.setUnit(*curFunc).build();

  InstCDGraph cdg2;
  Convert(*cdg, cdg2, BlockToInstructionUnitConverter{});

  EXPECT_EQ(td.numVertices, cdg2.numVertices());
  EXPECT_EQ(td.numEdges, cdg2.numEdges());
}

std::array<CDGraphTestData, 5> testData1 = {{{"whalebook_fig81.ll", 4, 3},
                                             {"whalebook_fig85.ll", 4, 4},
                                             {"whalebook_fig821.ll", 5, 4},
                                             {"hpc4pc_book_fig37.ll", 8, 9},
                                             {"hpc4pc_book_fig321.ll", 9, 9}}};

INSTANTIATE_TEST_CASE_P(DefaultInstance, CDGraphConstructionTest,
                        ::testing::ValuesIn(testData1));

} // unnamed namespace
} // namespace testing
} // namespace pedigree
