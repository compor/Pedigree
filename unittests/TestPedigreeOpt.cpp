//
//
//

#include "gtest/gtest.h"
// using testing::Test

#include <array>
// using std::array

#include "TestIRAssemblyParser.hpp"

#include "TestCommon.hpp"

namespace pedigree {
namespace testing {
namespace {

class ExampleTest : public TestIRAssemblyParser,
                    public ::testing::TestWithParam<ExampleTestData> {};

//

TEST_P(ExampleTest, Example) {
  auto td = GetParam();

  parseAssemblyFile(td.assemblyFile);
  auto LI = calculateLoopInfo(*m_Module->begin());

  EXPECT_EQ(LI.empty(), false);
}

std::array<ExampleTestData, 1> testData1 = {"test01.ll"};

INSTANTIATE_TEST_CASE_P(DefaultInstance, ExampleTest,
                        ::testing::ValuesIn(testData1));

} // unnamed namespace end
} // namespace testing end
} // namespace pedigree end
