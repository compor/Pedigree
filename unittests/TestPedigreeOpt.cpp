//
//
//

#include "gtest/gtest.h"
// using testing::Test

#include <array>
// using std::array

#include "TestIRAssemblyParser.hpp"

#include "TestCommon.hpp"

#include <string>
// using std::string

#include <iostream>
// using std::ostream

namespace pedigree {
namespace testing {
namespace {

struct ExampleTestData {
  std::string assemblyFile;
};

std::ostream &operator<<(std::ostream &os, const ExampleTestData &td) {
  auto delim = ' ';
  return os << delim << "assembly file: " << td.assemblyFile << delim;
}

class ExampleTest : public TestIRAssemblyParser,
                    public ::testing::TestWithParam<ExampleTestData> {};

//

TEST_P(ExampleTest, Example) {
  auto td = GetParam();

  parseAssemblyFile(td.assemblyFile);
  auto LI = calculateLoopInfo(*module().begin());

  EXPECT_EQ(LI.empty(), false);
}

std::array<ExampleTestData, 1> testData1 = {"test01.ll"};

INSTANTIATE_TEST_CASE_P(DefaultInstance, ExampleTest,
                        ::testing::ValuesIn(testData1));

} // unnamed namespace
} // namespace testing
} // namespace pedigree
