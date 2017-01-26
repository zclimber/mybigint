#include <big_integer.h>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <algorithm>

typedef std::pair<datavec, eint> dpair;

using std::swap;

const uxint bitmask32 = (1LL << 32) - 1;

//std::ofstream cdb("debug_output.txt", std::ios::out);

void trim_data(datavec & lhs) {
	while (lhs.size() > 1 && lhs.back() == 0) {
		lhs.pop_back();
	}
	if (lhs.size() * 2 <= lhs.capacity()) {
		lhs.shrink_to_fit();
	}
}

//datavec & sum(datavec & lhs, datavec & rhs);

datavec lshift(datavec const & lhs, size_t shiftsize) {
	datavec res;
	res.reserve(lhs.size() + 1 + (shiftsize >> 5)); // 2^5 == 32
	res.resize(shiftsize >> 5, 0);
	shiftsize &= 31;
	if (shiftsize == 0) {
		res.insert(res.end(), lhs.begin(), lhs.end());
	} else {
		uxint carry = 0;
		for (size_t i = 0; i < lhs.size(); i++) {
			carry += uxint(lhs[i]) << shiftsize;
			res.push_back(carry & bitmask32);
			carry >>= 32;
		}
		if (carry > 0) {
			res.push_back(carry);
		}
	}
	return res;
}

datavec rshift(datavec const & lhs, size_t shiftsize, bool rounding_mode) {
	size_t discard = shiftsize >> 5;
	shiftsize &= 31;
	if (discard >= lhs.size()) {
		return datavec(1, -1 * rounding_mode);
	}
	datavec res;
	res.reserve(lhs.size() - discard); // 2^5 == 32
	if (shiftsize == 0) {
		res.insert(res.end(), lhs.begin() + discard, lhs.end());
	} else {
		uxint carry = 0;
		if (rounding_mode) {
			for (size_t i = 0; i < discard; i++) {
				if (lhs[i] != 0) {
					carry++;
					break;
				}
			}
			carry = (lhs[discard] & ((1 << shiftsize) - 1)) > 0 ? 1 : carry;
			carry <<= shiftsize;
		}
		for (size_t i = discard; i + 1 < lhs.size(); i++) {
			carry += lhs[i] + (uxint(lhs[i + 1]) << 32);
			carry >>= shiftsize;
			res.push_back(carry & bitmask32);
			carry = 0;
		}
		if ((lhs.back() >> shiftsize) > 0 || res.size() == 0) {
			res.push_back((lhs.back() + carry) >> shiftsize);
		}
	}
	trim_data(res);
	return res;
}

datavec sum(datavec const & lhs, datavec const & rhs) {
	datavec result;

	result.reserve(std::max(lhs.size(), rhs.size()) + 1);
	xint carry = 0;
	for (size_t i = 0; i < std::min(lhs.size(), rhs.size()); i++) {
		carry += lhs[i];
		carry += rhs[i];
		result.push_back(carry & bitmask32);
		carry >>= 32;
	}
	if (lhs.size() > rhs.size()) {
		for (size_t i = rhs.size(); i < lhs.size(); i++) {
			carry = carry + lhs[i];
			result.push_back(carry & bitmask32);
			carry >>= 32;
		}
	} else {
		for (size_t i = lhs.size(); i < rhs.size(); i++) {
			carry = carry + rhs[i];
			result.push_back(carry & bitmask32);
			carry >>= 32;
		}
	}
	if (carry > 0) {
		result.push_back(carry);
	}
	trim_data(result);
	return result;
}

datavec & sum(datavec & lhs, datavec const & rhs) { // если можно мен€ть старую пам€ть, экономим на allocation

	lhs.reserve(std::max(lhs.size(), rhs.size()) + 1);
	xint carry = 0;
	for (size_t i = 0; i < std::min(lhs.size(), rhs.size()); i++) {
		carry = carry + lhs[i] + rhs[i];
		lhs[i] = (carry & bitmask32);
		carry >>= 32;
	}
	if (lhs.size() < rhs.size()) {
		for (size_t i = lhs.size(); i < rhs.size(); i++) {
			carry = carry + rhs[i];
			lhs.push_back(carry & bitmask32);
			carry >>= 32;
		}
	} else {
		for (size_t i = rhs.size(); i < lhs.size(); i++) {
			carry = carry + lhs[i];
			lhs[i] = (carry & bitmask32);
			carry >>= 32;
		}
	}
	if (carry > 0) {
		lhs.push_back(carry);
	}
	return lhs;
}

datavec & sum(datavec & lhs, datavec & rhs) { // если можно мен€ть старую пам€ть, экономим на allocation
	if (lhs.size() < rhs.size()) {
		swap(lhs, rhs);
	}
	xint carry = 0;
	for (size_t i = 0; i < rhs.size(); i++) {
		carry = carry + lhs[i] + rhs[i];
		lhs[i] = carry & bitmask32;
		carry >>= 32;
	}
	for (size_t i = rhs.size(); i < lhs.size() && carry != 0; i++) {
		carry = carry + lhs[i];
		lhs[i] = (carry & bitmask32);
		carry >>= 32;
	}
	if (carry > 0) {
		lhs.push_back(carry);
	}
	trim_data(lhs);
	return lhs;
}

bint compare(datavec const & lhs, datavec const & rhs) { // как Java
	if (lhs.size() != rhs.size()) {
		return lhs.size() > rhs.size() ? 1 : -1;
	}
	if (lhs.size() == 0)
		return 0;
	size_t index = lhs.size();
	do {
		index--;
		if (lhs[index] != rhs[index]) {
			return lhs[index] > rhs[index] ? 1 : -1;
		}
	} while (index > 0);
	return 0;
}

datavec subtract(datavec const & lhs, datavec const & rhs) { // lhs >= rhs;
	assert(compare(lhs, rhs) >= 0);
	datavec result;
	result.reserve(lhs.size());
	xint carry = 0; // знаковое
	for (size_t i = 0; i < rhs.size(); i++) {
		carry = carry + lhs[i] - rhs[i];
		result.push_back(carry & bitmask32);
		carry >>= 32;
	}
	for (size_t i = rhs.size(); i < lhs.size(); i++) {
		carry = carry + lhs[i];
		result.push_back(carry & bitmask32);
		carry >>= 32;
	}
	if (carry > 0) {
		result.push_back(carry);
	}
	trim_data(result);
	return result;
}

datavec multiply(datavec const & lhs, eint mul) {
	datavec result;
	result.reserve(lhs.size() + 1);
	uxint carry = 0;
	for (size_t i = 0; i < lhs.size(); i++) {
		carry += uxint(lhs[i]) * mul;
		result.push_back(bitmask32 & carry);
		carry >>= 32;
	}
	if (carry > 0) {
		result.push_back(carry);
	}
	trim_data(result);
	return result;
}

datavec multiply_simple(datavec const & lhs, datavec const & rhs) {
	datavec result;
	for (size_t st = 0; st < rhs.size(); st++) {
		datavec res_a = multiply(lhs, rhs[st]);
		datavec res_b = lshift(res_a, st * 32);
		datavec res_c = sum(result, res_b);
		result = res_c;
//		result = sum(result, lshift(multiply(lhs, rhs[st]), st * 32));
	}
	trim_data(result);
	return result;
}

datavec multiply_karatsuba(datavec const & lhs, datavec const & rhs) {
	if (std::min(lhs.size(), rhs.size()) <= 100) {
		return multiply_simple(lhs, rhs);
	}
	if (lhs.size() < rhs.size()) {
		return multiply_karatsuba(rhs, lhs);
	}
	//assert(lhs.size() >= rhs.size());
	size_t extend_to = lhs.size() >> 1;
	if (extend_to >= rhs.size()) {
		datavec hpart(lhs.begin() + extend_to, lhs.end());
		datavec lpart(lhs.begin(), lhs.begin() + extend_to);
		return sum(lshift(multiply_karatsuba(hpart, rhs), extend_to * 32), multiply_karatsuba(lpart, rhs));
	}
	datavec hpart_l(lhs.begin() + extend_to, lhs.end());
	datavec lpart_l(lhs.begin(), lhs.begin() + extend_to);
	datavec hpart_r(rhs.begin() + extend_to, rhs.end());
	datavec lpart_r(rhs.begin(), rhs.begin() + extend_to);
	trim_data(hpart_l);
	trim_data(lpart_l);
	trim_data(hpart_r);
	trim_data(lpart_r);
	datavec prod1 = multiply_karatsuba(hpart_l, hpart_r);
	datavec prod2 = multiply_karatsuba(lpart_l, lpart_r);
	datavec prod3 = multiply_karatsuba(sum(hpart_l, lpart_l), sum(hpart_r, lpart_r));
	datavec res = sum(lshift(prod1, extend_to * 64), prod2);
	return sum(res, lshift(subtract(prod3, sum(prod1, prod2)), extend_to * 32));
}

datavec multiply_switch(datavec const & lhs, datavec const & rhs) { // выбирает метод и исполн€ет
	datavec res;
	res = multiply_karatsuba(rhs, lhs);
	trim_data(res);
	return res;
}

dpair divide(datavec const & lhs, eint divs) { // считаем eint < 2^31
	datavec first;
	lldiv_t carry;
	if (lhs.back() >= divs) {
		first.resize(lhs.size());
		carry = lldiv(lhs.back(), divs);
		first.back() = carry.quot;
	} else {
		first.resize(std::max(size_t(1), lhs.size() - 1));
		carry = lldiv(lhs.back(), divs);
	}
	for (xint i = (xint) lhs.size() - 2; i >= 0; i--) {
		carry = lldiv((carry.rem << 32) + lhs[i], divs);
		first[i] = carry.quot;
	}
	trim_data(first);
	return {first, (eint)carry.rem};
}

std::pair<datavec, datavec> unsigned_divide_nw(datavec const & lhs, datavec const & rhs) {
	int normalizer = __builtin_clz(rhs.back());
	datavec dd = lshift(lhs, normalizer);
	datavec dv = lshift(rhs, normalizer);
	if (dv.size() > dd.size()) {
		return {datavec(1, 0), lhs};
	}
	datavec res;
	res.resize(dd.size() - dv.size() + 1);
	if (dd.back() > dv.back()
			|| (dd.back() == dv.back() && compare(dd, lshift(dv, 32 * (dd.size() - dv.size()))) >= 0)) {
		res.back() = 1;
		dd = subtract(dd, lshift(dv, 32 * (dd.size() - dv.size())));
	}
	for (xint res_pl = xint(dd.size()) - dv.size() - 1; res_pl >= 0; res_pl--) {
		uxint cand;
		if (dd.size() <= dv.size() + res_pl) {
			continue;
		}
		if (dd.size() > 1) {
			cand = (uxint(dd.back()) << 32) + uxint(dd[dd.size() - 1]);
		} else {
			cand = dd.back();
		}
		uxint cand_div = (cand / dv.back()) + 2;
		while (compare(dd, lshift(multiply(dv, cand_div), 32 * (dd.size() - dv.size() - 1))) < 0) {
			cand_div--;
		}
		res[res_pl] = cand_div;
		dd = subtract(dd, lshift(multiply(dv, cand_div), 32 * (dd.size() - dv.size() - 1)));
	}
	trim_data(res);
	return {res, rshift(dd, normalizer, false)};
}

std::pair<datavec, datavec> unsigned_divide(datavec const & lhs, datavec const & rhs) {
	return unsigned_divide_nw(lhs, rhs);
}

datavec to_complement(big_integer const & src) {
	datavec rs;
	rs.reserve(src.data.size() + 1);
	rs.assign(src.data.begin(), src.data.end());
	if (src.sign) {
		for (size_t i = 0; i < rs.size(); i++) {
			rs[i] ^= bitmask32;
		}
		rs = sum(rs, datavec(1, 1)); // короче с утра разберусь. Ќадеюсь(((
	}
	rs.push_back(eint(0 - src.sign));
	return rs;
}

datavec bitop(datavec const & lhs, datavec const & rhs, const eint trans) { // trans - 0-15
	if (lhs.size() < rhs.size()) {
		return bitop(rhs, lhs, trans);
	}
	datavec res(lhs.size(), 0);
	for (size_t i = 0; i < rhs.size(); i++) {
		for (int j = 0; j < 32; j++) {
			int now_trans_pos = 2 * ((lhs[i] >> j) & 1) + ((rhs[i] >> j) & 1);
			res[i] += ((trans >> now_trans_pos) & 1) << j;
		}
	}
	for (size_t i = rhs.size(); i < lhs.size(); i++) {
		for (int j = 0; j < 32; j++) {
			int now_trans_pos = 2 * ((lhs[i] >> j) & 1) + ((rhs.back() >> 31) & 1);
			res[i] += ((trans >> now_trans_pos) & 1) << j;
		}
	}
	return res;
}

big_integer from_complement(datavec const & data) {
	big_integer res(0);
	res.data = data;
	if (res.data.back() & (1 << 31)) {
		for (size_t i = 0; i < res.data.size(); i++) {
			res.data[i] ^= bitmask32;
		}
		res.data = sum(res.data, datavec(1, 1));
		res.sign = true;
	}
	trim_data(res.data);
	return res;
}

big_integer::big_integer() {
	*this = big_integer(0);
}

big_integer::big_integer(big_integer const& other) {
	data = other.data;
	sign = other.sign;
}

bool big_integer::is_zero() const {
	return this->data.size() == 1 && this->data[0] == 0;
}

/*big_integer::big_integer(big_integer & other) {
 swap(data, other.data);
 sign = other.sign;
 }*/

big_integer::big_integer(datavec new_data) {
	swap(data, new_data);
	sign = false;
}

big_integer::big_integer(int a) {
	xint al = a;
	sign = (al < 0);
	data.assign(1, std::llabs(al) & bitmask32);
}

big_integer::big_integer(std::string const& str) {
	std::string str2 = str;
	// TODO : переработать, меньше умножений?
	if (str.empty()) {
		*this = big_integer(0);
	} else {
		std::vector<bint> vc;
		vc.reserve(str.size());
		for (size_t i = str.size() - 1; i > 0; i--) {
			assert(isdigit(str[i]));
			vc.push_back(str[i] - '0');
		}
		if (isdigit(str[0])) {
			vc.push_back(str[0] - '0');
			sign = false;
		} else {
			assert(str[0] == '-');
			sign = true;
		}
//		reverse(vc.begin(), vc.end());
		const int POWT = 1000000000;
		lldiv_t portions = lldiv(vc.size(), 9);
		int carry(0);
		for (int i = 0; i < portions.rem; i++) {
			carry = carry * 10 + vc.back();
			vc.pop_back();
		}
		big_integer result(carry);
		for (int iter = 0; iter < portions.quot; iter++) {
			carry = 0;
			for (int i = 0; i < 9; i++) {
				carry = carry * 10 + vc.back();
				vc.pop_back();
			}
			result *= POWT;
			result += carry;
		}
		swap(data, result.data);
	}
	trim_data(data);
}

/*big_integer & big_integer::operator =(big_integer & other) {
 swap(data, other.data);
 sign = other.sign;
 return *this;
 }*/

big_integer & big_integer::operator =(big_integer const & other) {
	data = other.data;
	sign = other.sign;
	return *this;
}

big_integer & big_integer::operator +=(big_integer const & rhs) {
//cdb << "BEFORE " << *this << "\n";
//cdb << "AFTER  " << from_complement(to_complement(*this)) << "\n";
	if (this->sign != rhs.sign) {
		this->sign = rhs.sign;
		this->operator -=(rhs);
		this->sign = !(this->is_zero() || this->sign);
		return *this;
	}
	this->data = sum(this->data, rhs.data);
//cdb << "FYI RESULT IS " << *this << "\n";
	return *this;
}

big_integer & big_integer::operator -=(big_integer const & rhs) {
	if (this->sign != rhs.sign) {
		this->sign = rhs.sign;
		this->operator +=(rhs);
		this->sign = !(this->is_zero() || this->sign);
		return *this;
	}
	if (compare(this->data, rhs.data) < 0) {
		this->data = subtract(rhs.data, this->data);
		this->sign = !(this->is_zero() || this->sign);
	} else {
		this->data = subtract(this->data, rhs.data);
	}
//cdb << "SUB RESULT IS " << *this << "\n";
	return *this;
}

big_integer & big_integer::operator *=(big_integer const & rhs) {
	this->data = multiply_switch(this->data, rhs.data);
	this->sign = this->sign ^ rhs.sign;
	return *this;
}

big_integer & big_integer::operator /=(big_integer const & rhs) {
	if (rhs.is_zero()) {
		int num_inf = 1 / rhs.data.back();
		*this = big_integer(num_inf);
		return *this;
	}
//cdb << "DIVIDING " << *this << "\nDIVISOR  " << rhs << "\n";
	auto rs = unsigned_divide(this->data, rhs.data);
	swap(this->data, rs.first);
	this->sign = this->sign ^ rhs.sign;
	return *this;
}

big_integer & big_integer::operator %=(big_integer const & rhs) {
	if (rhs.is_zero()) {
		int num_inf = 1 / rhs.data.back();
		*this = big_integer(num_inf);
		return *this;
	}
	auto rs = unsigned_divide(this->data, rhs.data);
	swap(this->data, rs.second);
	return *this;
}

big_integer & big_integer::operator &=(big_integer const & rhs) {
	datavec a = to_complement(*this), b = to_complement(rhs);
	a = bitop(a, b, 8);
//	cdb << "AND ON " << *this << "\nAND ON " << rhs;
	*this = from_complement(a);
//	cdb << "\nGETS   " << *this << "\n";
	return *this;
}

big_integer & big_integer::operator |=(big_integer const & rhs) {
	datavec a = to_complement(*this), b = to_complement(rhs);
	a = bitop(a, b, 14);
	*this = from_complement(a);
	return *this;
}

big_integer & big_integer::operator ^=(big_integer const & rhs) {
	datavec a = to_complement(*this), b = to_complement(rhs);
	a = bitop(a, b, 6);
	*this = from_complement(a);
	return *this;
}

big_integer & big_integer::operator <<=(int rhs) {
	if (rhs < 0) {
		this->data = rshift(this->data, -xint(rhs), this->sign);
	} else {
		this->data = lshift(this->data, rhs);
	}
	return *this;
}

big_integer & big_integer::operator >>=(int rhs) {
	if (rhs < 0) {
		this->data = lshift(this->data, -xint(rhs));
	} else {
		this->data = rshift(this->data, rhs, this->sign);
		if (this->data.size() == 1 && this->sign && this->data[0] == 0) {
			this->data[0] = -1;
		}
	}
	return *this;
}

big_integer big_integer::operator +() const {
	return *this;
}

big_integer big_integer::operator -() const {
	big_integer ret = big_integer(*this);
	ret.sign ^= 1;
	return ret;
}

big_integer big_integer::operator ~() const {
	big_integer ret = big_integer(*this);
	++ret;
	ret.sign ^= 1;
	return ret;
}

big_integer & big_integer::operator ++() {
	(*this) += big_integer(1);
	return *this;
}

big_integer big_integer::operator++(int) {
	big_integer ret = *this;
	++*this;
	return ret;
}

big_integer & big_integer::operator --() {
	(*this) -= big_integer(1);
	return *this;
}

big_integer big_integer::operator--(int) {
	big_integer r = *this;
	--*this;
	return r;
}

big_integer operator+(big_integer a, big_integer const& b) {
	return a += b;
}

big_integer operator-(big_integer a, big_integer const& b) {
	return a -= b;
}

big_integer operator*(big_integer a, big_integer const& b) {
	return a *= b;
}

big_integer operator/(big_integer a, big_integer const& b) {
	return a /= b;
}

big_integer operator%(big_integer a, big_integer const& b) {
	return a %= b;
}

big_integer operator&(big_integer a, big_integer const& b) {
	return a &= b;
}

big_integer operator|(big_integer a, big_integer const& b) {
	return a |= b;
}

big_integer operator^(big_integer a, big_integer const& b) {
	return a ^= b;
}

big_integer operator<<(big_integer a, int b) {
	return a <<= b;
}

big_integer operator>>(big_integer a, int b) {
	return a >>= b;
}

bint compare_signed(big_integer const& a, big_integer const& b) {
	if (a.is_zero() || b.is_zero()) {
		if (a.is_zero() && b.is_zero()) {
			return 0;
		} else {
			if ((!a.is_zero() && a.sign) || (!b.is_zero() && !b.sign)) {
				return -1;
			} else {
				return 1;
			}
		}
	} else {
		if (a.sign != b.sign) {
			return a.sign ? 1 : -1;
		} else {
			return compare(a.data, b.data) * (a.sign ? -1 : 1);
		}
	}
}

bool operator==(big_integer const& a, big_integer const& b) {
	return compare_signed(a, b) == 0;
}

bool operator!=(big_integer const& a, big_integer const& b) {
	return compare_signed(a, b) != 0;
}

bool operator<(big_integer const& a, big_integer const& b) {
	return compare_signed(a, b) < 0;
}

bool operator>(big_integer const& a, big_integer const& b) {
	return compare_signed(a, b) > 0;
}

bool operator<=(big_integer const& a, big_integer const& b) {
	return compare_signed(a, b) <= 0;
}

bool operator>=(big_integer const& a, big_integer const& b) {
	return compare_signed(a, b) >= 0;
}

std::string to_string(big_integer const& a) {
	std::string res;
	const int POWT = 1000000000;
	if (a.is_zero()) {
		return std::string("0");
	}
	dpair tmp = { a.data, 0 };
	while (tmp.first.size() > 1 || tmp.first.front() > 0) {
		tmp = divide(tmp.first, POWT);
		div_t temp_z = div(tmp.second, 10);
		if (tmp.first.size() > 1 || tmp.first.front() > 0) {
			for (int i = 0; i < 9; i++) {
				res.push_back('0' + temp_z.rem);
				temp_z = div(temp_z.quot, 10);
			}
		} else {
			do {
				res.push_back('0' + temp_z.rem);
				temp_z = div(temp_z.quot, 10);
			} while (temp_z.quot > 0 || temp_z.rem > 0);
		}
	}
	if (a.sign) {
		res.push_back('-');
	}
	std::reverse(res.begin(), res.end());
	return res;
}

std::ostream& operator<<(std::ostream& s, big_integer const& a) {
	return s << to_string(a);
}
