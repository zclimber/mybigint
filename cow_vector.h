/*
 * soo_cow_vector.h
 *
 *  Created on: 7 апр. 2017 г.
 *      Author: ME
 */

#ifndef SOO_COW_VECTOR_CPP_
#define SOO_COW_VECTOR_CPP_

#include <iterator>

using std::size_t;

template<typename T>
struct data_storage {
	int use_count;
	size_t current_size;
	size_t capacity;
	T * storage;
};

template<typename T, size_t local_space>
class cow_vector {
	union {
		data_storage<T> * data;
		T local_data[local_space];
	};
	bool is_local;
public:
	cow_vector();
	cow_vector(size_t size);
	cow_vector(size_t size, const T& value);
	cow_vector(const cow_vector & other);
	cow_vector(cow_vector && other);

	~cow_vector();

	cow_vector& operator=(const cow_vector & other);
	cow_vector& operator=(cow_vector && other);

	void assign(size_t count, const T& value);
	template<class InputIt>
	void assign(InputIt first, InputIt last);

	T& operator [](size_t _n);
	const T& operator [](size_t _n) const;
	T& front();
	const T& front() const;
	T& back();
	const T& back() const;

	typedef __gnu_cxx:: __normal_iterator <T*, cow_vector> iterator;
	typedef __gnu_cxx::__normal_iterator<const T*, cow_vector> const_iterator;

	iterator begin();
	const_iterator begin() const;
	const_iterator cbegin() const;
	iterator end();
	const_iterator end() const;
	const_iterator cend() const;

	bool empty() const;
	size_t size() const;
	size_t capacity() const;
	void reserve(size_t new_capacity);

	void clear();
	iterator insert( const_iterator pos, const T& value );
	iterator insert( const_iterator pos, T&& value );
	iterator insert( const_iterator pos, size_t count, const T& value );
	template< class InputIt >
	iterator insert( const_iterator pos, InputIt first, InputIt last );
	void append(size_t count, const T& value);
	template< class InputIt >
	void append(InputIt first, InputIt last );

	void push_back( const T& value );
	void push_back( T&& value );
	void pop_back();

	void resize( size_t count );
	void resize( size_t count, const T& value );
	void swap( cow_vector& other );

};
template<class T, size_t local_space>
void swap(cow_vector<T, local_space>& lhs, cow_vector<T, local_space>& rhs);

#endif /* SOO_COW_VECTOR_CPP_ */
