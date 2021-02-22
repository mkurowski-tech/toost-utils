#ifndef ADDITIONALUTILS_H
#define ADDITIONALUTILS_H

#include <string>
#include <sstream>
#include <random>
#include <vector>
#include <set>
#include <cassert>
#include <iterator>

namespace toost {

	/*	template<typename... Ts> string make_string(args...);
	 *
	 *  making string from types given with commas,
	 *  to easy concatenate primary types into string
	 *  example
	 *  	throw std::logic_error(toost::make_string("can't find a file", filename));
	 *
	 */

	template<class T> inline
	std::string make_string_helper(T t)
	{
		std::stringstream ss;
		ss << t;
		return ss.str();
	}

	template<class T, class ...Ts> inline
	std::string make_string_helper(T t, Ts... args)
	{
		std::stringstream ss;
		ss << t << make_string_helper(args...);
		return ss.str();
	}

	template<typename... Ts>
	inline std::string make_string(Ts... args)
	{
		std::stringstream ss;
		ss << make_string_helper(args...);
		return ss.str();
	}

	// are_same
	// usage like std::is_same, but for more than two types
	template<class T, class U, class ... V>
	struct are_same : std::false_type {};

	template<class T>
	struct are_same<T, T> : std::true_type {};

	template<class T, class ... U>
	struct are_same<T, T, U...> : are_same<T, U...> {};


	/* sequence<T>(T v)
	 * for(auto i : sequence(6)) cout << i;
	 *
	 * equivalent to
	 * for(int i=0; i<6; ++i)
	 */

	template<typename T = int>
	class sequence_iterator {
		T pos;
	public:
		sequence_iterator(T pos) : pos {pos} {}

		sequence_iterator<T>& operator++() { pos++; return *this; }
		sequence_iterator<T>& operator--() { pos--; return *this; }

		const T& operator*() { return pos; }

		bool operator!=(const sequence_iterator<T>& it) {
			return pos != it.pos;
		}
	};

	template<typename T = int>
	class sequence {
		T head {};
		T tail;
	public:
		sequence(T tail) : tail(tail) {assert(tail>=head);}
		sequence_iterator<T> begin() { return sequence_iterator<T> (head); }
		sequence_iterator<T> end() { return sequence_iterator<T> (tail); }

		sequence<T>& operator()() { return *this; }

		std::reverse_iterator<sequence_iterator<T>> rbegin() { return make_reverse_iterator(end()); }
		std::reverse_iterator<sequence_iterator<T>> rend() { return make_reverse_iterator(end()); }
	};

	/* series<T>(T from, T to)
	 * usage
	 * for(auto i : series('a', 'z')) cout << i;
	 *
	 * equivalent to
	 * for(char c='a'; c<='z'; ++c)
	 */

	template<typename T = int>
	class series_iterator {
		T pos;
	public:
		series_iterator(T p) : pos {p} {}
		series_iterator<T>& operator++() { pos++; return *this; }
		const T& operator*() { return pos; }

		bool operator!=(const series_iterator<T>& it) {
			return pos != it.pos;
		}
		using pointer = T*;
		using reference = T&;
		using value_type = T;
		using difference_type = T;
		using iterator_category = std::forward_iterator_tag;
	};

	template<typename T = int>
	class series {
		T first;
		T last;
	public:

		using iterator = series_iterator<T>;
		using iter = series_iterator<T>;

		series(T f, T l) : first(f), last(l) {assert(first<=last);}
		series_iterator<T> begin() const { return series_iterator<T> (first); }
		series_iterator<T> end() const { return ++(series_iterator<T> (last)); }
	};


	/* randvector<T>
	 * a vector with getting random element
	 * usage:
	 * randvector<int> v {1, 5, 6, 9}	- ctors as in typical vector
	 * int random element = v();	- generate random element
	 */
	template <typename T>
	class randvector : public std::vector<T> {

		std::random_device rd;
		std::mt19937 gen {rd()};
		std::uniform_int_distribution<decltype(std::declval<std::vector<T>>().size())> dist {0, std::vector<T>::size()-1};

	public:
		using std::vector<T>::vector;

		const T& operator()() {
			assert(!std::vector<T>::empty());
			return std::vector<T>::operator[](dist(gen));
		}
	};

	/* randint
	 * usage: to random int number, counting from zero
	 * randint r{100};	// range from 0 up to 100
	 * r();				// get random number
	 */

	class randint {
		const int to;

		std::random_device rd;
		std::mt19937 gen {rd()};
		std::uniform_int_distribution<int> dist {0, to};

	public:
		randint(int to) : to{to} {}

		const int operator()() {
			return dist(gen);
		}
	};



	/* math function, a little more than available in libraries
	 *
	 */

	/* strong<T>	means n!
	 * usage
	 * int v = strong<int>(6);
	 */
	template<typename T>
	constexpr T strong(T n) {
		static_assert(std::is_integral<T>::value, "should be integral");
		assert (n>=0);
		return n > 1 ? strong(n-1)*n : 1;
	}

	/* tenpower<T>(n)	means 10 power of n, to write a number with many zeros
	 * usage
	 * int v = tenpower<int>(6);
	 */
	template<typename T>
	T tenpower(T n) {
		static_assert(std::is_integral<T>::value, "should be integral");
		assert (n>=0);
		if(n>1)
			return tenpower(n-1)*10;
		else if(n==1)
			return 10;
		else
			return 1;
	}

	/* permutation
	 * the purpose is to get particular n permutation or random
	 * the base is a set<T>
	 * usage:
	 * permutation<char> p {'a', 'b', 'c'};
	 * p.capacity();	- count all possibilities
	 * p.get_n(1);		- n permutation
	 * p.get_rand(); 	- random permutation
	 */

	template<typename T>
	class permutation {
		const std::set<T> s;

		std::random_device rd;
		std::mt19937 gen {rd()};
		std::uniform_int_distribution<size_t> dist {0, capacity()-1};

	public:
		permutation(const std::set<T> &s) : s{s} {}

		size_t capacity() const {
			return strong(s.size());
		}

		std::vector<T> get_rand() {
			return get_n(dist(gen));
		}

		std::vector<T> get_n(size_t n) const {
			assert(s.size() >= 0);
			assert(n >= 0);
			assert(n < capacity());

			std::vector<T> result;
			if(s.empty()) return result;

			std::vector<T> base {s.begin(), s.end()};

			const size_t size = base.size();
			for(auto k : sequence(size-1)) {

				typename std::vector<T>::iterator it = base.begin() + n%base.size();
				result.push_back(*it);
				n /= base.size();
				base.erase(it);
			}
			result.push_back(base[n%base.size()]);

			return result;
		}
	};

	/* seqgenerator
	 * usage
	 * seqgenerator<int> s{0}
	 * s(); 	- generator of numbers, sometimes useful in algorithms
	 */

	template<typename T>
	class seqgenerator {
		T n {};
	public:
		seqgenerator(T start) : n(start) {};
		T operator()() {return n++;}
	};

	/* permutationidx, similar to above, but the base are elements of a container
	 * the purpose is to get particular n permutation or random
	 * the case is the elements can repeats and the permutation is taken over indexes of a container, not values of a container
	 * usage:
	 * permutation<vector<char>> p {'a', 'a', 'c'};	- permutation over indexes
	 * p.capacity();	- count all possibilities
	 * p.get_n(1);		- n permutation
	 * p.get_rand(); 	- random permutation
	 */

	template<typename C>
	class permutationidx {
		const C container;

		std::random_device rd;
		std::mt19937 gen {rd()};
		std::uniform_int_distribution<size_t> dist {0, capacity()-1};

	public:
		permutationidx(const C &c) : container{c} {}

		size_t capacity() const {
			return strong(container.size());
		}

		C get_rand() {
			return get_n(dist(gen));
		}

		C get_n(size_t n) const {
			assert(container.size() >= 0);
			assert(n >= 0);
			assert(n < capacity());

			C resultcontainer;
			if(container.empty()) return resultcontainer;

			std::vector<int> base;
			generate_n(std::back_insert_iterator<std::vector<int>>(base), container.size(), seqgenerator<int> {0});

			const size_t size = base.size();
			for(auto k : sequence(size-1)) {

				typename std::vector<int>::iterator it = base.begin() + n%base.size();
				resultcontainer.push_back(container[*it]);
				n /= base.size();
				base.erase(it);
			}
			resultcontainer.push_back(container[base[n%base.size()]]);

			return resultcontainer;
		}
	};

	/* vectorfile<T>, is a vector with reading and saving to a file
	 *
	 * vectorfile<int> v;
	 * v.read("test");
	 * v.save("test");
	 */

	template<typename T>
	class vectorfile : vector<T> {
	public:
		using vector<T>::vector;
		using vector<T>::begin;
		using vector<T>::end;

		void read(const string &filename) {
			std::fstream f(filename, f.in);
			if (!f.is_open())
				throw logic_error(make_string("failed to open", filename));

			copy(std::istream_iterator<T>(f), std::istream_iterator<T>(), back_insert_iterator<vector<T>>(*this));
		}
		void save(const string &filename) {
			std::fstream f(filename, f.out);
			if (!f.is_open())
				throw logic_error(make_string("failed to open", filename));

			copy(vector<T>::begin(), vector<T>::end(), std::ostream_iterator<T>{f, "\n"});
		}
	};


}

#endif
