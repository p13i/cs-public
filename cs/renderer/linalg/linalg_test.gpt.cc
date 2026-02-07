// cs/renderer/linalg/linalg_test.gpt.cc
#include <cmath>

#include "cs/renderer/linalg/matrix4x4.hh"
#include "cs/renderer/linalg/transform.hh"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::renderer::geo::Point3;
using ::cs::renderer::geo::Vector3;
using ::cs::renderer::linalg::Matrix4x4;
using ::cs::renderer::linalg::Transform;
using ::cs::renderer::linalg::transforms::LookAt;
using ::cs::renderer::linalg::transforms::Rotate;
using ::cs::renderer::linalg::transforms::RotateX;
using ::cs::renderer::linalg::transforms::RotateY;
using ::cs::renderer::linalg::transforms::RotateZ;
using ::cs::renderer::linalg::transforms::Scale;
using ::cs::renderer::linalg::transforms::Translate;
using ::testing::IsFalse;
using ::testing::IsTrue;
}  // namespace

TEST(Matrix4x4Test, GetSetAndBounds) {
  Matrix4x4 m;
  ASSERT_THAT(m.set(0, 0, 2.0f).ok(), IsTrue());
  auto val = m.get(0, 0);
  ASSERT_THAT(val.ok(), IsTrue());
  EXPECT_FLOAT_EQ(val.value(), 2.0f);
  auto out_of_range = m.get(5, 0);
  EXPECT_THAT(out_of_range.ok(), IsFalse());
  auto y_out_of_range = m.get(0, 5);
  EXPECT_THAT(y_out_of_range.ok(), IsFalse());
}

TEST(Matrix4x4Test, InverseAndTranspose) {
  Matrix4x4 m;
  auto inv = m.inverse();
  ASSERT_THAT(inv.ok(), IsTrue());
  auto t = m.transpose();
  auto t00 = t.get(0, 0);
  ASSERT_THAT(t00.ok(), IsTrue());
  EXPECT_FLOAT_EQ(t00.value(), 1.0f);
}

TEST(Matrix4x4Test, InverseSingularFails) {
  Matrix4x4 m(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
              0);
  auto inv = m.inverse();
  EXPECT_THAT(inv.ok(), IsFalse());
}

TEST(TransformTest, TranslateAndScale) {
  Transform t = Translate(Vector3(1.0f, 2.0f, 3.0f));
  auto moved = t(Point3(1.0f, 1.0f, 1.0f));
  ASSERT_THAT(moved.ok(), IsTrue());
  EXPECT_EQ(moved.value(), Point3(2.0f, 3.0f, 4.0f));

  Transform s = Scale(2.0f, 3.0f, 4.0f);
  auto scaled = s(Point3(1.0f, 1.0f, 1.0f));
  ASSERT_THAT(scaled.ok(), IsTrue());
  EXPECT_EQ(scaled.value(), Point3(2.0f, 3.0f, 4.0f));
}

TEST(TransformTest, Rotations) {
  const float kPi = 3.14159265f;
  Transform rx = RotateX(kPi / 2.0f);
  auto rxp = rx(Point3(0.0f, 1.0f, 0.0f));
  ASSERT_THAT(rxp.ok(), IsTrue());
  EXPECT_NEAR(rxp.value().y, 0.0f, 1e-4f);

  Transform ry = RotateY(kPi / 2.0f);
  auto ryp = ry(Point3(1.0f, 0.0f, 0.0f));
  ASSERT_THAT(ryp.ok(), IsTrue());
  EXPECT_NEAR(ryp.value().x, 0.0f, 1e-4f);

  Transform rz = RotateZ(kPi / 2.0f);
  auto rzp = rz(Point3(1.0f, 0.0f, 0.0f));
  ASSERT_THAT(rzp.ok(), IsTrue());
  EXPECT_NEAR(rzp.value().y, 1.0f, 1e-4f);

  Transform r =
      Rotate(kPi / 2.0f, Vector3(0.0f, 0.0f, 1.0f));
  auto rp = r(Point3(1.0f, 0.0f, 0.0f));
  ASSERT_THAT(rp.ok(), IsTrue());
  EXPECT_NEAR(rp.value().y, 1.0f, 1e-4f);
}

TEST(TransformTest, LookAtAndDivisionByZero) {
  Transform look = LookAt(Point3(0.0f, 0.0f, -1.0f),
                          Point3(0.0f, 0.0f, 0.0f),
                          Vector3(0.0f, 1.0f, 0.0f));
  auto result = look(Point3(0.0f, 0.0f, 0.0f));
  ASSERT_THAT(result.ok(), IsTrue());

  Matrix4x4 m(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0,
              0);
  Transform bad(m, m);
  auto bad_result = bad(Point3(1.0f, 2.0f, 3.0f));
  EXPECT_THAT(bad_result.ok(), IsFalse());
}
