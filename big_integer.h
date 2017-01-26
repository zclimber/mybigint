#ifndef GNU_BIG_INTEGER_H
#define GNU_BIG_INTEGER_H

#include <cstddef>
#include <iosfwd>
#include <vector>
#include <memory>

#include <cassert>

typedef unsigned int eint; // extended unsigned int
typedef unsigned long long uxint; // extra unsigned int - 64 bits
typedef long long xint; // extra int - 64 bits
typedef signed char bint; // byte int

typedef std::vector<eint> datavec;

struct num_storage {
	datavec data;
	const datavec & get_readable() const;
	datavec & get_assignable();
	datavec & get_modifiable();
	datavec get_copy() const;
	datavec & operator()() {
		return data;
	}
	const datavec & operator()() const {
		return data;
	}
};

struct big_integer {
	big_integer(); //
	big_integer(big_integer const& other); //
//    big_integer(big_integer & other); //
	big_integer(int a); //
	explicit big_integer(std::string const& str); //
//    ~big_integer(); // автосгенерированный пойдет

	big_integer& operator=(big_integer const& other); //
//    big_integer& operator=(big_integer & other); //

	big_integer& operator+=(big_integer const& rhs); //
	big_integer& operator-=(big_integer const& rhs); //
	big_integer& operator*=(big_integer const& rhs); //
	big_integer& operator/=(big_integer const& rhs);
	big_integer& operator%=(big_integer const& rhs);

	big_integer& operator&=(big_integer const& rhs);
	big_integer& operator|=(big_integer const& rhs);
	big_integer& operator^=(big_integer const& rhs);

	big_integer& operator<<=(int rhs); //
	big_integer& operator>>=(int rhs); //

	big_integer operator+() const; //
	big_integer operator-() const; //
	big_integer operator~() const; //

	big_integer& operator++(); //
	big_integer operator++(int); //

	big_integer& operator--(); //
	big_integer operator--(int); //

	bool is_zero() const;

	friend bool operator==(big_integer const& a, big_integer const& b);
	friend bool operator!=(big_integer const& a, big_integer const& b);
	friend bool operator<(big_integer const& a, big_integer const& b);
	friend bool operator>(big_integer const& a, big_integer const& b);
	friend bool operator<=(big_integer const& a, big_integer const& b);
	friend bool operator>=(big_integer const& a, big_integer const& b);

	friend std::string to_string(big_integer const& a);

	friend datavec to_complement(big_integer const & src);
	friend big_integer from_complement(datavec const & data);

private:
	friend signed char compare_signed(big_integer const& a, big_integer const& b);
	datavec data;
	big_integer(datavec data);
	bool sign;
};

big_integer operator+(big_integer a, big_integer const& b);
big_integer operator-(big_integer a, big_integer const& b);
big_integer operator*(big_integer a, big_integer const& b);
big_integer operator/(big_integer a, big_integer const& b);
big_integer operator%(big_integer a, big_integer const& b);

big_integer operator&(big_integer a, big_integer const& b);
big_integer operator|(big_integer a, big_integer const& b);
big_integer operator^(big_integer a, big_integer const& b);

big_integer operator<<(big_integer a, int b);
big_integer operator>>(big_integer a, int b);

bool operator==(big_integer const& a, big_integer const& b);
bool operator!=(big_integer const& a, big_integer const& b);
bool operator<(big_integer const& a, big_integer const& b);
bool operator>(big_integer const& a, big_integer const& b);
bool operator<=(big_integer const& a, big_integer const& b);
bool operator>=(big_integer const& a, big_integer const& b);

std::string to_string(big_integer const& a);
std::ostream& operator<<(std::ostream& s, big_integer const& a);

#endif // BIG_INTEGER_H
