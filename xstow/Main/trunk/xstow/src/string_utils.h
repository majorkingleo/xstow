/*
 * $Log: string_utils.h,v $
 * Revision 1.2  2005/07/04 21:59:42  martin
 * added logging to all files
 *
 */
#ifndef string_utils_h
#define string_utils_h

#include <string>
#include <vector>

#include "local_config.h"

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

std::string toupper( std::string s );
std::string strip( const std::string& str, const std::string& what = " \t\n\0" );
bool is_int( const std::string &s );
std::vector<std::string> split_simple( std::string str, std::string seperator = " \t\n\0", int max = -1 );

/*
/// convert a string to anything
template <class T> T s2x( const std::string& s )
{
  std::strstream str;
  
  str << s << std::ends;
  T t;
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
  
  str << what << std::ends;
  std::string s( str.str() );

#ifdef STRSTREAM
  str.freeze(0);
#endif

  return s;
}

bool s2x( const std::string &s );
std::string x2s( bool b );

*/


inline bool is_bool( const bool &b ) { return true; }
template<class T> bool is_bool( const T &t ) { return false; }


bool s2bool( const std::string &s );

/// convert a string to anything
template <class T> T s2x( const std::string& s )
{
    if( is_bool( T() ) )
    {
	return s2bool( s );
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

std::string substitude( std::string str, std::string what, std::string with );

#undef STRSTREAM
#undef strstream
#undef ENDS

#endif
