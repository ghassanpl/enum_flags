/// Copyright 2017-2020 Ghassan.pl
/// Usage of the works is permitted provided that this instrument is retained with
/// the works, so that any entity that uses the works is notified of this instrument.
/// DISCLAIMER: THE WORKS ARE WITHOUT WARRANTY.

#include "../include/flag_bits_v.h"
#include <gtest/gtest.h>

template <typename RESULT_TYPE>
class flag_bits_test : public ::testing::Test {
public:
  
  using result_type = RESULT_TYPE;

};

using integer_types = ::testing::Types<
  short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, 
  signed char, unsigned char, char, wchar_t, char16_t, char32_t>;

/// Note: Can't use char8_t in the tests because for some reason you can't print these...

TYPED_TEST_SUITE(flag_bits_test, integer_types);

enum class TestEnum : int64_t
{
  Negative = -1, /// causes undefined behavior
  Zero = 0,
  ZeroTwo = 0,
  One = 1,
  OneTwo = 1,
  Seven = 7,
  Eight,
  Nine,
  Fifteen = 15,
  Sixteen,
  Seventeen,
  ThirtyOne = 31,
  ThirtyTwo,
  ThirtyThree,
  SixtyThree = 63,
  SixtyFour,
  SixtyFive,
  Small = std::numeric_limits<int64_t>::lowest(),
  Big = std::numeric_limits<int64_t>::max(),
};

enum class UnsignedTestEnum : uint64_t
{
  Big = std::numeric_limits<uint64_t>::max(),
};

using namespace enum_flags;

template <class U, auto... ARGS>
concept flag_bits_overload_exists = requires { flag_bits_v<U, ARGS...>; };

TYPED_TEST(flag_bits_test, work_with_template_parameters)
{
  EXPECT_EQ((flag_bits_v<TypeParam>), TypeParam{ 0 });
  EXPECT_EQ((flag_bits_v<TypeParam, TestEnum::Zero>), TypeParam{ 1 });
  EXPECT_EQ((flag_bits_v<TypeParam, TestEnum::Zero, TestEnum::ZeroTwo>), TypeParam{ 1 });
  EXPECT_EQ((flag_bits_v<TypeParam, TestEnum::One>), TypeParam{ 2 });
  EXPECT_EQ((flag_bits_v<TypeParam, TestEnum::Zero, TestEnum::ZeroTwo, TestEnum::One>), TypeParam{ 3 });
}

TYPED_TEST(flag_bits_test, is_set_v_works)
{
  EXPECT_TRUE((is_flag_set_v<flag_bits_v<TypeParam, TestEnum::Zero>, TestEnum::Zero>));
  EXPECT_FALSE((is_flag_set_v<flag_bits_v<TypeParam, TestEnum::Zero>, TestEnum::One>));
  EXPECT_FALSE((is_flag_set_v<flag_bits_v<TypeParam, TestEnum::One>, TestEnum::Zero>));
}

TYPED_TEST(flag_bits_test, are_all_set_v_works)
{
  EXPECT_TRUE((are_all_flags_set_v<flag_bits_v<TypeParam, TestEnum::One, TestEnum::Zero, TestEnum::Seven>, TestEnum::Zero, TestEnum::Seven, TestEnum::One>));
  EXPECT_FALSE((are_all_flags_set_v<flag_bits_v<TypeParam, TestEnum::One, TestEnum::Seven>, TestEnum::Zero, TestEnum::Seven, TestEnum::One>));
  EXPECT_TRUE((are_all_flags_set_v<flag_bits_v<TypeParam, TestEnum::One, TestEnum::Zero, TestEnum::Seven>, TestEnum::Zero, TestEnum::One>));
}

TYPED_TEST(flag_bits_test, are_any_set_v_works)
{
  EXPECT_TRUE((are_any_flags_set_v<flag_bits_v<TypeParam, TestEnum::One>, TestEnum::Zero, TestEnum::Seven, TestEnum::One>));
  EXPECT_FALSE((are_any_flags_set_v<flag_bits_v<TypeParam, TestEnum::Zero>, TestEnum::Seven, TestEnum::One>));
  EXPECT_TRUE((are_any_flags_set_v<flag_bits_v<TypeParam, TestEnum::Zero, TestEnum::Seven>, TestEnum::Seven, TestEnum::One>));
}

TYPED_TEST(flag_bits_test, set_v_works)
{
  constexpr auto one_set = flag_bits_v<TypeParam, TestEnum::One>;
  EXPECT_EQ((set_flag_v<one_set>), one_set);
  EXPECT_EQ((set_flag_v<one_set, TestEnum::Seven>), (flag_bits_v<TypeParam, TestEnum::Seven, TestEnum::One>));
  EXPECT_EQ((set_flag_v<TypeParam(0), TestEnum::Seven>), (flag_bits_v<TypeParam, TestEnum::Seven>));
  EXPECT_EQ((set_flag_v<TypeParam(0)>), (flag_bits_v<TypeParam>));
  EXPECT_EQ((set_flag_v<one_set>), one_set);
}

TYPED_TEST(flag_bits_test, unset_v_works)
{
  constexpr auto one_set = flag_bits_v<TypeParam, TestEnum::One>;
  EXPECT_EQ((unset_flag_v<one_set>), one_set);
  EXPECT_EQ((unset_flag_v<one_set, TestEnum::Seven>), one_set);
  EXPECT_EQ((unset_flag_v<one_set, TestEnum::One>), TypeParam{ 0 });
  constexpr auto seventyone = flag_bits_v<TypeParam, TestEnum::Seven, TestEnum::One>;
  EXPECT_EQ((unset_flag_v<seventyone, TestEnum::One>), (flag_bits_v<TypeParam, TestEnum::Seven>));
  EXPECT_EQ((unset_flag_v<seventyone, TestEnum::Seven>), (flag_bits_v<TypeParam, TestEnum::One>));
  EXPECT_EQ((unset_flag_v<seventyone, TestEnum::Seven, TestEnum::One>), (flag_bits_v<TypeParam>));
  EXPECT_EQ((unset_flag_v<seventyone, TestEnum::Zero, TestEnum::One>), (flag_bits_v<TypeParam, TestEnum::Seven>));
}

TYPED_TEST(flag_bits_test, toggle_v_works)
{
  constexpr auto bits = flag_bits_v<TypeParam, TestEnum::Seven, TestEnum::One, TestEnum::Seven, TestEnum::Zero>;
  constexpr auto bits2 = flag_bits_v<TypeParam, TestEnum::Seven>;
  EXPECT_EQ((toggle_flag_v<bits, TestEnum::Zero, TestEnum::One>), bits2);
  EXPECT_EQ((toggle_flag_v<bits>), bits);
}

TYPED_TEST(flag_bits_test, set_to_v_works)
{
  constexpr auto bits = flag_bits_v<TypeParam, TestEnum::Seven, TestEnum::One, TestEnum::Seven, TestEnum::Zero>;
  EXPECT_EQ((set_flag_to_v<bits, false, TestEnum::Seven, TestEnum::Zero>), (flag_bits_v<TypeParam, TestEnum::One>));
  EXPECT_EQ((set_flag_to_v<(flag_bits_v<TypeParam, TestEnum::One>), true, TestEnum::Seven, TestEnum::Zero>), bits);
}

TYPED_TEST(flag_bits_test, disallow_invalid_bit_numbers_for_template_parameters)
{
  EXPECT_FALSE((flag_bits_overload_exists<TypeParam, TestEnum::Negative>));

  EXPECT_FALSE((flag_bits_overload_exists<TypeParam, TestEnum::Small>));
  EXPECT_FALSE((flag_bits_overload_exists<TypeParam, TestEnum::Big>));

  EXPECT_FALSE((flag_bits_overload_exists<TypeParam, UnsignedTestEnum::Big>));
}

TEST(flag_bits_test, disallow_invalid_bit_numbers_for_ints)
{
  EXPECT_TRUE ((flag_bits_overload_exists<uint8_t,  TestEnum::Seven>));
  EXPECT_FALSE((flag_bits_overload_exists<uint8_t,  TestEnum::Eight>));
  EXPECT_TRUE ((flag_bits_overload_exists<uint16_t, TestEnum::Nine>));
  EXPECT_TRUE ((flag_bits_overload_exists<uint16_t, TestEnum::Fifteen>));
  EXPECT_FALSE((flag_bits_overload_exists<uint16_t, TestEnum::Sixteen>));
  EXPECT_TRUE ((flag_bits_overload_exists<uint32_t, TestEnum::Seventeen>));
  EXPECT_TRUE ((flag_bits_overload_exists<uint32_t, TestEnum::ThirtyOne>));
  EXPECT_FALSE((flag_bits_overload_exists<uint32_t, TestEnum::ThirtyTwo>));
  EXPECT_TRUE ((flag_bits_overload_exists<uint64_t, TestEnum::ThirtyThree>));
  EXPECT_TRUE ((flag_bits_overload_exists<uint64_t, TestEnum::SixtyThree>));
  EXPECT_FALSE((flag_bits_overload_exists<uint64_t, TestEnum::SixtyFour>));
  EXPECT_FALSE((flag_bits_overload_exists<uint64_t, TestEnum::SixtyFive>));

  EXPECT_TRUE ((flag_bits_overload_exists<int8_t,   TestEnum::Seven>));
  EXPECT_FALSE((flag_bits_overload_exists<int8_t,   TestEnum::Eight>));
  EXPECT_TRUE ((flag_bits_overload_exists<int16_t,  TestEnum::Nine>));
  EXPECT_TRUE ((flag_bits_overload_exists<int16_t,  TestEnum::Fifteen>));
  EXPECT_FALSE((flag_bits_overload_exists<int16_t,  TestEnum::Sixteen>));
  EXPECT_TRUE ((flag_bits_overload_exists<int32_t,  TestEnum::Seventeen>));
  EXPECT_TRUE ((flag_bits_overload_exists<int32_t,  TestEnum::ThirtyOne>));
  EXPECT_FALSE((flag_bits_overload_exists<int32_t,  TestEnum::ThirtyTwo>));
  EXPECT_TRUE ((flag_bits_overload_exists<int64_t,  TestEnum::ThirtyThree>));
  EXPECT_TRUE ((flag_bits_overload_exists<int64_t,  TestEnum::SixtyThree>));
  EXPECT_FALSE((flag_bits_overload_exists<int64_t,  TestEnum::SixtyFour>));
  EXPECT_FALSE((flag_bits_overload_exists<int64_t,  TestEnum::SixtyFive>));
}


TEST(flag_bits_test, disallow_non_integral_types)
{
  EXPECT_FALSE((flag_bits_overload_exists<bool>));
  EXPECT_FALSE((flag_bits_overload_exists<std::nullptr_t>));
  EXPECT_FALSE((flag_bits_overload_exists<float>));
  EXPECT_FALSE((flag_bits_overload_exists<double>));
  EXPECT_FALSE((flag_bits_overload_exists<long double>));
  EXPECT_FALSE((flag_bits_overload_exists<void>));

  EXPECT_FALSE((flag_bits_overload_exists<class _c>));
  EXPECT_FALSE((flag_bits_overload_exists<union _u>));
  enum _e {};
  EXPECT_FALSE((flag_bits_overload_exists<_e>));
  enum class _es {};
  EXPECT_FALSE((flag_bits_overload_exists<_es>));

  EXPECT_FALSE((flag_bits_overload_exists<int&>));
  EXPECT_FALSE((flag_bits_overload_exists<int*>));
  EXPECT_FALSE((flag_bits_overload_exists<int&&>));
  EXPECT_FALSE((flag_bits_overload_exists<int(*)()>));
  EXPECT_FALSE((flag_bits_overload_exists<int[5]>));
  EXPECT_FALSE((flag_bits_overload_exists<int(std::string::*)>));
}


TYPED_TEST(flag_bits_test, allows_cv_types)
{
  EXPECT_TRUE((flag_bits_overload_exists<std::add_const_t<TypeParam>>));
  EXPECT_TRUE((flag_bits_overload_exists<std::add_volatile_t<TypeParam>>));
  EXPECT_TRUE((flag_bits_overload_exists<std::add_cv_t<TypeParam>>));
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}