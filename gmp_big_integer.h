#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <cstddef>
#include <gmp.h>
#include <iosfwd>

struct gmp_big_integer {
	gmp_big_integer();
	gmp_big_integer(gmp_big_integer const& other);
	gmp_big_integer(int a);
	explicit gmp_big_integer(std::string const& str);
	~gmp_big_integer();

	gmp_big_integer& operator=(gmp_big_integer const& other);

	gmp_big_integer& operator+=(gmp_big_integer const& rhs);
	gmp_big_integer& operator-=(gmp_big_integer const& rhs);
	gmp_big_integer& operator*=(gmp_big_integer const& rhs);
	gmp_big_integer& operator/=(gmp_big_integer const& rhs);
	gmp_big_integer& operator%=(gmp_big_integer const& rhs);

	gmp_big_integer& operator&=(gmp_big_integer const& rhs);
	gmp_big_integer& operator|=(gmp_big_integer const& rhs);
	gmp_big_integer& operator^=(gmp_big_integer const& rhs);

	gmp_big_integer& operator<<=(int rhs);
	gmp_big_integer& operator>>=(int rhs);

	gmp_big_integer operator+() const;
	gmp_big_integer operator-() const;
	gmp_big_integer operator~() const;

	gmp_big_integer& operator++();
	gmp_big_integer operator++(int);

	gmp_big_integer& operator--();
	gmp_big_integer operator--(int);

	friend bool operator==(gmp_big_integer const& a, gmp_big_integer const& b);
	friend bool operator!=(gmp_big_integer const& a, gmp_big_integer const& b);
	friend bool operator<(gmp_big_integer const& a, gmp_big_integer const& b);
	friend bool operator>(gmp_big_integer const& a, gmp_big_integer const& b);
	friend bool operator<=(gmp_big_integer const& a, gmp_big_integer const& b);
	friend bool operator>=(gmp_big_integer const& a, gmp_big_integer const& b);

	friend std::string to_string(gmp_big_integer const& a);

private:
	mpz_t mpz;
};

gmp_big_integer operator+(gmp_big_integer a, gmp_big_integer const& b);
gmp_big_integer operator-(gmp_big_integer a, gmp_big_integer const& b);
gmp_big_integer operator*(gmp_big_integer a, gmp_big_integer const& b);
gmp_big_integer operator/(gmp_big_integer a, gmp_big_integer const& b);
gmp_big_integer operator%(gmp_big_integer a, gmp_big_integer const& b);

gmp_big_integer operator&(gmp_big_integer a, gmp_big_integer const& b);
gmp_big_integer operator|(gmp_big_integer a, gmp_big_integer const& b);
gmp_big_integer operator^(gmp_big_integer a, gmp_big_integer const& b);

gmp_big_integer operator<<(gmp_big_integer a, int b);
gmp_big_integer operator>>(gmp_big_integer a, int b);

bool operator==(gmp_big_integer const& a, gmp_big_integer const& b);
bool operator!=(gmp_big_integer const& a, gmp_big_integer const& b);
bool operator<(gmp_big_integer const& a, gmp_big_integer const& b);
bool operator>(gmp_big_integer const& a, gmp_big_integer const& b);
bool operator<=(gmp_big_integer const& a, gmp_big_integer const& b);
bool operator>=(gmp_big_integer const& a, gmp_big_integer const& b);

std::string to_string(gmp_big_integer const& a);
std::ostream& operator<<(std::ostream& s, gmp_big_integer const& a);

#endif // BIG_INTEGER_H
