//
//
//

#include "TestCommon.hpp"

#include "GenericDependenceGraph.hpp"

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

//

struct GenericDependenceNodeTest : public ::testing::Test {
  using TestNodeTy = GenericDependenceNode<int>;
  std::array<int, 4> nodes{1, 3, 5, 7};
  std::vector<TestNodeTy> depNodes;

  void SetUp() override {
    for (auto &e : nodes)
      depNodes.emplace_back(&e);

    depNodes[0].addDependentNode(&depNodes[1], {});
    depNodes[0].addDependentNode(&depNodes[2], {});
    depNodes[0].addDependentNode(&depNodes[3], {});
  }
};

//

TEST_F(GenericDependenceNodeTest, Comparison) {
  std::vector<TestNodeTy> otherDepNodes;
  for (auto &e : nodes)
    otherDepNodes.emplace_back(&e);

  otherDepNodes[3].addDependentNode(&otherDepNodes[0], {});
  otherDepNodes[3].addDependentNode(&otherDepNodes[1], {});
  otherDepNodes[3].addDependentNode(&otherDepNodes[2], {});

  EXPECT_EQ(true, depNodes[0].compare(otherDepNodes[0]));
  EXPECT_EQ(false, depNodes[1].compare(otherDepNodes[1]));
}

} // unnamed namespace end
} // namespace testing end
} // namespace pedigree end
