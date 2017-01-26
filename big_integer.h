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

//#define DATAVEC_VECTOR

#ifdef DATAVEC_VECTOR

typedef std::vector<eint> datavec;

#else

struct datavec {
	std::vector<eint> storage;
	datavec();
	datavec(const datavec & dat);
	datavec(size_t size, eint value);
	datavec(const eint * start, const eint * finish);
	datavec& operator=(const datavec& dat);
	void assign(size_t size, eint value);
	void assign(const eint * start, const eint * finish);
	~datavec();
	void externalize();
	eint& back();
	const eint& back() const;
	eint& front();
	const eint& front() const;
	eint & operator[](size_t __n);
	const eint & operator[](size_t __n) const;
	void pop_back();
	void push_back(eint value);
	void append(const eint * start, const eint * finish);
	void reserve(size_t new_capacity);
	void resize(size_t new_size);
	void resize(size_t new_size, eint new_value);
	void shrink_to_fit();
	const eint * begin() const;
	const eint * end() const;
	size_t size() const;
	size_t capacity() const;
	friend void swap(datavec &a, datavec &b);
};

void swap(datavec &a, datavec &b);

#endif

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
