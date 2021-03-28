#ifndef printf_format_h
#define printf_format_h

#include <format.h>
#include <ref.h>

namespace Tools
{

namespace Format
{
template<typename ostream>
	class PrintF
	{
	public:
		class FinalizeString
		{
		public:
			virtual ~FinalizeString() {}

			virtual std::string operator()( int module, const std::string & s ) {
				return s;
			}
		};

	private:

		int level;
		int dlevel;

		int dmodule;
		int module;

		Tools::Ref<FinalizeString> fs;
		bool finalizerEnabled;

	public:

		ostream &out;

	public:

		PrintF(ostream &out = std::cout,
				int module = -1,
				int debug_level = -1,
				Tools::Ref<FinalizeString> fs_ = Tools::Ref<FinalizeString>() )
			: level(debug_level),
			  dlevel(debug_level),
			  dmodule(module),
			  module(module),
			  out(out),
			  fs( fs_ ),
			  finalizerEnabled ( true )
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
				if (check()) {
					out << t;
				}

				return *this;
			}

		// io manipulator overloading
		PrintF& operator<<(ostream& (*f)(ostream&))
		{
			if (check()) {
				out << f;
			}

			return *this;
		}

		PrintF& operator()(std::string fs)
		{
			if (check()) {
				out << fs;
			}

			return *this;
		}

		 template <typename... Args> PrintF& operator()( const std::string & format, Args... args )
		  {
			 if( check() ) {
				 std::vector<Format2::BaseArg*> v_args;

				 Format2::add_args( v_args, args... );

				 Format2::Format2 f2( format, v_args );

				 for( auto x: v_args )
				 {
					 delete x;
				 }

				 std::string res = f2.get_string();

				 if( finalizerEnabled && fs.valid() ) {
					 res = fs->operator()( module, res );
				 }

				 out << res;
			 }

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

		PrintF & setFinalizer( bool enabled ) {
			finalizerEnabled = enabled;
			return *this;
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
