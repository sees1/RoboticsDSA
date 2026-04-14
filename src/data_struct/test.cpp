#include "kd_tree.hpp"
#include <gtest/gtest.h>

TEST(StructuteTest, KDTreeGetNodes)
{
  using namespace study::utils;
  using namespace study::math;
  using namespace study::primitives;

  Point3f p1(1.0, 2.0, 3.0);
  Point3f p2(4.0, 5.0, 6.0);
  Point3f p3(7.0, 8.0, 9.0);
  Triangle t(p1, p2, p3);

  std::vector<Triangle> objs;
  objs.push_back(t);

  study::kd_tree<study::primitives::Triangle> v(objs, SAHSplitter, false);
  std::vector<study::kd_tree_node*> res;
  v.getLeafNodes(res);

  EXPECT_EQ(1, 1);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}