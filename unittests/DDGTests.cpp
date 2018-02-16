//
//
//

#include "TestIRAssemblyParser.hpp"

#include "TestCommon.hpp"

#include "DataDependenceGraph.hpp"

#include "boost/graph/graph_traits.hpp"
// using boost::graph_traits

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
  std::string assemblyFile;
  boost::graph_traits<DataDependenceGraphTy>::vertices_size_type num_vertices;
};

std::ostream &operator<<(std::ostream &os, const DDGTestData &td) {
  auto delim = ' ';
  return os << delim << "assembly file: " << td.assemblyFile << delim
            << "vertices num: " << td.num_vertices << delim;
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

  DataDependenceGraphTy g{};
  CreateGraph(g, *curFunc);

  EXPECT_EQ(td.num_vertices, num_vertices(g));
}

std::array<DDGTestData, 1> testData1 = {"whalebook_fig81.ll", 13};

INSTANTIATE_TEST_CASE_P(DefaultInstance, DDGConstructionTest,
                        ::testing::ValuesIn(testData1));

} // unnamed namespace end
} // namespace testing end
} // namespace pedigree end
