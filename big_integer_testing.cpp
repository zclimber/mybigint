#include <big_integer.h>
#include <gmp_big_integer.h>
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <vector>
#include <utility>
#include <random>
#include <gtest/gtest.h>

#include "cow_vector.h"
#include <vector>
#include <iostream>

template<class T>
std::ostream& operator<<(std::ostream& stream, const std::vector<T> & v) {
	std::string str("[");
	for (const T & t : v) {
		str += std::to_string(t) + ", ";
	}
	str.pop_back();
	str.back() = ']';
	return stream << str << "\n";
}

template<class T, unsigned int S>
std::ostream& operator<<(std::ostream& stream, const cow_vector<T, S> & cw) {
	cw.assert_correctness();
	if (cw.is_local()) {
		std::string str("[");
		for (eint i = 0; i < cw.local_size; i++) {
			str += std::to_string(cw.local_data[i]) + ", ";
		}
		str.pop_back();
		str.back() = ']';
		return stream << str << "\n";
	} else {
		return stream << *cw.rem_data;
	}
}

TEST(cow, constr_def) {
	auto cw = cow_vector<int, 10>();
	std::vector<int> vv;
	EXPECT_TRUE(cw.compare(vv));
}

TEST(cow, constr_size) {
	auto cw = cow_vector<int, 10>(5);
	std::vector<int> vv(5);
	EXPECT_TRUE(cw.compare(vv));
	auto cw2 = cow_vector<int, 10>(15);
	std::vector<int> vv2(15);
	EXPECT_TRUE(cw2.compare(vv2));
}

TEST(cow, constr_size_elem) {
	std::vector<int> vv(5, 1);
	std::vector<int> vv2(15, 2);
	auto cw = cow_vector<int, 10>(5, 1);
	auto cw2 = cow_vector<int, 10>(15, 2);
	EXPECT_TRUE(cw.compare(vv));
	EXPECT_TRUE(cw2.compare(vv2));
}

TEST(cow, constr_copy) {
	std::vector<int> vv(5, 1);
	std::vector<int> vv2(15, 2);

	auto cw = cow_vector<int, 10>(5, 1);
	auto cw2 = cow_vector<int, 10>(15, 2);

	auto cww = cow_vector<int, 10>(cw);
	auto cww2 = cow_vector<int, 10>(cw2);

	EXPECT_TRUE(cww.compare(vv));
	EXPECT_TRUE(cww2.compare(vv2));

	EXPECT_TRUE(cw.same_remote(cww));
	EXPECT_TRUE(cw2.same_remote(cww2));
}

TEST(cow, constr_move) {
	auto cww = cow_vector<int, 10>(cow_vector<int, 10>(5, 1));
	std::vector<int> vv(5, 1);
	EXPECT_TRUE(cww.compare(vv));
	auto cww2 = cow_vector<int, 10>(cow_vector<int, 10>(15, 2));
	std::vector<int> vv2(15, 2);
	EXPECT_TRUE(cww2.compare(vv2));
}

TEST(cow, assign_oth_copy) {
	std::vector<int> vv(5, 1);
	std::vector<int> vv2(15, 2);

	auto cw = cow_vector<int, 10>(5, 1);
	auto cw2 = cow_vector<int, 10>(15, 2);

	auto cww = cow_vector<int, 10>(7);
	cww = cw;
	EXPECT_TRUE(cww.compare(vv));
	EXPECT_TRUE(cww.same_remote(cw));

	auto cww2 = cow_vector<int, 10>(71, 6);
	cww2 = cw2;
	EXPECT_TRUE(cww2.compare(vv2));
	EXPECT_TRUE(cww2.same_remote(cw2));

	cww = cw2;
	EXPECT_TRUE(cww.compare(vv2));

	cww2 = cw;
	EXPECT_TRUE(cww2.compare(vv));

	EXPECT_TRUE(cw.compare(vv));
	EXPECT_TRUE(cw2.compare(vv2));
}

TEST(cow, assign_oth_move) {
//	auto cw = cow_vector<int, 10>(5, 1);
	auto cww = cow_vector<int, 10>(7);
	cww = cow_vector<int, 10>(5, 1);
	std::vector<int> vv(5, 1);
	EXPECT_TRUE(cww.compare(vv));
//	auto cw2 = cow_vector<int, 10>(15, 2);
	auto cww2 = cow_vector<int, 10>(71, 6);
	cww2 = cow_vector<int, 10>(15, 2);
	std::vector<int> vv2(15, 2);
	EXPECT_TRUE(cww2.compare(vv2));

	cww = cow_vector<int, 10>(15, 2);
	EXPECT_TRUE(cww.compare(vv2));

	cww2 = cow_vector<int, 10>(5, 1);
	EXPECT_TRUE(cww2.compare(vv));
}

TEST(cow, assign_copies) {
	auto cww = cow_vector<int, 10>(7);
	cww.assign(5, 1);
	std::vector<int> vv(5, 1);
	EXPECT_TRUE(cww.compare(vv));
	auto cww2 = cow_vector<int, 10>(71, 6);
	cww2.assign(15, 2);
	std::vector<int> vv2(15, 2);
	EXPECT_TRUE(cww2.compare(vv2));

	cww.assign(15, 2);
	EXPECT_TRUE(cww.compare(vv2));

	cww2.assign(5, 1);
	EXPECT_TRUE(cww2.compare(vv));
}

TEST(cow, assign_range) {
	std::vector<int> vv(5, 1);
	std::vector<int> vv2(15, 2);
	auto cww = cow_vector<int, 10>(7);
	cww.assign(vv.begin(), vv.end());
	EXPECT_TRUE(cww.compare(vv));
	auto cww2 = cow_vector<int, 10>(71, 6);
	cww2.assign(vv2.begin(), vv2.end());
	EXPECT_TRUE(cww2.compare(vv2));

	cww.assign(vv2.begin(), vv2.end());
	EXPECT_TRUE(cww.compare(vv2));

	cww2.assign(vv.begin(), vv.end());
	EXPECT_TRUE(cww2.compare(vv));

}

TEST(cow, access) {
	std::vector<int> vv(5, 1);
	std::vector<int> vv2(15, 2);

	auto cw = cow_vector<int, 10>(5, 1);
	auto cw2 = cow_vector<int, 10>(15, 2);

	auto cww = cow_vector<int, 10>(7);
	cww = cw;
	EXPECT_TRUE(cww.compare(vv));
	EXPECT_TRUE(cww.same_remote(cw));

	auto cww2 = cow_vector<int, 10>(71, 6);
	cww2 = cw2;
	EXPECT_TRUE(cww2.compare(vv2));
	EXPECT_TRUE(cww2.same_remote(cw2));

	cw[3] = vv[3] = 5;
	EXPECT_TRUE(cw.compare(vv));
	vv[3] = 1;
	EXPECT_TRUE(cww.compare(vv));

	cw2[7] = vv2[7] = 7;
	EXPECT_TRUE(cw2.compare(vv2));
	EXPECT_FALSE(cww2.same_remote(cw2));
}

TEST(cow, access_const) {
	std::vector<int> vv(5, 1);
	std::vector<int> vv2(15, 2);

	const auto cw = cow_vector<int, 10>(5, 1);
	const auto cw2 = cow_vector<int, 10>(15, 2);

	auto cww = cow_vector<int, 10>(7);
	cww = cw;
	EXPECT_TRUE(cww.compare(vv));
	EXPECT_TRUE(cww.same_remote(cw));

	auto cww2 = cow_vector<int, 10>(71, 6);
	cww2 = cw2;
	EXPECT_TRUE(cww2.compare(vv2));
	EXPECT_TRUE(cww2.same_remote(cw2));

	EXPECT_TRUE(cw[3] == vv[3]);

	EXPECT_TRUE(vv2[11] == cw2[11]);
	EXPECT_TRUE(cww2.same_remote(cw2));
}

TEST(cow, front) {
	std::vector<int> vv( { 1, 2, 3, 4, 5 });
	std::vector<int> vv2( { 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 });

	auto cw = cow_vector<int, 10>();
	cw.assign(vv.begin(), vv.end());

	auto cw2 = cow_vector<int, 10>();
	cw2.assign(vv2.begin(), vv2.end());

	auto cww = cw;
	auto cww2 = cw2;

	cww.front() = vv.front() = 7;
	EXPECT_TRUE(cww.compare(vv));
	EXPECT_FALSE(!cww.is_local() && cww.same_remote(cw));

	cww2.front() = vv2.front() = 70;

	EXPECT_TRUE(cww2.compare(vv2));
	EXPECT_FALSE(!cww2.is_local() && cww2.same_remote(cw2));
}

TEST(cow, back) {
	std::vector<int> vv( { 1, 2, 3, 4, 5 });
	std::vector<int> vv2( { 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 });

	auto cw = cow_vector<int, 10>();
	cw.assign(vv.begin(), vv.end());

	auto cw2 = cow_vector<int, 10>();
	cw2.assign(vv2.begin(), vv2.end());

	auto cww = cw;
	auto cww2 = cw2;

	cww.back() = vv.back() = 7;
	EXPECT_TRUE(cww.compare(vv));
	EXPECT_FALSE(!cww.is_local() && cww.same_remote(cw));

	cww2.back() = vv2.back() = 70;

	EXPECT_TRUE(cww2.compare(vv2));
	EXPECT_FALSE(!cww2.is_local() && cww2.same_remote(cw2));
}

TEST(cow, iterators) {
	std::vector<int> vv( { 1, 2, 3, 4, 5 });
	std::vector<int> vv2( { 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 });

	auto cw = cow_vector<int, 10>();
	cw.assign(vv.begin(), vv.end());

	auto cw2 = cow_vector<int, 10>();
	cw2.assign(vv2.begin(), vv2.end());

	auto cww = cw;
	auto cww2 = cw2;

	std::vector<int> v(cww.begin(), cww.end());
	EXPECT_EQ(vv, v);
	EXPECT_FALSE(!cww.is_local() && cww.same_remote(cw));

	std::vector<int> v2(cww2.begin(), cww2.end());
	EXPECT_EQ(vv2, v2);
	EXPECT_FALSE(!cww2.is_local() && cww2.same_remote(cw2));
}

TEST(cow, const_iterators) {
	std::vector<int> vv( { 1, 2, 3, 4, 5 });
	std::vector<int> vv2( { 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 });

	auto cw = cow_vector<int, 10>();
	cw.assign(vv.begin(), vv.end());

	auto cw2 = cow_vector<int, 10>();
	cw2.assign(vv2.begin(), vv2.end());

	const auto cww = cw;
	const auto cww2 = cw2;

	std::vector<int> v(cww.begin(), cww.end());
	EXPECT_EQ(vv, v);
	EXPECT_TRUE(cww.is_local() || cww.same_remote(cw));

	std::vector<int> v2(cww2.begin(), cww2.end());
	EXPECT_EQ(vv2, v2);
	EXPECT_TRUE(cww2.is_local() || cww2.same_remote(cw2));
}

TEST(cow, size) {
	std::vector<int> vv( { 1, 2, 3, 4, 5 });
	std::vector<int> vv2( { 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 });

	auto cw = cow_vector<int, 10>();
	cw.assign(vv.begin(), vv.end());

	auto cw2 = cow_vector<int, 10>();
	cw2.assign(vv2.begin(), vv2.end());

	auto cww = cw;
	auto cww2 = cw2;

	EXPECT_EQ(5UL, cww.size());
	EXPECT_EQ(15UL, cww2.size());
	EXPECT_TRUE(cww.is_local() || cww.same_remote(cw));
	EXPECT_TRUE(cww2.is_local() || cww2.same_remote(cw2));
}

TEST(cow, capacity) {
	std::vector<int> vv( { 1, 2, 3, 4, 5 });
	std::vector<int> vv2( { 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 });

	auto cw = cow_vector<int, 10>();
	cw.assign(vv.begin(), vv.end());

	auto cw2 = cow_vector<int, 10>();
	cw2.assign(vv2.begin(), vv2.end());

	auto cww = cw;
	auto cww2 = cw2;

	EXPECT_EQ(10UL, cww.capacity());
//	EXPECT_EQ(15UL, cww2.capacity());
	EXPECT_TRUE(cww.is_local() || cww.same_remote(cw));
	EXPECT_TRUE(cww2.is_local() || cww2.same_remote(cw2));
}

TEST(cow, resize) {
	std::vector<int> vv( { 1, 2, 3, 4, 5 });
	std::vector<int> vv2( { 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 });
	const auto vvt = vv;
	const auto vvt2 = vv2;

	auto cw = cow_vector<int, 10>();
	cw.assign(vv.begin(), vv.end());

	auto cw2 = cow_vector<int, 10>();
	cw2.assign(vv2.begin(), vv2.end());

	auto cww = cw;
	auto cww2 = cw2;

	cww.resize(3);
	vv.resize(3);
	EXPECT_TRUE(cww.compare(vv));
	EXPECT_TRUE(cww.is_local());
	EXPECT_EQ(3, cww.size());

	cww = cw;
	vv = vvt;
	cww.resize(11);
	vv.resize(11);
	EXPECT_TRUE(cww.compare(vv));
	EXPECT_FALSE(!cww.is_local() && cww.same_remote(cw));
	EXPECT_EQ(11, cww.size());

	cww = cw;
	vv = vvt;
	cww.resize(11, 9);
	vv.resize(11, 9);
	EXPECT_TRUE(cww.compare(vv));
	EXPECT_FALSE(!cww.is_local() && cww.same_remote(cw));
	EXPECT_EQ(11, cww.size());

	cww2.resize(3);
	vv2.resize(3);
	EXPECT_TRUE(cww2.compare(vv2));
	EXPECT_TRUE(cww2.is_local());
	EXPECT_EQ(3, cww2.size());

	cww2 = cw2;
	vv2 = vvt2;
	cww2.resize(17);
	vv2.resize(17);
	EXPECT_TRUE(cww2.compare(vv2));
	EXPECT_FALSE(!cww2.is_local() && cww2.same_remote(cw2));
	EXPECT_EQ(17, cww2.size());

	cww2 = cw2;
	vv2 = vvt2;
	cww2.resize(17, 9);
	vv2.resize(17, 9);
	EXPECT_TRUE(cww2.compare(vv2));
	EXPECT_FALSE(!cww2.is_local() && cww2.same_remote(cw2));
	EXPECT_EQ(17, cww2.size());
}

TEST(cow, reserve) {
	std::vector<int> vv( { 1, 2, 3, 4, 5 });
	std::vector<int> vv2( { 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 });
	const auto vvt = vv;
	const auto vvt2 = vv2;

	auto cw = cow_vector<int, 10>();
	cw.assign(vv.begin(), vv.end());

	auto cw2 = cow_vector<int, 10>();
	cw2.assign(vv2.begin(), vv2.end());

	auto cww = cw;
	auto cww2 = cw2;
	cww.reserve(7);
	EXPECT_TRUE(cww.capacity() >= 7);
	cww.reserve(13);
	EXPECT_TRUE(cww.capacity() >= 13);
	cww2.reserve(21);
	EXPECT_TRUE(cww2.capacity() >= 21);
	EXPECT_FALSE(!cww2.is_local() && cww2.same_remote(cw2));
}

TEST(cow, shrink) {
	std::vector<int> vv( { 1, 2, 3, 4, 5 });
	std::vector<int> vv2( { 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 });
	const auto vvt = vv;
	const auto vvt2 = vv2;

	auto cw = cow_vector<int, 10>();
	cw.assign(vv.begin(), vv.end());

	auto cw2 = cow_vector<int, 10>();
	cw2.assign(vv2.begin(), vv2.end());

	auto cww = cw;
	auto cww2 = cw2;

	cww.reserve(11);
	EXPECT_TRUE(cww.capacity() >= 11);
	cww.shrink_to_fit();
	EXPECT_TRUE(cww.is_local());

	cww2.shrink_to_fit();
	EXPECT_FALSE(!cww2.is_local() && cww2.same_remote(cw2));

	cww2.reserve(21);
	EXPECT_TRUE(cww2.capacity() >= 21);
	EXPECT_FALSE(!cww2.is_local() && cww2.same_remote(cw2));
	cww2.shrink_to_fit();
	EXPECT_TRUE(cww2.capacity() == 15);
}

TEST(cow, append) {
	// TODO
}

TEST(cow, push_back) {
	// TODO
}

TEST(cow, pop_back) {
	// TODO
}

TEST(cow, clear) {
	// TODO
}

TEST(cow, fast_copy) {
	// TODO
}

TEST(cow, random_input) {
	// TODO
}

TEST(mlim, lims) {
	big_integer a = std::numeric_limits<int>::min();
	big_integer b = std::numeric_limits<int>::max();
	EXPECT_EQ(a / -1, a / big_integer(-1));
	EXPECT_EQ((a / -1) - b, big_integer(1));
}

TEST(correctness, two_plus_two) {
	EXPECT_EQ(big_integer(2) + big_integer(2), big_integer(4));
	EXPECT_EQ(big_integer(2) + 2, 4); // implicit converion from int must work
	EXPECT_EQ(2 + big_integer(2), 4);
}

TEST(correctness, default_ctor) {
	big_integer a;
	big_integer b = 0;
	EXPECT_EQ(a, 0);
	EXPECT_EQ(a, b);
}

TEST(correctness, ctor_limits) {
	big_integer a = std::numeric_limits<int>::min();
	big_integer b = std::numeric_limits<int>::max();
	EXPECT_EQ(a + b, -1);
}

TEST(correctness, copy_ctor) {
	big_integer a = 3;
	big_integer b = a;

	EXPECT_EQ(a, b);
	EXPECT_EQ(b, 3);
}

TEST(correctness, copy_ctor_real_copy) {
	big_integer a = 3;
	big_integer b = a;
	a = 5;

	EXPECT_EQ(b, 3);
}

TEST(correctness, copy_ctor_real_copy2) {
	big_integer a = 3;
	big_integer b = a;
	b = 5;

	EXPECT_EQ(a, 3);
}

TEST(correctness, assignment_operator) {
	big_integer a = 4;
	big_integer b = 7;
	b = a;

	EXPECT_TRUE(a == b);
}

TEST(correctness, self_assignment) {
	big_integer a = 5;
	a = a;

	EXPECT_TRUE(a == 5);
}

TEST(correctness, assignment_return_value) {
	big_integer a = 4;
	big_integer b = 7;
	(a = b) = a;

	EXPECT_TRUE(a == 7);
	EXPECT_TRUE(b == 7);
}

TEST(correctness, comparisons) {
	big_integer a = 100;
	big_integer b = 100;
	big_integer c = 200;

	EXPECT_TRUE(a == b);
	EXPECT_TRUE(a != c);
	EXPECT_TRUE(a < c);
	EXPECT_TRUE(c > a);
	EXPECT_TRUE(a <= a);
	EXPECT_TRUE(a <= b);
	EXPECT_TRUE(a <= c);
	EXPECT_TRUE(c >= a);
}

TEST(correctness, compare_zero_and_minus_zero) {
	big_integer a;
	big_integer b = -a;

	EXPECT_TRUE(a == b);
}

TEST(correctness, add) {
	big_integer a = 5;
	big_integer b = 20;

	EXPECT_TRUE(a + b == 25);

	a += b;
	EXPECT_TRUE(a == 25);
}

TEST(correctness, add_signed) {
	big_integer a = 5;
	big_integer b = -20;

	EXPECT_TRUE(a + b == -15);

	a += b;
	EXPECT_TRUE(a == -15);
}

TEST(correctness, add_return_value) {
	big_integer a = 5;
	big_integer b = 1;

	(a += b) += b;
	EXPECT_EQ(a, 7);
}

TEST(correctness, sub) {
	big_integer a = 20;
	big_integer b = 5;

	EXPECT_TRUE(a - b == 15);

	a -= b;
	EXPECT_TRUE(a == 15);
}

TEST(correctness, sub_signed) {
	big_integer a = 5;
	big_integer b = 20;

	EXPECT_TRUE(a - b == -15);

	a -= b;
	EXPECT_TRUE(a == -15);

	a -= -100;
	EXPECT_TRUE(a == 85);
}

TEST(correctness, sub_return_value) {
	big_integer a = 5;
	big_integer b = 1;

	(a -= b) -= b;
	EXPECT_EQ(a, 3);
}

TEST(correctness, mul) {
	big_integer a = 5;
	big_integer b = 20;

	EXPECT_TRUE(a * b == 100);

	a *= b;
	EXPECT_TRUE(a == 100);
}

TEST(correctness, mul_signed) {
	big_integer a = -5;
	big_integer b = 20;

	EXPECT_TRUE(a * b == -100);

	a *= b;
	EXPECT_TRUE(a == -100);
}

TEST(correctness, mul_return_value) {
	big_integer a = 5;
	big_integer b = 2;

	(a *= b) *= b;
	EXPECT_EQ(a, 20);
}

TEST(correctness, div_) {
	big_integer a = 20;
	big_integer b = 5;
	big_integer c = 20;

	EXPECT_TRUE(a / b == 4);
	EXPECT_TRUE(a % b == 0);

	a /= b;
	EXPECT_TRUE(a == 4);

	c %= b;
	EXPECT_TRUE(c == 0);
}

TEST(correctness, div_int_min) {
	big_integer a = std::numeric_limits<int>::min();
	EXPECT_TRUE((a / a) == (a / std::numeric_limits<int>::min()));
}

TEST(correctness, div_int_min_2) {
	big_integer a = std::numeric_limits<int>::min();
	big_integer b = -1;
	big_integer c = a / b;
	EXPECT_TRUE(c == (a / -1));
	EXPECT_TRUE((c - std::numeric_limits<int>::max()) == 1);
}

TEST(correctness, div_signed) {
	big_integer a = -20;
	big_integer b = 5;

	EXPECT_TRUE(a / b == -4);
	EXPECT_TRUE(a % b == 0);
}

TEST(correctness, div_rounding) {
	big_integer a = 23;
	big_integer b = 5;

	EXPECT_TRUE(a / b == 4);
	EXPECT_TRUE(a % b == 3);
}

TEST(correctness, div_rounding_negative) {
	big_integer a = 23;
	big_integer b = -5;
	big_integer c = -23;
	big_integer d = 5;

	EXPECT_TRUE(a / b == -4);
	EXPECT_TRUE(c / d == -4);
	EXPECT_TRUE(a % b == 3);
	EXPECT_TRUE(c % d == -3);
}

TEST(correctness, div_return_value) {
	big_integer a = 100;
	big_integer b = 2;

	(a /= b) /= b;
	EXPECT_EQ(a, 25);
}

TEST(correctness, unary_plus) {
	big_integer a = 123;
	big_integer b = +a;

	EXPECT_TRUE(a == b);

	// this code should not compile:
	// &+a;
}

TEST(correctness, negation) {
	big_integer a = 666;
	big_integer b = -a;

	EXPECT_TRUE(b == -666);
}

TEST(correctness, and_) {
	big_integer a = 0x55;
	big_integer b = 0xaa;

	EXPECT_TRUE((a & b) == 0);
	EXPECT_TRUE((a & 0xcc) == 0x44);
	a &= b;
	EXPECT_TRUE(a == 0);
}

TEST(correctness, and_signed) {
	big_integer a = 0x55;
	big_integer b = 0xaa;

	EXPECT_TRUE((b & -1) == 0xaa);
	EXPECT_TRUE((a & (0xaa - 256)) == 0);
	EXPECT_TRUE((a & (0xcc - 256)) == 0x44);
}

TEST(correctness, and_return_value) {
	big_integer a = 7;

	(a &= 3) &= 6;
	EXPECT_EQ(a, 2);
}

TEST(correctness, or_) {
	big_integer a = 0x55;
	big_integer b = 0xaa;

	EXPECT_TRUE((a | b) == 0xff);
	a |= b;
	EXPECT_TRUE(a == 0xff);
}

TEST(correctness, or_signed) {
	big_integer a = 0x55;
	big_integer b = 0xaa;

	EXPECT_TRUE((a | (b - 256)) == -1);
}

TEST(correctness, or_return_value) {
	big_integer a = 1;

	(a |= 2) |= 4;
	EXPECT_EQ(a, 7);
}

TEST(correctness, xor_) {
	big_integer a = 0xaa;
	big_integer b = 0xcc;

	EXPECT_TRUE((a ^ b) == 0x66);
}

TEST(correctness, xor_signed) {
	big_integer a = 0xaa;
	big_integer b = 0xcc;

	EXPECT_TRUE((a ^ (b - 256)) == (0x66 - 256));
}

TEST(correctness, xor_return_value) {
	big_integer a = 1;

	(a ^= 2) ^= 1;
	EXPECT_EQ(a, 2);
}

TEST(correctness, not_) {
	big_integer a = 0xaa;

	EXPECT_TRUE(~a == (-a - 1));
}

TEST(correctness, shl_) {
	big_integer a = 23;

	EXPECT_TRUE((a << 5) == 23 * 32);

	a <<= 5;
	EXPECT_TRUE(a == 23 * 32);
}

TEST(correctness, shl_return_value) {
	big_integer a = 1;

	(a <<= 2) <<= 1;
	EXPECT_EQ(a, 8);
}

TEST(correctness, shr_) {
	big_integer a = 23;

	EXPECT_EQ(a >> 2, 5);

	a >>= 2;
	EXPECT_EQ(a, 5);
}

TEST(correctness, shr_31) {
	big_integer a = 65536;

	EXPECT_EQ((a * a) >> 31, 2);
}

TEST(correctness, shr_signed) {
	big_integer a = -1234;

	EXPECT_EQ(a >> 3, -155);

	a >>= 3;
	EXPECT_EQ(a, -155);
}

TEST(correctness, shr_return_value) {
	big_integer a = 64;

	(a >>= 2) >>= 1;
	EXPECT_EQ(a, 8);
}

TEST(correctness, add_long) {
	big_integer a("10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
	big_integer b("100000000000000000000000000000000000000");
	big_integer c("10000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000");

	EXPECT_EQ(a + b, c);
}

TEST(correctness, add_long_signed) {
	big_integer a("-1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
	big_integer b("1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");

	EXPECT_EQ(a + b, 0);
}

TEST(correctness, add_long_signed2) {
	big_integer a("-1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
	big_integer b("100000000000000000000000000000000000000");
	big_integer c("-999999999999999999999999999999999999999999999999999900000000000000000000000000000000000000");

	EXPECT_EQ(a + b, c);
}

TEST(correctness, add_long_pow2) {
	big_integer a("18446744073709551616");
	big_integer b("-18446744073709551616");
	big_integer c("36893488147419103232");

	EXPECT_EQ(a + a, c);
	EXPECT_EQ(b + c, a);
	EXPECT_EQ(c + b, a);
}

TEST(correctness, sub_long) {
	big_integer a("10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
	big_integer b("100000000000000000000000000000000000000");
	big_integer c("9999999999999999999999999999999999999999999999999999900000000000000000000000000000000000000");

	EXPECT_EQ(a - b, c);
}

TEST(correctness, mul_long) {
	big_integer a("10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
	big_integer b("100000000000000000000000000000000000000");
	big_integer c("10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
			"00000000000000000000000000000000000000");

	EXPECT_EQ(a * b, c);
}

TEST(correctness, mul_long_signed) {
	big_integer a("-1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
	big_integer b("100000000000000000000000000000000000000");
	big_integer c("-1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
			"00000000000000000000000000000000000000");

	EXPECT_EQ(a * b, c);
}

TEST(correctness, mul_long_signed2) {
	big_integer a("-100000000000000000000000000");
	big_integer c("100000000000000000000000000"
			"00000000000000000000000000");

	EXPECT_EQ(a * a, c);
}

TEST(correctness, mul_long_pow2) {
	big_integer a("18446744073709551616");
	big_integer b("340282366920938463463374607431768211456");
	big_integer c("115792089237316195423570985008687907853269984665640564039457584007913129639936");

	EXPECT_EQ(a * a, b);
	EXPECT_EQ(b * b, c);
}

TEST(correctness, div_long) {
	big_integer a("10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
	big_integer b("100000000000000000000000000000000000000");
	big_integer c("100000000000000000000000000000000000000000000000000000");

	EXPECT_EQ(a / b, c);
}

TEST(correctness, div_long_signed) {
	big_integer a("-10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
	big_integer b("100000000000000000000000000000000000000");
	big_integer c("-100000000000000000000000000000000000000000000000000000");

	EXPECT_EQ(a / b, c);
}

TEST(correctness, div_long_signed2) {
	big_integer a("-10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
	big_integer b("-100000000000000000000000000000000000000");
	big_integer c("100000000000000000000000000000000000000000000000000000");

	EXPECT_EQ(a / b, c);
}

TEST(correctness, negation_long) {
	big_integer a("10000000000000000000000000000000000000000000000000000");
	big_integer c("-10000000000000000000000000000000000000000000000000000");

	EXPECT_EQ(-a, c);
	EXPECT_EQ(a, -c);
}

TEST(correctness, string_conv) {
	EXPECT_EQ(to_string(big_integer("100")), "100");
	EXPECT_EQ(to_string(big_integer("0100")), "100");
	EXPECT_EQ(to_string(big_integer("0")), "0");
	EXPECT_EQ(to_string(big_integer("-0")), "0");
	EXPECT_EQ(to_string(big_integer("-1000000000000000")), "-1000000000000000");
}

namespace {
unsigned const number_of_iterations = 10;
size_t const number_of_multipliers = 1000;

int myrand() {
	int val = rand() - RAND_MAX / 2;
	if (val != 0)
		return val;
	else
		return 1;
}
}

TEST(correctness, mul_div_randomized) {
	for (unsigned itn = 0; itn != number_of_iterations; ++itn) {
		std::vector<int> multipliers;

		for (size_t i = 0; i != number_of_multipliers; ++i)
			multipliers.push_back(myrand());

		big_integer accumulator = 1;

		for (size_t i = 0; i != number_of_multipliers; ++i)
			accumulator *= multipliers[i];

		std::random_shuffle(multipliers.begin(), multipliers.end());

		for (size_t i = 1; i != number_of_multipliers; ++i)
			accumulator /= multipliers[i];

		EXPECT_TRUE(accumulator == multipliers[0]);
	}
}

namespace {
template<typename T>
void erase_unordered(std::vector<T>& v, typename std::vector<T>::iterator pos) {
	std::swap(v.back(), *pos);
	v.pop_back();
}

template<typename T>
T extract_random_element(std::vector<T>& v) {
	size_t index = rand() % v.size();
	T copy = v[index];
	erase_unordered(v, v.begin() + index);
	return copy;
}

template<typename T>
void merge_two(std::vector<T>& v) {
	assert(v.size() >= 2);

	T a = extract_random_element(v);
	T b = extract_random_element(v);

	T ab = a * b;
	ASSERT_TRUE(ab / a == b);
	ASSERT_TRUE(ab / b == a);

	v.push_back(ab);
}

template<typename T>
T merge_all(std::vector<T> v) {
	assert(!v.empty());

	while (v.size() >= 2)
		merge_two(v);

	return v[0];
}
}

TEST(correctness, mul_merge_randomized) {
	for (unsigned itn = 0; itn != number_of_iterations; ++itn) {
		std::vector<big_integer> x;
		for (size_t i = 0; i != number_of_multipliers; ++i)
			x.push_back(myrand());

		big_integer a = merge_all(x);
		big_integer b = merge_all(x);

		EXPECT_TRUE(a == b);
	}
}

TEST(cow_bigint, long_copy){
	std::vector<big_integer> vec(100);
	big_integer original(1000000000);
	for(unsigned i = 0; i < 15; i++){
		original *= original;
	}
	for(unsigned i = 0; i < 100; i++){
		vec[i] = original;
	}
}

TEST(cow_bigint, long_copy_b){
	std::vector<big_integer> vec(100000);
	big_integer original(1000000000);
	for(unsigned i = 0; i < 15; i++){
		original *= original;
	}
	for(unsigned i = 0; i < 100000; i++){
		vec[i] = original;
	}
}
