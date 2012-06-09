/*
 * $Log: string_utils.cpp,v $
 * Revision 1.2  2005/07/04 21:59:42  martin
 * added logging to all files
 *
 */
#include "string_utils.h"
#include <cctype>
#include "debug.h"

std::string toupper( std::string s )
{
  for( unsigned int i = 0; i < s.size(); ++i )
    s[i] = std::toupper( s[i] );

  return s;
}

bool is_int( const std::string &s )
{
  if( s.empty() )
    return false;

  for( unsigned int i = 0; i < s.size(); ++i )
    {
      switch( s[i] )
	{
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	  break;

	default:
	  return false;
	}
    }

  return true;
}

std::string strip( const std::string& str, const std::string& what )
{
  if( str.empty() )
    return std::string();

  std::string::size_type start = str.find_first_not_of( what );
  std::string::size_type end = str.find_last_not_of( what );

  if( start == std::string::npos )
    start = 0;

  if( !end == std::string::npos )
    if( end - start + 1 <= 0 )
      return std::string();

  return str.substr( start, end - start + 1 );
}

std::string text_right_format( std::string s, unsigned int max_size, unsigned int spaces )
{
  std::string space;
  
  for( unsigned int i = 0; i < spaces; ++i )
    space += ' ';
  
  unsigned int i;
  
  for( i = max_size; i >= 0 && s[i] != ' '; --i );
  
  std::string ss = s.substr( i + 1 );
  s = s.substr( 0, i );
 
  bool has_new_line = false;
 
  while( !ss.empty() )
    {
      for( i = max_size - spaces; i < ss.size() && i >= 0 && ss[i] != ' '; --i );
      
      s += '\n';
      s += space;      

      s += ss.substr( 0, i );
      
      has_new_line = true;

      if( i >= ss.size() )
	break;
      
      ss = ss.substr( i + 1 );
    }

  if( has_new_line )
    s +='\n';

  return s;
}

std::vector<std::string> split_simple( std::string str, std::string sep, int max )
{
  str = strip( str, sep );

  std::string::size_type start = 0, last = 0;
  int count = 0;

  std::vector<std::string> sl;

  while( true )
    {
      if( max > 0 )
	count++;

      if( count >= max && max > 0 )
	{
	  sl.push_back( str.substr( last ) );
	  break;
	}


      start = str.find_first_of( sep, last );

      if( start == std::string::npos )
	{
	  sl.push_back( str.substr( last ) );
	  break;
	}

      sl.push_back( str.substr( last, start - last ) );

      last = start + 1;
    }

  return sl;
}

/*
bool s2x( const std::string &s )
{
  if( s == "0" || toupper( s ) == "FALSE" )
    return false;
  
  if( s == "1" || toupper( s ) == "TRUE" )
    return true;
  
  out( "Warning: illegal value for boolean state: %s\n", s );

  return false;
}
*/

bool s2bool( const std::string &s )
{
    if( s == "1" || toupper( s ) == "TRUE" )
	return true;

    return false;
}

std::string x2s( bool b )
{
  if( b )
    return "TRUE";

  return "FALSE";
}

std::string substitude( std::string str, std::string what, std::string with )
{
  std::string::size_type pos=0;

  for(;;)
    {
      pos = str.find( what, pos );
      if( pos == std::string::npos )
        break;

      str.replace( pos, what.size(), with );
      pos += (what.size() > with.size() ? what.size() : with.size() );
    }
  return str;
}
