/**
 * Classes for typesave versions of sprintf() that are returning a std::string
 * @author Copyright (c) 2001 - 2022 Martin Oberzalek
 *
 * C++-11 version with variadic templates
 *
 * Examples:
 *    std::cout << format( "Hello %s, I have $05d$ in my pocket", "world", 5 ) << std::endl;
 *    std::cout << format( "Do not try this with printf: %s", 10101 ) << std::endl;
 *
 */

#ifndef _wamas_FORMAT2_H
#define _wamas_FORMAT2_H

#if __cplusplus - 0 >= 201103L

#include <string>
#include <iomanip>
#include <iostream>

#include <sstream>
#include <cctype>
#include <vector>
#include <cformat.h>

namespace Tools {

  namespace Format2
  {
    class BaseException : public std::exception
    {
      const char *err;

    public:
      BaseException(const BaseException& ex): std::exception() , err ( ex.err ) {}
      BaseException( const char* e ) : std::exception(), err( e ) {}
      BaseException & operator=(const BaseException& ex) { err = ex.err; return *this; }

      virtual const char* what() const throw() { return err; }
    };

    class BaseArg
    {
      bool _is_int;
      bool _is_string;

    public:
      BaseArg( bool is_int_ = false, bool is_string_  = false )
    : _is_int( is_int_ ),
      _is_string( is_string_ )
    {}
      virtual ~BaseArg() {}

      bool isInt() { return _is_int; }
      bool isString() { return _is_string; }

      virtual std::string doFormat( const Format::CFormat & cf ) = 0;
      virtual int get_int() {
        if( !isInt() ) {
            throw BaseException( "expecting int arg" );
        }
        return 0;
      }

      template <class N> bool is_int( const N &n ) const { return false; }
      bool is_int( const int &n ) const { return true; }
      bool is_int( const unsigned int &n ) const { return true; }
      bool is_int( const short &n ) const { return true; }
      bool is_int( const unsigned short ) const { return true; }
      bool is_int( const unsigned long ) const { return true; }
      bool is_int( const unsigned long long ) const { return true; }
      bool is_int( const long long ) const { return true; }

      template <class S> bool is_string( const S &s_ ) const { return false; }
      bool is_string( std::string& s_ ) const { return true; }
      bool is_string( const std::string& s_ ) const { return true; }
      bool is_string( char* ) const { return true; }
      bool is_string( const char* ) const { return true; }
    };

    template<typename Arg> class RealArg : public BaseArg
    {
      const Arg arg;
    public:
      RealArg( const Arg & arg_ )
    : BaseArg( is_int( arg_ ), is_string( arg_ ) ),
      arg(arg_)
    {}

    private:
      RealArg()
        : BaseArg(), arg()
      {}

      RealArg( const RealArg & other )
      : BaseArg(), arg()
      {}

      RealArg & operator=( const RealArg & other )
      {
        return *this;
      }

    private:

      template <class S> std::string x2s( S ss, const Format::CFormat &cf )
      {
        std::stringstream str;
        str << cf << ss;
        return str.str();
      }

      virtual std::string doFormat( const Format::CFormat & cf )
      {
        return x2s( arg, cf );
      }

      template<class T> int get_int( const T &t ) { return 0; }
      int get_int( int n ) { return (int)n; }
      int get_int( unsigned int n ) { return (int)n; }
      int get_int( short n ) { return (int)n; }
      int get_int( unsigned short n ) { return (int)n; }
      int get_int( long long n ) { return (int)n; }
      int get_int( unsigned long long n  ) { return (int)n; }
      int get_int( long n ) { return (int)n; }
      int get_int( unsigned long n ) { return (int)n; }

      virtual int get_int() {
        BaseArg::get_int();
        return get_int(arg);
      }
    };

    template <class BaseArgType, class CastTo> class RealArgCastFromChar : public BaseArg
        {
          const BaseArgType arg;
        public:
          RealArgCastFromChar( const BaseArgType & arg_ )
        : BaseArg( false, false ),
          arg(arg_)
        {}

        private:
          virtual std::string doFormat( const Format::CFormat & cf )
          {
            std::stringstream str;
            str << cf;

            if( cf.numerical_representation )
              str << static_cast<CastTo>(arg);
            else
              str << arg;

            return str.str();
          }
        };

    template <> class RealArg<unsigned char> : public RealArgCastFromChar<unsigned char,int>
    {
    public:
      RealArg( const unsigned char & arg_ )
        : RealArgCastFromChar( arg_ )
       {}
    };

    template <> class RealArg<char> : public RealArgCastFromChar<char,int>
    {
    public:
      RealArg( const char & arg_ )
        : RealArgCastFromChar( arg_ )
       {}
    };


    template <class BaseArgType, class CastTo> class RealArgCastFromInt : public BaseArg
        {
          const BaseArgType arg;
        public:
          RealArgCastFromInt( const BaseArgType & arg_ )
        : BaseArg( true, false ),
          arg(arg_)
        {}

        private:
          virtual std::string doFormat( const Format::CFormat & cf )
          {
            std::stringstream str;
            str << cf;

            if( cf.character_representation )
              str << static_cast<CastTo>(arg);
            else
              str << arg;

            return str.str();
          }

          virtual int get_int() {
            BaseArg::get_int();
            return arg;
          }
        };

    template <> class RealArg<int> : public RealArgCastFromInt<int,char>
    {
    public:
      RealArg( const int & arg_ )
        : RealArgCastFromInt( arg_ )
       {}
    };

#define INT_REAL_ARG_CAST( type )\
    template <> class RealArg<type> : public RealArgCastFromInt<type,char>\
    {\
    public:\
      RealArg( const type & arg_ )\
        : RealArgCastFromInt( arg_ )\
       {}\
    }

    INT_REAL_ARG_CAST( unsigned int );
    INT_REAL_ARG_CAST( short );
    INT_REAL_ARG_CAST( unsigned short );
    INT_REAL_ARG_CAST( long );
    INT_REAL_ARG_CAST( unsigned long );

#undef INT_REAL_ARG_CAST

    template<typename Arg> void add_argsx( std::vector<BaseArg*> & v_args, Arg & arg )
    {
      v_args.push_back( new RealArg<Arg>( arg ) );
    }

    template<typename A, typename... Arg> void add_args( std::vector<BaseArg*> & v_args, A & a, Arg... arg )
    {
      add_argsx( v_args, a );
      add_args( v_args, arg... );
    }

    inline void add_args( std::vector<BaseArg*> & v_args )
    {

    }

    class Format2
    {
    private:
      struct Arg
      {
        bool is_int;
        bool is_string;
      };

      std::vector<BaseArg*> args;

      std::string format;


      unsigned int num_of_args;

      std::string s;

    private:
      Format2();
      Format2(const Format2 & f);
      Format2 & operator=(const Format2 & f);

    public:
      Format2( const std::string &format, std::vector<BaseArg*> & args );

      std::string get_string() const { return s; }

    private:
      void parse();

      int get_int_arg( int num );
      void gen_arg_list();
      std::string use_arg( unsigned int i, const Format::CFormat &cf );

      template<class T> int get_int( const T &t ) { return 0; }
      int get_int( int n ) { return (int)n; }
      int get_int( unsigned int n ) { return (int)n; }
      int get_int( short n ) { return (int)n; }
      int get_int( unsigned short n ) { return (int)n; }
      int get_int( long long n ) { return (int)n; }
      int get_int( unsigned long long n  ) { return (int)n; }
      int get_int( long n ) { return (int)n; }
      int get_int( unsigned long n ) { return (int)n; }


      int skip_atoi( std::string s, std::string::size_type start, std::string::size_type & pos ) const;
      std::string substitude( const std::string & str_orig, const std::string & what, const std::string & with, std::string::size_type start = 0 ) const;
    }; // class Format2

  } // namespace Format2
} // /namespace Tools

#if TOOLS_VERSION+0 >= 40

// forward to tools implementation
#include <sstring.h>

namespace Tools {
  template <typename... Args> std::string format( const std::string & format, Args... args )
  {
    std::vector<wamas::platform::format_2011::BaseArg*> v_args;

    wamas::platform::format_2011::add_args( v_args, args... );

    wamas::platform::format_2011::Format2011 f2( format, v_args );

    for( auto x: v_args )
      {
        delete x;
      }

    return f2.get_string();
  }
} // /namespace Tools

#else
namespace Tools {
  template <typename... Args> std::string format( const std::string & format, Args... args )
  {
    std::vector<Format2::BaseArg*> v_args;

    Format2::add_args( v_args, args... );

    Format2::Format2 f2( format, v_args );

    for( auto x: v_args )
      {
        delete x;
      }

    return f2.get_string();
  }
} // /namespace Tools
#endif

#endif
#endif  /* _wamas_FORMAT2_H */
