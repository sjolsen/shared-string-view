#ifndef SJO_SHARED_STRING_VIEW_HH
#define SJO_SHARED_STRING_VIEW_HH

#include <memory>
#include <string>
#include <utility>
#include <ostream>
#include <functional>
#include <experimental/string_view>

namespace sjo {

	class shared_string_view
	{
	public:
		using traits_type            = typename std::string::traits_type;
		using value_type             = typename std::string::value_type;
		using pointer                = typename std::string::const_pointer;
		using const_pointer          = typename std::string::const_pointer;
		using reference              = typename std::string::const_reference;
		using const_reference        = typename std::string::const_reference;
		using iterator               = typename std::string::const_iterator;
		using const_iterator         = typename std::string::const_iterator;
		using reverse_iterator       = typename std::string::const_reverse_iterator;
		using const_reverse_iterator = typename std::string::const_reverse_iterator;
		using size_type              = typename std::string::size_type;
		using difference_type        = typename std::string::difference_type;

	private:
		std::shared_ptr <const std::string> _data;
		size_type _begin;
		size_type _end;

	public:
		shared_string_view (std::string s)
			: _data  (std::make_shared <const std::string> (std::move (s))),
			  _begin (0),
			  _end   (_data->length ())
		{
		}

		shared_string_view (std::experimental::string_view s)
			: shared_string_view (s.to_string ())
		{
		}

		shared_string_view (const char* s)
			: shared_string_view (std::string (s))
		{
		}

		shared_string_view ()
			: _begin (0),
			  _end (0)
		{
		}

		shared_string_view (const shared_string_view&) = default;
		shared_string_view (shared_string_view&&) = default;
		shared_string_view& operator = (const shared_string_view&) = default;
		shared_string_view& operator = (shared_string_view&&) = default;

		// Iterators
		auto   begin () const { return _data->begin  () + _begin; }
		auto  cbegin () const { return _data->cbegin () + _begin; }
		auto  rbegin () const { return _data->rend   () - _end; }
		auto crbegin () const { return _data->crend  () - _end; }

		auto   end () const { return _data->begin  () + _end; }
		auto  cend () const { return _data->cbegin () + _end; }
		auto  rend () const { return _data->rend   () - _begin; }
		auto crend () const { return _data->crend  () - _begin; }

		// Element Access
		auto operator [] (size_type pos) const { return (*_data)  [_begin + pos]; }
		auto at          (size_type pos) const { return _data->at (_begin + pos); }

		auto front () const { return *begin  (); }
		auto back  () const { return *rbegin (); }
		auto data  () const { return _data->data () + _begin; }

		// Capacity
		auto size   () const { return _end - _begin; }
		auto length () const { return _end - _begin; }
		auto empty  () const { return _begin == _end; }

		auto max_size () const { return _data->max_size (); }

		// Modifiers
		void remove_prefix (size_type n) { _begin += n; }
		void remove_suffix (size_type n) { _end   -= n; }

		void swap (shared_string_view& v) {
			std::swap (_data,  v._data);
			std::swap (_begin, v._begin);
			std::swap (_end,   v._end);
		}

		// Operations
		explicit operator std::string () const {
			return std::string (begin (), end ());
		}

		explicit operator std::experimental::string_view () const {
			return std::experimental::string_view (data (), size ());
		}

		int compare (const shared_string_view& v) const {
			auto rlen = std::min (length (), v.length ());
			auto cmp  = traits_type::compare (data (), v.data (), rlen);
			if (cmp != 0)
				return cmp;
			if (length () < v.length ())
				return -1;
			if (length () > v.length ())
				return +1;
			return 0;
		}
	};

	static inline
	std::string to_string (const shared_string_view& v) {
		return static_cast <std::string> (v);
	}

	static inline
	std::experimental::string_view to_string_view (const shared_string_view& v) {
		return static_cast <std::experimental::string_view> (v);
	}

	static inline
	shared_string_view subseq (shared_string_view s, size_t begin, size_t end) {
		s.remove_prefix (begin);
		s.remove_suffix (s.length () - end);
		return std::move (s);
	}

	static inline
	bool operator == (const shared_string_view& a, const shared_string_view& b) {
		return a.compare (b) == 0;
	}

	static inline
	bool operator != (const shared_string_view& a, const shared_string_view& b) {
		return !(a == b);
	}

	static inline
	bool operator < (const shared_string_view& a, const shared_string_view& b) {
		return a.compare (b) < 0;
	}

	static inline
	bool operator > (const shared_string_view& a, const shared_string_view& b) {
		return b < a;
	}

	static inline
	bool operator <= (const shared_string_view& a, const shared_string_view& b) {
		return !(a > b);
	}

	static inline
	bool operator >= (const shared_string_view& a, const shared_string_view& b) {
		return !(a < b);
	}

	static inline
	std::ostream& operator << (std::ostream& os, const shared_string_view& v) {
		return os.write (v.data (), v.size ());
	}

}

namespace std {
	template <>
	struct hash <sjo::shared_string_view>
	{
		using argument_type = sjo::shared_string_view;
		using result_type   = std::size_t;

		result_type operator () (const argument_type& v) const noexcept {
			using delegate = std::hash <std::experimental::string_view>;
			return delegate {} (to_string_view (v));
		}
	};
}

#endif
