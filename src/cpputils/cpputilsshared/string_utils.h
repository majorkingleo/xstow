#ifndef TOOLS_string_utils_h
#define TOOLS_string_utils_h

#include <string>
#include <vector>

#define HAVE_STL_SSTREAM

#if __GNUC__ == 2
#undef HAVE_STL_SSTREAM
#endif

#ifdef HAVE_STL_SSTREAM
#  include <sstream>
#  include <cctype>
#  define strstream stringstream
#  define ENDS
#else
#  include <ctype.h>
#  include <strstream>
#  define ENDS << std::ends;
#  define STRSTREAM
#endif  

namespace Tools {

std::string toupper( std::string s );
std::string tolower( std::string s );
std::string strip( const std::string& str, const std::string& what = " \t\n\r" );
std::string strip_leading( const std::string& str, const std::string& what = " \t\n\r" );
std::string strip_trailing( const std::string& str, const std::string& what = " \t\n\r" );
bool is_int( const std::string &s );
std::vector<std::string> split_simple( std::string str, std::string seperator = " \t\n", int max = -1 );
std::vector<std::string> split_string( std::string str, std::string seperator, int max = -1 );
std::vector<std::string> split_and_strip_simple( std::string str, const std::string & sep = " \t\n", int max = -1 );

inline bool is_bool( const bool &b ) { return true; }
template<class T> bool is_bool( const T &t ) { return false; }


inline bool get_bool( const bool & b ) { return b; }

// dummy function
template<class T> T get_bool( bool b ) { return T(); }


bool s2bool( const std::string &s );

/// convert a string to anything
template <class T> T s2x( const std::string& s )
{
    if( is_bool( T() ) )
    {
      bool b = s2bool( s );

      return get_bool<T>(b);
    }

  std::strstream str;

  str << s ENDS;
  T t;
  str >> t;

#ifdef STRSTREAM
  str.freeze(0);
#endif

  return t;
}


/// convert a string to anything
template <class T> T s2x( const std::string& s, const T & init )
{
    if( is_bool( T() ) )
    {
	return s2bool( s );
    }

  std::strstream str;
  
  str << s ENDS;
  T t(init);
  str >> t;

#ifdef STRSTREAM
  str.freeze(0);
#endif

  return t;
}


/// converts anything to a string
template<class T>std::string x2s( T what )
{
  std::strstream str;
  
  str << what ENDS;
  std::string s( str.str() );

#ifdef STRSTREAM
  str.freeze(0);
#endif

  return s;
}

std::string x2s( const bool b );


std::string text_right_format( std::string text, unsigned int max_size, unsigned int spaces );

inline std::string auto_linebreak( std::string text, unsigned int max_size = 90 )
{
  return text_right_format( text, max_size, 0 );
}

std::string substitude( const std::string & str, const std::string & what, const std::string & with, std::string::size_type start = 0 );

std::string prepand( std::string str, std::string what, std::string prefix );

inline bool icase_cmp( const std::string &a, const std::string &b )
{
    if( a.size() != b.size() )
	return false;
    
    if( a == b )
	return true;

    if( toupper( a ) == toupper( b ) )
	return true;

    return false;
}

std::string bin_encode( const std::string &s );
std::string bin_decode( const std::string &s );

std::string group_thousand( const std::string &s, unsigned digit = 3, const std::string &sep = "." );

std::string escape( const std::string &s );

std::vector<std::string> split_safe( const std::string &s, const std::string &sep = " \n\t");

template <class T> std::string createInStatement( const T & list )
{
  std::string res;

  if( list.empty() )
	return res;

  for( typename T::const_iterator it = list.begin(); it != list.end(); it++ )
	{
	  if( !res.empty() )
		  res += ", ";

	  res += "'";
	  res += x2s(*it);
	  res += "'";		
	}  

  return "(" + res + ")";
}

std::string fill_leading( std::string s, const std::string fill_sign, unsigned int len );
std::string fill_trailing( std::string s, const std::string fill_sign, unsigned int len );

bool is_empty_str( const char *pcString );
bool is_empty_str( const std::string & string );
inline bool is_empty_string( const char *pcString ) { return is_empty_str( pcString ); }
inline bool is_empty_string(  const std::string & s ) {  return is_empty_str( s ); }

} // namespace Tools

#undef STRSTREAM
#undef strstream
#undef ENDS

#endif
