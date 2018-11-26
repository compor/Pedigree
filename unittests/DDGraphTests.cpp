//
//
//

#include "TestIRAssemblyParser.hpp"

#include "TestCommon.hpp"

#include "Pedigree/Analysis/Graphs/DDGraph.hpp"

#include "Pedigree/Analysis/Creational/DDGraphBuilder.hpp"

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

struct DDGraphTestData {
  std::string assemblyFile;
  DDGraph::VerticesSizeType numVertices;
  DDGraph::OutEdgesSizeType numOutEdges;
};

std::ostream &operator<<(std::ostream &os, const DDGraphTestData &td) {
  auto delim = ' ';
  return os << delim << "assembly file: " << td.assemblyFile << delim
            << "vertices num: " << td.numVertices << delim
            << "out edges num: " << td.numOutEdges << delim;
}

//

class DDGraphConstructionTest
    : public TestIRAssemblyParser,
      public ::testing::TestWithParam<DDGraphTestData> {};

//

TEST_P(DDGraphConstructionTest, DDGraphConstruction) {
  auto td = GetParam();

  parseAssemblyFile(td.assemblyFile);
  auto *curFunc = module().getFunction("foo");
  ASSERT_FALSE(nullptr == curFunc);

  DDGraphBuilder ddgBuilder{};
  auto ddg = ddgBuilder.setUnit(*curFunc).build();

  EXPECT_EQ(td.numVertices, ddg->numVertices());
  EXPECT_EQ(td.numOutEdges, ddg->numOutEdges());
}

std::array<DDGraphTestData, 3> testData1{{{"whalebook_fig81.ll", 11, 9},
                                          {"whalebook_fig85.ll", 7, 3},
                                          {"whalebook_fig821.ll", 12, 10}}};

INSTANTIATE_TEST_CASE_P(DefaultInstance, DDGraphConstructionTest,
                        ::testing::ValuesIn(testData1), );

} // unnamed namespace
} // namespace testing
} // namespace pedigree
