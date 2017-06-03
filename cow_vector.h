/*
 * soo_cow_vector.h
 *
 *  Created on: 7 апр. 2017 г.
 *      Author: ME
 */

#ifndef SOO_COW_VECTOR_CPP_
#define SOO_COW_VECTOR_CPP_

#include <iterator>
#include <type_traits>

#include <cassert>

using std::size_t;

static const unsigned int EMPTY = 777777777;

template<typename T, unsigned int local_space>
class cow_vector {

	void assert_correctness() const {
		assert(local_size == REMOTE || local_size <= local_space);
		if (local_size == REMOTE) {
			assert(rem_data.use_count() >= 1);
		} else {
			assert(rem_data.use_count() == 0);
		}
	}

public:
	bool compare(const std::vector<T> & other) const {
		assert_correctness();
		if (is_local()) {
			if (other.size() != local_size) {
				return false;
			}
			return std::equal(local_data, local_data + local_size, other.begin());
		} else {
			return (*rem_data == other);
		}
	}

	bool same_remote(const cow_vector<T, local_space> & other) const {
		return (rem_data.use_count() == 0 && other.rem_data.use_count() == 0) || rem_data == other.rem_data;
	}
private:
	std::shared_ptr<std::vector<T>> rem_data;
	T local_data[local_space];
	unsigned int local_size;
	static const unsigned int REMOTE = (unsigned int) -1;
public:
	bool is_local() const {

		return local_size != REMOTE;
	}
private:
	void unlink() {
		if (!is_local() && rem_data.use_count() > 1) {
			rem_data = std::make_shared<std::vector<T>>(*rem_data);
		}
	}
	void make_remote() {
		if (is_local()) {
			rem_data = std::make_shared<std::vector<T>>(local_data, local_data + local_size);
			local_size = REMOTE;
		}
	}
	void try_make_local(unsigned int reserve = local_space / 2) {
		if (!is_local() && size() + reserve <= local_space) {
			local_size = size();
			std::move(rem_data->begin(), rem_data->end(), local_data);
			rem_data.reset();
		}
	}
//	};
public:
	cow_vector();
	cow_vector(size_t size);
	cow_vector(size_t size, const T& value);
	cow_vector(const cow_vector & other);
	cow_vector(cow_vector && other);

	template<class InputIt,
			typename = typename std::enable_if<
					std::is_base_of<std::input_iterator_tag, typename std::iterator_traits<InputIt>::iterator_category>::value>::type>
	cow_vector(InputIt first, InputIt last) :
			cow_vector() {
		assign(first, last);
	}

	~cow_vector() {
	}

	cow_vector& operator=(const cow_vector & other);
	cow_vector& operator=(cow_vector && other);

	void assign(size_t count, const T& value);
	template<class InputIt,
			typename = typename std::enable_if<
					std::is_base_of<std::input_iterator_tag, typename std::iterator_traits<InputIt>::iterator_category>::value>::type>
	void assign(InputIt first, InputIt last) {
		local_size = 0;

		rem_data.reset();
		while (first != last && local_size < local_space) {
			local_data[local_size] = *first;
			first++;
			local_size++;
		}
		if (first != last) {
			make_remote();
			rem_data->insert(rem_data->end(), first, last);
		}
	}

	T& operator [](size_t _n);
	const T& operator [](size_t _n) const;
	T& front();
	const T& front() const;
	T& back();
	const T& back() const;

	typedef T* pointer;
	typedef __gnu_cxx:: __normal_iterator <T*, cow_vector> iterator;
	typedef __gnu_cxx:: __normal_iterator< const T*, cow_vector> const_iterator;

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
	void shrink_to_fit();

	void clear();
	iterator insert(const_iterator pos, const T& value);
	iterator insert(const_iterator pos, T&& value);
	iterator insert(const_iterator pos, size_t count, const T& value);
	template<class InputIt,
	typename = typename std::enable_if<
	std::is_base_of<std::input_iterator_tag, typename std::iterator_traits<InputIt>::iterator_category>::value>::type>
	iterator insert(const_iterator pos, InputIt first, InputIt last) {
		assert(pos == cend());
		if(first == last) {
			return end();
		}
		if(is_local()) {
			int p = local_size;
			for(; local_size < local_space && first != last;) {
				local_data[local_size++] = *(first++);
			}
			if(first != last) {
				make_remote();
				rem_data->insert(rem_data->end(), first, last);
				return iterator(&*(rem_data->begin() + p));
			} else {
				return iterator(local_data + p);
			}
		} else {
//			return iterator(&*rem_data->insert(rem_data->begin() + (pos - begin()), 0, 0));
			return iterator(&*(rem_data->insert(rem_data->begin() + (pos - begin()), first, last)));
		}
	}
	void append(size_t count, const T& value);
	template<class InputIt,
	typename = typename std::enable_if<
	std::is_base_of<std::input_iterator_tag, typename std::iterator_traits<InputIt>::iterator_category>::value>::type>
	void append(InputIt first, InputIt last) {
		if(is_local()) {
			for(; local_size < local_space && first != last;) {
				local_data[local_size++] = *(first++);
			}
			if(first != last) {
				make_remote();
				rem_data->insert(rem_data->end(), first, last);
			}
		} else {
			rem_data->insert(rem_data->end(), first, last);
		}
	}

	void push_back(const T& value);
	void push_back(T&& value);
	void pop_back();

	void resize(size_t count);
	void resize(size_t count, const T& value);
	void swap(cow_vector& other);

};
template<class T, size_t local_space>
void swap(cow_vector<T, local_space>& lhs, cow_vector<T, local_space>& rhs);

template<typename T, unsigned int local_space>
inline cow_vector<T, local_space>::cow_vector() {
	local_size = 0;

}

template<typename T, unsigned int local_space>
inline cow_vector<T, local_space>::cow_vector(size_t size) {
	if (size <= local_space) {
		local_size = size;
		std::fill(local_data, local_data + size, T());
	} else {
		local_size = -1;
		rem_data = std::make_shared<std::vector<T>>(size);
	}

}

template<typename T, unsigned int local_space>
inline cow_vector<T, local_space>::cow_vector(size_t size, const T& value) {
	if (size <= local_space) {
		local_size = size;
		std::fill(local_data, local_data + size, value);
	} else {
		local_size = -1;
		rem_data = std::make_shared<std::vector<T>>(size, value);
	}

}

template<typename T, unsigned int local_space>
inline cow_vector<T, local_space>::cow_vector(const cow_vector& other) {
	local_size = other.local_size;
	if (other.is_local()) {
		std::copy(other.local_data, other.local_data + other.local_size, local_data);
	} else {
		rem_data = other.rem_data;
	}

}

template<typename T, unsigned int local_space>
inline cow_vector<T, local_space>::cow_vector(cow_vector&& other) {
	local_size = other.local_size;
	if (other.is_local()) {
		std::copy(other.local_data, other.local_data + other.local_size, local_data);
	} else {
		rem_data = other.rem_data;
	}

}

typedef cow_vector<unsigned int, 4> cw;

template<typename T, unsigned int local_space>
inline cow_vector<T, local_space>& cow_vector<T, local_space>::operator =(const cow_vector<T, local_space>& other) {
	local_size = other.local_size;
	if (other.is_local()) {
		std::copy(other.local_data, other.local_data + local_size, local_data);
		rem_data.reset();
	} else {
		rem_data = other.rem_data;
	}

	return *this;
}

template<typename T, unsigned int local_space>
inline cow_vector<T, local_space>& cow_vector<T, local_space>::operator =(cow_vector<T, local_space> && other) {
	local_size = other.local_size;
	rem_data.reset();
	if (other.is_local()) {
		std::copy(other.local_data, other.local_data + local_size, local_data);
	} else {
		rem_data = other.rem_data;
	}

	return *this;
}

template<typename T, unsigned int local_space>
inline void cow_vector<T, local_space>::assign(size_t count, const T& value) {
	if (count <= local_space) {
		local_size = count;
		std::fill(local_data, local_data + count, value);
		rem_data = std::shared_ptr<std::vector<T>>();
	} else {
		local_size = -1;
		rem_data = std::make_shared<std::vector<T>>(count, value);
	}

}

template<typename T, unsigned int local_space>
inline T& cow_vector<T, local_space>::operator [](size_t _n) {
	unlink();

	if (is_local()) {
		return local_data[_n];
	} else {
		return rem_data->at(_n);
	}
}

template<typename T, unsigned int local_space>
inline const T& cow_vector<T, local_space>::operator [](size_t _n) const {
	if (is_local()) {
		return local_data[_n];
	} else {
		return rem_data->at(_n);
	}
}

template<typename T, unsigned int local_space>
inline T& cow_vector<T, local_space>::front() {
	unlink();

	if (is_local()) {
		return local_data[0];
	} else {
		return rem_data->front();
	}
}

template<typename T, unsigned int local_space>
inline const T& cow_vector<T, local_space>::front() const {

	if (is_local()) {
		return local_data[0];
	} else {
		return rem_data->front();
	}
}

template<typename T, unsigned int local_space>
inline T& cow_vector<T, local_space>::back() {
	unlink();

	if (is_local()) {
		return local_data[local_size - 1];
	} else {
		return rem_data->back();
	}
}

template<typename T, unsigned int local_space>
inline const T& cow_vector<T, local_space>::back() const {
	if (is_local()) {
		return local_data[local_size - 1];
	} else {
		return rem_data->back();
	}
}

template<typename T, unsigned int local_space>
inline typename cow_vector<T, local_space>::iterator cow_vector<T, local_space>::begin() {
	unlink();
	if (is_local()) {
		return iterator(local_data);
	} else {
		return iterator(&*rem_data->begin());
	}
}

template<typename T, unsigned int local_space>
inline typename cow_vector<T, local_space>::const_iterator cow_vector<T, local_space>::begin() const {
	if (is_local()) {
		return const_iterator(local_data);
	} else {
		return const_iterator(&*rem_data->cbegin());
	}
}

template<typename T, unsigned int local_space>
inline typename cow_vector<T, local_space>::const_iterator cow_vector<T, local_space>::cbegin() const {
	if (is_local()) {
		return const_iterator(local_data);
	} else {
		return const_iterator(&*rem_data->cbegin());
	}
}

template<typename T, unsigned int local_space>
inline typename cow_vector<T, local_space>::iterator cow_vector<T, local_space>::end() {
	unlink();
	if (is_local()) {
		return iterator(local_data + local_size);
	} else {
		return iterator(&*rem_data->end());
	}
}

template<typename T, unsigned int local_space>
inline typename cow_vector<T, local_space>::const_iterator cow_vector<T, local_space>::end() const {
	if (is_local()) {
		return const_iterator(local_data + local_size);
	} else {
		return const_iterator(&*rem_data->cend());
	}
}

template<typename T, unsigned int local_space>
inline typename cow_vector<T, local_space>::const_iterator cow_vector<T, local_space>::cend() const {
	if (is_local()) {
		return const_iterator(local_data + local_size);
	} else {
		return const_iterator(&*rem_data->cend());
	}
}

template<typename T, unsigned int local_space>
inline size_t cow_vector<T, local_space>::size() const {
	if (is_local()) {
		return local_size;
	} else {
		return rem_data->size();
	}
}

template<typename T, unsigned int local_space>
inline size_t cow_vector<T, local_space>::capacity() const {
	if (is_local()) {
		return local_space;
	} else {
		return rem_data->capacity();
	}
}

template<typename T, unsigned int local_space>
inline void cow_vector<T, local_space>::reserve(size_t new_capacity) {
	unlink();
	if (is_local()) {
		if (new_capacity > local_space) {
			make_remote();
			rem_data->reserve(new_capacity);
		}
	} else {
		rem_data->reserve(new_capacity);
	}
}

template<typename T, unsigned int local_space>
inline void cow_vector<T, local_space>::clear() {
	local_size = 0;
	rem_data.reset();
}

template<typename T, unsigned int local_space>
inline bool cow_vector<T, local_space>::empty() const {
	return size() == 0;
}

template<typename T, unsigned int local_space>
inline typename cow_vector<T, local_space>::iterator cow_vector<T, local_space>::insert(const_iterator pos,
		const T& value) {
	unlink();
	if (is_local()) {
		if (local_size + 1 > local_space) {
			make_remote();
			insert(pos, value);
		} else {
			int d = pos - begin();
			for (int i = local_size; i > d; i--) {
				local_data[i] = std::move(local_data[i - 1]);
			}
			local_data[d] = value;

			return iterator(local_data + d);
		}
	} else {

		return iterator(&*rem_data->insert(rem_data->begin() + (pos - begin()), value));
	}
}

template<typename T, unsigned int local_space>
inline typename cow_vector<T, local_space>::iterator cow_vector<T, local_space>::insert(const_iterator pos, T&& value) {
	unlink();
	if (is_local()) {
		if (local_size + 1 > local_space) {
			make_remote();
			insert(pos, value);
		} else {
			int d = pos - begin();
			for (int i = local_size; i > d; i--) {
				local_data[i] = std::move(local_data[i - 1]);
			}
			local_data[d] = value;

			return iterator(local_data + d);
		}
	} else {

		return iterator(&*rem_data->insert(rem_data->begin() + (pos - begin()), value));
	}
}

template<typename T, unsigned int local_space>
inline typename cow_vector<T, local_space>::iterator cow_vector<T, local_space>::insert(const_iterator pos,
		size_t count, const T& value) {
	unlink();
	if (is_local()) {
		if (local_size + count > local_space) {
			make_remote();
			insert(pos, value);
		} else {
			int d = pos - begin();
			for (int i = local_size + count - 1; i >= d + count; i--) {
				local_data[i] = std::move(local_data[i - 1]);
			}
			std::fill(local_data + d, local_data + d + count, value);
			local_data[d] = value;

			return iterator(local_data + d);
		}
	} else {

		return iterator(&*rem_data->insert(rem_data->begin() + (pos - begin()), count, value));
	}
}

template<typename T, unsigned int local_space>
inline void cow_vector<T, local_space>::append(size_t count, const T& value) {
	unlink();
	if (is_local()) {
		if (count + local_size > local_space) {
			make_remote();
			append(count, value);
		} else {
			std::fill(local_data + local_size, local_data + local_size + count, value);
			local_size += count;
		}
	} else {
		rem_data->insert(rem_data->end(), count, value);
	}
}

template<typename T, unsigned int local_space>
inline void cow_vector<T, local_space>::push_back(const T& value) {
	unlink();
	if (is_local()) {
		if (local_size + 1 > local_space) {
			make_remote();
			push_back(value);
		} else {
			local_data[local_size++] = value;
		}

	} else {
		rem_data->push_back(value);
	}
}

template<typename T, unsigned int local_space>
inline void cow_vector<T, local_space>::push_back(T&& value) {
	unlink();
	if (is_local()) {
		if (local_size + 1 > local_space) {
			make_remote();
			push_back(value);
		} else {
			local_data[local_size++] = value;
		}

	} else {
		rem_data->push_back(value);
	}
}

template<typename T, unsigned int local_space>
inline void cow_vector<T, local_space>::pop_back() {
	unlink();
	if (is_local()) {
		local_size--;
	} else {
		rem_data->pop_back();
		try_make_local();
	}

}

template<typename T, unsigned int local_space>
inline void cow_vector<T, local_space>::resize(size_t count) {
	unlink();
	if (is_local()) {
		if (count > local_space) {
			make_remote();
			resize(count);
		} else if (count > local_size) {
			std::fill(local_data + local_size, local_data + count, T());
			local_size = count;
		} else {
			local_size = count;
		}
	} else {
		rem_data->resize(count);
	}

}

template<typename T, unsigned int local_space>
inline void cow_vector<T, local_space>::resize(size_t count, const T& value) {
	unlink();
	if (is_local()) {
		if (count > local_space) {
			make_remote();
			resize(count, value);
		} else if (count > local_size) {
			std::fill(local_data + local_size, local_data + count, value);
			local_size = count;
		} else {
			local_size = count;
		}
	} else {
		rem_data->resize(count, value);
		try_make_local();
	}

}

template<typename T, unsigned int local_space>
inline void cow_vector<T, local_space>::shrink_to_fit() {
	try_make_local(0);
	if (!is_local()) {
		unlink();
		rem_data->shrink_to_fit();
	}
}

template<typename T, unsigned int local_space>
inline void cow_vector<T, local_space>::swap(cow_vector<T, local_space>& other) {
	if (is_local() && other.is_local()) {
		T buf[local_space];
		std::move(local_data, local_data + local_size, buf);
		std::move(other.local_data, other.local_data + other.local_size, local_data);
		std::move(buf, buf + local_size, other.local_data);
		std::swap(local_size, other.local_size);
	} else {
		if (is_local()) {
			std::move(local_data, local_data + local_size, other.local_data);
		} else if (other.is_local()) {
			std::move(other.local_data, other.local_data + other.local_size, local_data);
		}
		std::swap(rem_data, other.rem_data);
		std::swap(local_size, other.local_size);
	}
}

template<class T, size_t local_space>
inline void swap(cow_vector<T, local_space>& lhs, cow_vector<T, local_space>& rhs) {
	lhs.swap(rhs);
}

#endif /* SOO_COW_VECTOR_CPP_ */
