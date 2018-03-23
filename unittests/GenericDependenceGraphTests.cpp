//
//
//

#include "TestCommon.hpp"

#include "Support/GenericDependenceNode.hpp"

#include "Support/GenericDependenceGraph.hpp"

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

struct GenericDependenceGraphTest : public ::testing::Test {
  using TestNodeTy = GenericDependenceNode<int>;
  std::array<int, 4> TestNodes{1, 3, 5, 7};

  std::vector<TestNodeTy *> DepNodes1;
  std::vector<TestNodeTy *> DepNodes1b;
  std::vector<TestNodeTy *> DepNodes2;

  GenericDependenceGraph<TestNodeTy> G1, G1b, G2;

  void SetUp() override {
    for (auto &e : TestNodes) {
      DepNodes1.emplace_back(G1.getOrInsertNode(&e));
      DepNodes1b.emplace_back(G1b.getOrInsertNode(&e));
      DepNodes2.emplace_back(G2.getOrInsertNode(&e));
    }

    DepNodes1[0]->addDependentNode(DepNodes1[1], {});
    DepNodes1[0]->addDependentNode(DepNodes1[2], {});
    DepNodes1[0]->addDependentNode(DepNodes1[3], {});

    DepNodes1b[0]->addDependentNode(DepNodes1b[1], {});
    DepNodes1b[0]->addDependentNode(DepNodes1b[2], {});
    DepNodes1b[0]->addDependentNode(DepNodes1b[3], {});

    DepNodes2[3]->addDependentNode(DepNodes2[0], {});
    DepNodes2[3]->addDependentNode(DepNodes2[1], {});
    DepNodes2[3]->addDependentNode(DepNodes2[2], {});
  }
};

//

TEST_F(GenericDependenceGraphTest, NodeComparison) {
  EXPECT_EQ(true, DepNodes1[0]->compare(*DepNodes2[0]));
  EXPECT_EQ(false, *DepNodes1[0] == *DepNodes2[0]);
  EXPECT_EQ(false, DepNodes1[1]->compare(*DepNodes2[1]));
  EXPECT_EQ(true, *DepNodes1[1] == *DepNodes2[1]);
}

TEST_F(GenericDependenceGraphTest, GraphComparison) {
  EXPECT_EQ(true, G1.compare(G2));
  EXPECT_EQ(false, G1 == G2);
  EXPECT_EQ(false, G1.compare(G1b));
  EXPECT_EQ(true, G1 == G1b);
}

} // unnamed namespace end
} // namespace testing end
} // namespace pedigree end
