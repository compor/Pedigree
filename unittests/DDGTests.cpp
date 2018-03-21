//
//
//

#include "TestIRAssemblyParser.hpp"

#include "TestCommon.hpp"

#include "DDG.hpp"

#include "DDGBuilder.hpp"

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

struct DDGTestData {
  DDGTestData() = delete;

  std::string assemblyFile;
  DDG::VerticesSizeType numVertices;
  DDG::EdgesSizeType numEdges;
};

std::ostream &operator<<(std::ostream &os, const DDGTestData &td) {
  auto delim = ' ';
  return os << delim << "assembly file: " << td.assemblyFile << delim
            << "vertices num: " << td.numVertices << delim
            << "edges num: " << td.numEdges << delim;
}

//

class DDGConstructionTest : public TestIRAssemblyParser,
                            public ::testing::TestWithParam<DDGTestData> {};

//

TEST_P(DDGConstructionTest, DDGConstruction) {
  auto td = GetParam();

  parseAssemblyFile(td.assemblyFile);
  auto *curFunc = m_Module->getFunction("foo");
  ASSERT_FALSE(nullptr == curFunc);

  DDG ddg;
  DDGBuilder ddgBuilder{ddg};

  ddgBuilder.visit(*curFunc);

  EXPECT_EQ(td.numVertices, ddg.numVertices());
  EXPECT_EQ(td.numEdges, ddg.numEdges());
}

std::array<DDGTestData, 3> testData1 = {"whalebook_fig81.ll",  13, 9,
                                        "whalebook_fig85.ll",  10, 3,
                                        "whalebook_fig821.ll", 16, 10};

INSTANTIATE_TEST_CASE_P(DefaultInstance, DDGConstructionTest,
                        ::testing::ValuesIn(testData1));

} // unnamed namespace end
} // namespace testing end
} // namespace pedigree end
