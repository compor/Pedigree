//
//
//

#include "TestCommon.hpp"

#include "Pedigree/Support/GenericDependenceNode.hpp"

#include "Pedigree/Support/GenericDependenceGraph.hpp"

#include "Pedigree/Support/Traits/LLVMGraphTraitsHelperBase.hpp"

#include "llvm/ADT/GraphTraits.h"

#include "gtest/gtest.h"
// using testing::Test

#include <array>
// using std::array

#include <iterator>
// using std::distance

namespace pedigree {
namespace testing {
namespace {

using TestNodeTy = GenericDependenceNode<int>;
using TestGraphTy = GenericDependenceGraph<TestNodeTy>;

} // namespace
} // namespace testing
} // namespace pedigree

namespace llvm {

template <>
struct GraphTraits<pedigree::testing::TestNodeTy *>
    : public pedigree::LLVMDependenceGraphNodeTraitsHelperBase<
          pedigree::testing::TestNodeTy *> {};

template <>
struct GraphTraits<pedigree::testing::TestGraphTy *>
    : public pedigree::LLVMDependenceGraphTraitsHelperBase<
          pedigree::testing::TestGraphTy *> {};

} // namespace llvm

//

namespace pedigree {
namespace testing {
namespace {

struct GenericDependenceGraphTest : public ::testing::Test {
  std::array<int, 4> TestNodes{{1, 3, 5, 7}};

  std::vector<TestNodeTy *> DepNodes1;
  std::vector<TestNodeTy *> DepNodes1b;
  std::vector<TestNodeTy *> DepNodes2;

  TestGraphTy G1, G1b, G2;

  void SetUp() override {
    for (auto &e : TestNodes) {
      DepNodes1.emplace_back(G1.getOrInsertNode(&e));
      DepNodes1b.emplace_back(G1b.getOrInsertNode(&e));
      DepNodes2.emplace_back(G2.getOrInsertNode(&e));
    }

    DepNodes1[0]->addDependentNode(DepNodes1[1]);
    DepNodes1[0]->addDependentNode(DepNodes1[2]);
    DepNodes1[0]->addDependentNode(DepNodes1[3]);

    DepNodes1b[0]->addDependentNode(DepNodes1b[1]);
    DepNodes1b[0]->addDependentNode(DepNodes1b[2]);
    DepNodes1b[0]->addDependentNode(DepNodes1b[3]);

    DepNodes2[3]->addDependentNode(DepNodes2[0]);
    DepNodes2[3]->addDependentNode(DepNodes2[1]);
    DepNodes2[3]->addDependentNode(DepNodes2[2]);
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

TEST_F(GenericDependenceGraphTest, IterateNodeEdges) {
  using GT = llvm::GraphTraits<TestGraphTy *>;

  auto n = std::distance(GT::child_edge_begin(DepNodes1[0]),
                         GT::child_edge_end(DepNodes1[0]));

  EXPECT_EQ(3, n);
}

TEST_F(GenericDependenceGraphTest, IterateGraphEdges) {
  auto n = std::distance(G1.edges_begin(), G1.edges_end());

  EXPECT_EQ(3, n);
}

} // unnamed namespace
} // namespace testing
} // namespace pedigree
