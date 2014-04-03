/*
  Format a C++ library for typesafe string formating in printf style
      (C) 2001 - 2003 by Martin Oberzalek <kingleo@gmx.at>  

   2004-01-01 remerged version 1.1 of Format with xstow's Format

  Examples:
     std::cout << format( "Hello %s, I have $05d$ in my pocket", "world", 5 ) << std::endl;
     std::cout << format( "Do not try this with printf: %s", 10101 ) << std::endl;

  
*/
/*
 * $Log: format.h,v $
 * Revision 1.4  2010/07/21 19:38:25  martin
 * gcc-4 Port
 *
 * Revision 1.3  2005/07/04 21:59:42  martin
 * added logging to all files
 *
 */

#ifndef format_h
#define format_h

#include <string>
#include <iomanip>
#include <iostream>

#include "local_config.h"

#if __GNUC__ == 2
#undef HAVE_STL_SSTREAM
#endif

#ifdef HAVE_STL_SSTREAM
#  include <sstream>
#  include <cctype>
#  define IS_DIGIT( x ) std::isdigit( x )
#else
extern "C" {
#  include <ctype.h>
}
#  include <strstream>
#  define IS_DIGIT( x ) isdigit( x )
#endif  

#ifndef NFORMAT

namespace Format
{
  class CFormat;
}

inline std::ostream& operator<<( std::ostream& out, Format::CFormat cf );

namespace Format
{
  typedef std::string::size_type ST;
  
  class CFormat
  {    
  public:
    
    enum Adjust
      {
		LEFT,
		RIGHT
      };
    
    enum Base
      {
		OCT,
		DEC,
		HEX
      };
    
	enum Floating
      {
		FIXED,
		SCIENTIFIC
      };
    
    bool valid;
    Adjust adjust;
    bool special;
    bool sign;
    bool grouping; // SUSv2 extension
    bool conversion; // glibc 2.2 extension
    bool zero;
    bool precision_explicit;
    bool internal;
    Base base;
    bool setupper;
    Floating floating;
    bool showbase;
    
    int width;
    int precision;
    int strlength;

    std::string format;

  public:    
    CFormat() { reset(); }
    
    void set( std::ostream& out );
    
  private:    
    void reset();
  };

  /****************************************/
  // all the errors that are thrown 
  // are a cause of a mistake with %* or %*m$
  class Error
  {
  public:
    std::string err;
    Error( std::string s ) : err( s ) {}
  };

  /****************************************/

  template <class A, class B, class C, class D, class E, class F>
  class Format
  {
  private:
    struct Arg
    {
      bool is_int;
      bool is_string;
    };
    
    Arg args[6];

    std::string format;
    
    A a;
    B b;
    C c;
    D d;
    E e;
    F f;
    
    unsigned int num_of_args;
    
    std::string s;
    
  public:
    Format( const std::string &format, A a, B b, C c, D d, E e, F f, unsigned int num_of_args );
    
    std::string get_string() const { return s; }
    
  private:
    void parse();

    template <class N> bool is_int( N &n ) { return false; }
    bool is_int( int &n ) { return true; }
    bool is_int( unsigned int &n ) { return true; }
    bool is_int( short &n ) { return true; }
    bool is_int( unsigned short ) { return true; }

    template <class S> bool is_string( S &s ) { return false; }
    bool is_string( std::string& s ) { return true; }
    bool is_string( const std::string& s ) { return true; }
    bool is_string( char* ) { return true; }
    bool is_string( const char* ) { return true; }

    int get_int_arg( unsigned int num );
    void gen_arg_list();
    std::string use_arg( unsigned int i, const CFormat &cf );

    template <class S> std::string x2s( S ss, const CFormat &cf )
    {
#ifdef HAVE_STL_SSTREAM
	std::stringstream str;
	str << cf << ss;
	std::string st = str.str();
	return st;
#else
      std::strstream str;
      str << cf << ss << std::ends;
      std::string st = str.str();
      str.freeze(0);
      return st;
#endif
    }

  };

  int skip_atoi( std::string s, ST start, ST& pos );

} // namespace Format

inline std::ostream& operator<<( std::ostream& out, Format::CFormat cf )
{
  cf.set( out );
  return out;
}

template <class A, class B, class C, class D, class E, class F>
Format::Format<A,B,C,D,E,F>::Format( std::string const &format, A a, B b, C c, D d, E e, F f, unsigned int num_of_args )
  : format( format ), a(a), b(b), c(c), d(d), e(e), f(f), num_of_args( num_of_args )
{
  if( num_of_args > 6 )
    throw Error( "Number of args out of range" );

  gen_arg_list();
  
  parse();
}

template <class A, class B, class C, class D, class E, class F>
int Format::Format<A,B,C,D,E,F>::get_int_arg( unsigned int num )
{
  if( static_cast<unsigned int>(num) > num_of_args - 1 )
    throw Error( "The arg you wan't to use is out of range" );

  if( num < 0 )
    throw Error( "negativ number for arg number not allowed" );

  if( args[num].is_int ) 
    {
      switch( num )
	{
	case 0: return *((int*) &a); // I have to cast here cause the compiler
	case 1: return *((int*) &b); // will make troubles if any of these
	case 2: return *((int*) &c); // values is not an unsigned int.
	case 3: return *((int*) &d); // Even if we are sure that
	case 4: return *((int*) &e); // an unsigned int value will be returned
	case 5: return *((int*) &f);
	}
    }
  else
    throw Error( "expecting int arg" );

  return 0; // should never be reached
}

template <class A, class B, class C, class D, class E, class F>
void Format::Format<A,B,C,D,E,F>::gen_arg_list()
{
  for( unsigned int i = 0; i < num_of_args; i++ )
    { 
      switch( i )
	{
	case 0: 
	  args[i].is_int = is_int( a ); 
	  args[i].is_string = is_string( a ); 
	  break;
	case 1: 
	  args[i].is_int = is_int( b ); 
	  args[i].is_string = is_string( b ); 
	  break;

	case 2: 
	  args[i].is_int = is_int( c ); 
	  args[i].is_string = is_string( c ); 
	  break;

	case 3: 
	  args[i].is_int = is_int( d ); 
	  args[i].is_string = is_string( d ); 
	  break;

	case 4: 
	  args[i].is_int = is_int( e ); 
	  args[i].is_string = is_string( e ); 
	  break;

	case 5: 
	  args[i].is_int = is_int( f ); 
	  args[i].is_string = is_string( f ); 
	  break;
	}
    }
}

template <class A, class B, class C, class D, class E, class F>
std::string Format::Format<A,B,C,D,E,F>::use_arg( unsigned int i, const CFormat &cf )
{
  if( i > num_of_args || i < 0 )
    throw Error( "out of arg range" );

  switch( i )
    {
    case 0: return x2s( a, cf );
    case 1: return x2s( b, cf );
    case 2: return x2s( c, cf );
    case 3: return x2s( d, cf );
    case 4: return x2s( e, cf );
    case 5: return x2s( f, cf );
    }

  return "";
}

template <class A, class B, class C, class D, class E, class F>
void Format::Format<A,B,C,D,E,F>::parse()
{
  if( format.empty() )
    return;

  unsigned int par = 0;
  unsigned int use_par = 0;
  ST pos = 0;
  ST len = format.size();
  s = "";
  bool had_precision = false;

  while( par < num_of_args && pos < len )
    { // while

      use_par = par;

      if( pos >= len )
	break;

      if( format[pos] != '%' )
	{
	  s += format[pos];
	  pos++;
	  continue;
	}

      // % digit found
      pos++;

      if( !(pos < len ) || (format[pos] == '%') )
	{
	  // %% -> %
	  s += format[pos];
	  pos++; 
	  continue;
	}
  
      // format string found

      ST start = pos - 1;
      CFormat f;

      // process flags

      while( (pos < len) )
	{
	  bool finished = false;
	  
	  switch( format[pos] )
	    {
	    case '-' : f.adjust = CFormat::LEFT; break;
	    case '+' : f.sign = true; break;
	    case ' ' : f.zero = false; break;
	    case '#' : f.special = true; break;
	    case '\'': f.grouping = true; break;
	    case 'I' : f.conversion = true; break; 
	    case '0' : f.zero = true; break;
	    default: finished = true; break;
	    }
	  
	  if( finished )
	    break;
	  
	  pos++;
	} //       while( (pos < len) )

      // get argument number
      if( pos < len )
	{
	  // search for the $ digit
	  unsigned int dp = pos;
	  
	  while( dp < len && IS_DIGIT( format[dp] ) ) 
	    dp++;
	  
	  if( dp < len && format[dp] == '$' )
	    {
	      use_par = skip_atoi( format, pos, pos ) - 1;
	      pos = dp + 1;
	    }
	}

      // get field with
      if( pos < len )
	{
	  if( IS_DIGIT( format[pos] ) )
	    f.width = skip_atoi( format, pos, pos );
	  else if( format[pos] == '*' )
	    {
	      pos++;

	      // search for the $ digit
	      unsigned int dp = pos;
	      
	      while( dp < len && IS_DIGIT( format[dp] ) ) 
		dp++;

	      if( dp < len && format[dp] == '$' )
		{
		  f.width = get_int_arg( skip_atoi( format, pos, pos ) - 1 );
		  // skip $ sign
		  pos++;
		}
	      else 
		{
		  f.width = get_int_arg( par );

		  if( use_par == par )
		    use_par++;

		  par++;
		} 

	      if( f.width < 0 )
		{
		  f.width *= -1;
		  f.adjust = CFormat::LEFT;
		}
	    }
	}

      // precision
      if( pos < len )
	{
	  if( format[pos] == '.' )
	    {
	      pos++;
	      if( !(pos < len) )
		return;
	      
	      had_precision = true;

	      if( IS_DIGIT( format[pos] ) )
		  f.precision = skip_atoi( format, pos, pos );
	      else if( format[pos] == '*' )
		{
		  pos++;
		  
		  
		  // search for the $ digit
		  unsigned int dp = pos;
		  
		  while( dp < len && IS_DIGIT( format[dp] ) ) 
		    dp++;
		  
		  if( dp < len && format[dp] == '$' )
		    {
		      f.precision = get_int_arg( skip_atoi( format, pos, pos ) - 1 );
		      // skip $ sign
		      pos++;
		    }
		  else 
		    {
		      f.precision = get_int_arg( par );

		      if( use_par == par )
			use_par++;

		      par++;
		    }
		  
		  if( f.precision == 0)
		    f.precision_explicit = true;
		  
		  if( f.precision < 0 )
		    f.precision = 0;
		}
	      else
		f.precision = 0;
	    }
	  
	}
    
      // lenght modifier
      /* 
	 they will be ignored 
	 cause we know the types of the parameter
      */      
      if( (pos < len) )
	{
	  bool hh = false;
	  bool ll = false;
	  bool found = false;
	  
	  switch( format[pos] )
	    {
	    case 'h': hh = true; found = true; break;
	    case 'l': ll = true; found = true; break;
	    case 'L':
	    case 'q':
	    case 'j':
	    case 'z':
	    case 't': found = true; break;
	    default: break;
	    }

	  if(found )
	    {
	      pos++;
	      
	      if( pos < len )
		if( hh == true )
		  {
		    if( format[pos] == 'h' )
		      pos++;
		  }
		else if( ll = true )
		  if( format[pos] == 'l' )
		    pos++;
	    }
	}

      // conversion specifier

      if( pos < len )
	{
	  bool invalid = false;
	   switch( format[pos] )
	     {
	     case 'd':
	     case 'i':
	       f.base = CFormat::DEC;
	       if( f.zero && (f.adjust != CFormat::LEFT) )
		 f.internal = true;
	       break;
	       
	     case 'X': f.setupper = true;
	     case 'x': 
	       f.base = CFormat::HEX;
	       if( f.special )
		 f.showbase = true;
	       break;
	       
	     case 'o': 
	       f.base = CFormat::OCT;
	       if( f.special )
		 f.showbase = true;
	       break;

	       
	     case 'E': 
	       f.setupper = true;

	     case 'e': 
	       if( f.special )
		 f.sign = true;
	       f.floating = CFormat::SCIENTIFIC;
	       break;
	       
	     case 'F': // not supported
	     case 'f': 
	       if( f.special )
		 f.sign = true;
	       f.floating = CFormat::FIXED;
	       break;
	       
	     case 's':
	       if( f.zero ) 
		 f.zero = false;
	       break;
	      

	       // unsupported modifiers


	     case 'G':
	     case 'g':
	       
	     case 'A':
	     case 'a':
	       
	     case 'c':
	       
	     case 'C':
	     case 'S':
	     case 'P':
	     case 'n': break;
	       
	     default: invalid = true;
	     }

	   if( !invalid )
	     f.valid = true;
	}  

      if( f.valid )
	{
	  std::string str;
	  int upar = par;

	  if( use_par != par )
	    upar = use_par;

	  if( f.base == CFormat::HEX && had_precision && f.special )
	  {
	      CFormat f2;
	      f2.base = f.base;
	      std::string s = use_arg( upar, f2 );
	      f.strlength = s.size();
//	      printf( "str: %s\n", s.c_str() );
	  }

	  str = use_arg( upar, f );

	  // cut string
	  if( had_precision && args[upar].is_string )
	    str = str.substr( 0, f.precision );

	  s += str;

	  if( use_par == par )
	    par++;
	}
      else
	{
	  // copy the invalid format string
	  for( ST i = start;  i<= pos; i++ )
	    if( i < len )
	      s += format[i];
	}
  
      pos++;

    } // while

  if( pos < len )
    {
      while( pos < len )
	{
	  s += format[pos];
	  pos++;
	}
    }
}

#else // ifndef NFORMAT

#include "string_utils.h" // for x2s

#include <cstdio>
#include <cstring>

namespace Format
{

template <class T> const char* convert( T t ) { return x2s( t ).c_str(); }

#define DEF( TYPE ) \
inline TYPE convert( TYPE t ) { return t; }

    DEF( unsigned )
    DEF( int )
    DEF( char  )
    DEF( char* )
    DEF( const char* )
    DEF( short )
    DEF( double )
    DEF( float )
    DEF( long )

#undef DEF


template<class A, class B, class C, class D, class E, class F>
class Format
{
    std::string s;

 public:
    Format( const std::string &format, A a, B b, C c, D d, E e, F f, unsigned int num_of_args )
	{
#define D( T ) convert( T )
	    unsigned buffer_size = 256;

	    bool cont = false;

	    do {

		cont = false;

		char *buffer = new char[buffer_size];
		int n = 0;
		
		switch( num_of_args )
		{
		    case 1: n = std::snprintf( buffer, buffer_size, format.c_str(),
					       D( a ) ); break;
		    case 2: n = std::snprintf( buffer, buffer_size, format.c_str(),
					       D( a ), D( b ) ); break;
		    case 3: n = std::snprintf( buffer, buffer_size, format.c_str(),
					       D( a ), D( b ), D( c ) ); break;
		    case 4: n = std::snprintf( buffer, buffer_size, format.c_str(),
					       D( a ), D( b ), D( c ), D( d ) ); break;
		    case 5: n = std::snprintf( buffer, buffer_size, format.c_str(),
					       D( a ), D( b ), D( c ), D( d ), D( e ) ); break;
		    case 6: n = std::snprintf( buffer, buffer_size, format.c_str(),
					       D( a ), D( b ), D( c ), D( d ), D( e ), D( f ) ); break;
		}
		
		
		if( (unsigned) n >= buffer_size - 2 )
		{
		    buffer_size *= 2;
		    cont = true;
		    n = 0;
		}

		for( int i = 0; i < n; i++ )
		    s += buffer[i];
		
		delete[] buffer;
		
	    } while( cont );
#undef D
	}

    std::string get_string() const { return s; }
};

}

#endif

template <class A, class B, class C, class D, class E, class F>
inline std::string format( std::string fs, A a, B b, C c, D d, E e, F f )
{
  return Format::Format<A,B,C,D,E,F>( fs, a, b, c, d, e, f, 6).get_string();
}

template <class A, class B, class C, class D, class E>
inline std::string format( std::string fs, A a, B b, C c, D d, E e )
{
  return Format::Format<A,B,C,D,E,char>( fs, a, b, c, d, e, 0, 5).get_string();
}

template <class A, class B, class C, class D>
inline std::string format( std::string fs, A a, B b, C c, D d)
{
  return Format::Format<A,B,C,D,char,char>( fs, a, b, c, d, 0, 0, 4).get_string();
}

template <class A, class B, class C>
inline std::string format( std::string fs, A a, B b, C c )
{
  return Format::Format<A,B,C,char,char,char>( fs, a, b, c, 0, 0, 0, 3).get_string();
}

template <class A, class B>
inline std::string format( std::string fs, A a, B b )
{
  return Format::Format<A,B,char,char,char,char>( fs, a, b, 0, 0, 0, 0, 2).get_string();
}

template <class A>
inline std::string format( std::string fs, A a)
{
  return Format::Format<A,char,char,char,char,char>( fs, a, 0, 0, 0, 0, 0, 1).get_string(); 
} 


namespace Format
{
  template<typename ostream> class PrintF
  {
  private:

    int level;
    int dlevel;

    int dmodule;
    int module;
    

  public:

    ostream &out;
    
  public:

    PrintF( ostream &out = std::cout, int module = -1, int debug_level = -1 ) 
      : level( debug_level ), dlevel( debug_level ), 
      dmodule( module), module( module),  out( out ) 
      {}

    void set_debug_level( int dlevel_ ) { dlevel = dlevel_; }
    void set_module( int module_ ) { dmodule = module_; }

    PrintF operator()( int module_ )
      {
	PrintF printf( *this );
	printf.module = module_;

	return printf;
      }

    PrintF operator[]( int level_ )
      {
	PrintF printf( *this );
	printf.level = level_;

	return printf;
      }

    template<typename T> PrintF& operator<<( const T &t ) 
      { 
	if( check() )
	  out << t; 

	return *this; 
      }

    // io manipulator overloading
    PrintF& operator<<(ostream& (*f)(ostream&))
      {
	if( check() )
	  out << f;

	return *this;
      }

    PrintF& operator()( std::string fs )
    {
      if( check() )
	out << fs;

      return *this;
    }


    template<typename A> PrintF& operator()( std::string fs, const A &a )
    {
      if( check() )
	out << format( fs, a );

      return *this;
    }

    template<typename A, typename B> 
    PrintF& operator()( std::string fs, const A &a, const B &b )
    {
      if( check() )
	out << format( fs, a, b );

      return *this;
    }

    template<typename A, typename B, typename C> 
    PrintF& operator()( std::string fs, const A &a, const B &b, const C &c )
    {
      if( check() )
	out << format( fs, a, b, c );

      return *this;
    }

    template<typename A, typename B, typename C, typename D> 
    PrintF& operator()( std::string fs, const A &a, const B &b, const C &c, const D &d )
    {
      if( check() )
	out << format( fs, a, b, c, d );

      return *this;
    }

    template<typename A, typename B, typename C, typename D, typename E> 
    PrintF& operator()( std::string fs, const A &a, const B &b, const C &c, const D &d, const E &e )
    {
      if( check() ) 
	out << format( fs, a, b, c, d, e );

      return *this;
    }

    template<typename A, typename B, typename C, typename D, typename E, typename F> 
    PrintF& operator()( std::string fs, const A &a, const B &b, const C &c, const D &d, const E &e, const F &f )
    {
      if( check() )
	out << format( fs, a, b, c, d, e, f );

      return *this;
    }

    bool check( int module, int level ) const
      {
	if( module == dmodule || dmodule == -1 )
	  {
	    if( dlevel == -1 )
	      return true;
	    
	    if( level <= dlevel )
	      return true;
	  }

	return false;	
      }

  private:
    
    bool check() const { return check( module, level ); }

  };

}


#undef IS_DIGIT

#endif
