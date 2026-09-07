#include "kd_tree.hpp"
#include <gtest/gtest.h>

TEST(StructuteTest, KDTreeGetNodes)
{
  using namespace study::utils;
  using namespace study::math;
  using namespace study::primitives;

  Point3f p1(2.0f, 2.0f, 0.0f);
  Point3f p2(3.0f, 4.5f, 0.0f);
  Point3f p3(4.0f, 2.5f, 0.0f);
  Triangle t(p1, p2, p3);

  std::vector<Triangle> objs;
  objs.push_back(t);
  study::kd_tree<study::primitives::Triangle> v(objs, SAHSplitter);

  EXPECT_EQ(1, 1);
}

// TODO: add qt gui
int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}