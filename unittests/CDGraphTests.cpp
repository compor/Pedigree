//
//
//

#include "TestIRAssemblyParser.hpp"

#include "TestCommon.hpp"

#include "Analysis/CDGraph.hpp"

#include "Analysis/CDGraphBuilder.hpp"

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
  auto *curFunc = m_Module->getFunction("foo");
  ASSERT_FALSE(nullptr == curFunc);

  CDGraph cdg;
  CDGraphBuilder cdgBuilder{cdg};

  cdgBuilder.build(*curFunc);

  EXPECT_EQ(td.numVertices, cdg.numVertices());
  EXPECT_EQ(td.numEdges, cdg.numEdges());
}

TEST_P(CDGraphConstructionTest, CDGraphConvertion) {
  auto td = GetParam();

  parseAssemblyFile(td.assemblyFile);
  auto *curFunc = m_Module->getFunction("foo");
  ASSERT_FALSE(nullptr == curFunc);

  CDGraph cdg;
  CDGraphBuilder cdgBuilder{cdg};

  cdgBuilder.build(*curFunc);

  InstCDGraph cdg2;
  Convert(cdg, cdg2, BlockToInstructionUnitConverter{});

  EXPECT_EQ(td.numVertices, cdg2.numVertices());
  EXPECT_EQ(td.numEdges, cdg2.numEdges());
}

std::array<CDGraphTestData, 5> testData1 = {{{"whalebook_fig81.ll", 6, 3},
                                             {"whalebook_fig85.ll", 5, 4},
                                             {"whalebook_fig821.ll", 7, 4},
                                             {"hpc4pc_book_fig37.ll", 13, 9},
                                             {"hpc4pc_book_fig321.ll", 11, 9}}};

INSTANTIATE_TEST_CASE_P(DefaultInstance, CDGraphConstructionTest,
                        ::testing::ValuesIn(testData1));

} // unnamed namespace end
} // namespace testing end
} // namespace pedigree end
