#ifndef printf_format_h
#define printf_format_h

#include <format.h>

namespace Tools
{

namespace Format
{
template<typename ostream>
	class PrintF
	{
	private:

		int level;
		int dlevel;

		int dmodule;
		int module;

	public:

		ostream &out;

	public:

		PrintF(ostream &out = std::cout, int module = -1, int debug_level = -1)
				: level(debug_level), dlevel(debug_level), dmodule(module), module(module), out(out)
		{
		}

		void set_debug_level(int dlevel_)
		{
			dlevel = dlevel_;
		}
		void set_module(int module_)
		{
			dmodule = module_;
		}

		PrintF operator()(int module_)
		{
			PrintF printf(*this);
			printf.module = module_;

			return printf;
		}

		PrintF operator[](int level_)
		{
			PrintF printf(*this);
			printf.level = level_;

			return printf;
		}

		template<typename T>
			PrintF& operator<<(const T &t)
			{
				if (check())
					out << t;

				return *this;
			}

		// io manipulator overloading
		PrintF& operator<<(ostream& (*f)(ostream&))
		{
			if (check())
				out << f;

			return *this;
		}

		PrintF& operator()(std::string fs)
		{
			if (check())
				out << fs;

			return *this;
		}

		template<typename A>
			PrintF& operator()(std::string fs, const A &a)
			{
				if (check())
					out << format(fs, a);

				return *this;
			}

		template<typename A, typename B>
			PrintF& operator()(std::string fs, const A &a, const B &b)
			{
				if (check())
					out << format(fs, a, b);

				return *this;
			}

		template<typename A, typename B, typename C>
			PrintF& operator()(std::string fs, const A &a, const B &b, const C &c)
			{
				if (check())
					out << format(fs, a, b, c);

				return *this;
			}

		template<typename A, typename B, typename C, typename D>
			PrintF& operator()(std::string fs, const A &a, const B &b, const C &c, const D &d)
			{
				if (check())
					out << format(fs, a, b, c, d);

				return *this;
			}

		template<typename A, typename B, typename C, typename D, typename E>
			PrintF& operator()(std::string fs, const A &a, const B &b, const C &c, const D &d, const E &e)
			{
				if (check())
					out << format(fs, a, b, c, d, e);

				return *this;
			}

		template<typename A, typename B, typename C, typename D, typename E, typename F>
			PrintF& operator()(std::string fs, const A &a, const B &b, const C &c, const D &d, const E &e, const F &f)
			{
				if (check())
					out << format(fs, a, b, c, d, e, f);

				return *this;
			}

		bool check(int module, int level) const
		{
			if (module == dmodule || dmodule == -1) {
				if (dlevel == -1)
					return true;

				if (level <= dlevel)
					return true;
			}

			return false;
		}

	private:

		bool check() const
		{
			return check(module, level);
		}

	};

}

}

#endif
