//
//
//

#include "TestIRAssemblyParser.hpp"

#include "TestCommon.hpp"

#include "Analysis/CDG.hpp"

#include "Analysis/CDGBuilder.hpp"

#include "Converter.hpp"

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

struct CDGTestData {
  CDGTestData() = delete;

  std::string assemblyFile;
  CDG::VerticesSizeType numVertices;
  CDG::EdgesSizeType numEdges;
};

std::ostream &operator<<(std::ostream &os, const CDGTestData &td) {
  auto delim = ' ';
  return os << delim << "assembly file: " << td.assemblyFile << delim
            << "vertices num: " << td.numVertices << delim
            << "edges num: " << td.numEdges << delim;
}

//

class CDGConstructionTest : public TestIRAssemblyParser,
                            public ::testing::TestWithParam<CDGTestData> {};

//

TEST_P(CDGConstructionTest, CDGConstruction) {
  auto td = GetParam();

  parseAssemblyFile(td.assemblyFile);
  auto *curFunc = m_Module->getFunction("foo");
  ASSERT_FALSE(nullptr == curFunc);

  CDG cdg;
  CDGBuilder cdgBuilder{cdg};

  cdgBuilder.build(*curFunc);

  EXPECT_EQ(td.numVertices, cdg.numVertices());
  EXPECT_EQ(td.numEdges, cdg.numEdges());
}

std::array<CDGTestData, 5> testData1 = {{{"whalebook_fig81.ll", 6, 3},
                                         {"whalebook_fig85.ll", 5, 4},
                                         {"whalebook_fig821.ll", 7, 4},
                                         {"hpc4pc_book_fig37.ll", 13, 9},
                                         {"hpc4pc_book_fig321.ll", 11, 9}}};

INSTANTIATE_TEST_CASE_P(DefaultInstance, CDGConstructionTest,
                        ::testing::ValuesIn(testData1));

} // unnamed namespace end
} // namespace testing end
} // namespace pedigree end
