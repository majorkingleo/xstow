/*
 * $Log: setup_config.cpp,v $
 * Revision 1.2  2005/07/04 21:59:42  martin
 * added logging to all files
 *
 */
#include "setup_config.h"
#include "string_utils.h"
#include "leoini.h"
#include "cppdir.h"
#include "debug.h"

#undef OUT

#define OUT( level ) DEBUG_OUT( level, MODULE_SETUP )

#ifdef CAN_USE_INI

bool read_ini_value( Leo::Ini &ini, const std::string &section, const std::string &key, std::string &value )
{
  if( !ini )
    return false;

  Leo::Ini::Element el( Leo::Ini::Element::TYPE::KEY, section, key );

  if( ini.read( el ) )
    {
      value = strip( el.value );
      return true;
    }

  return false;
}

bool read_ini_value( Leo::Ini &ini, const std::string &section, const std::string &key, vec_string &values )
{

  if( !ini )
    return false;

  Leo::Ini::Element el( Leo::Ini::Element::TYPE::SECTION, section );

  std::string k = toupper( key );

  if( ini.read( el ) )
    {
      bool found = false;

      for( Leo::Ini::Element::element_list_it it = el.elements.begin();
	   it != el.elements.end(); ++it )
	if( toupper( it->key ) == k )
	  {
	    found = true;
	    values.push_back( strip( it->value ) );
	  }

      return found;
    }

  return false;
}

#endif

bool DirChecker::check( const std::string &s )
{
  CppDir::File file( s );

  if( !file )
    {
      ERROR( "Warning: %s is not a valid file\n", s );
      return false;
    }

  if( file.get_type() != CppDir::EFILE::DIR )
    {
      ERROR( "Warning: %s is not a valid directory\n", s );
      return false;
    }

  if( must_be_link )
    if( !file.is_link() )
      {
	ERROR( "Warning: %s is not a valid link to a valid directory\n", s );
	return false;
      }
  
  return true;
}

std::string DirChecker::strip( const std::string &s )
{
  if( s.size() > 1 )
    if( s[ s.size() - 1 ] == '/' )
      return s.substr( 0, s.size() - 1 );

  return s;
}

std::string VecDirChecker::strip( const std::string &s )
{
  return checker.strip( s );
}


bool VecDirChecker::check( const vec_string &v )
{
  for( unsigned int i = 0; i < v.size(); ++i )
    {
      if( !checker.check( v[i] ) )
	return false;
    }

  return true;
}

bool VecDirChecker::check( const std::string &s )
{
  return checker.check( s );    
}

bool BoolValue::string2data( const std::string &s )
{
  return s2x<bool>( s );
}

void VecStringValue::operator+=( const std::string &s )
{
  if( match_only_once )
    if( already_have( s ) )
      return;

  if( checker )
    {
      if( checker->check( s ) )
	data.push_back( checker->strip( s ) );
    }
  else
    data.push_back( s );
}

void VecStringValue::operator+=( const vec_string &v )
{
  for( unsigned int i = 0; i < v.size(); ++i )
    operator+=( v[i] );
}


vec_string VecStringValue::string2data( const std::string &s )
{
  return split_simple( s );
}

#ifdef CAN_USE_INI

void VecStringValue::read_ini( Leo::Ini &ini )
{
  if( !ini_section.empty() && !ini_key.empty() )
    {
      vec_string value;
      
      if( read_ini_value( ini, ini_section, ini_key, value ) )
	operator+=( value );
    }
}

#endif

bool VecStringValue::already_have( const std::string &s )
{
  for( unsigned int i = 0; i < data.size(); ++i )
    if( data[i] == s )
      return true;

  return false;
}


void RegExVecStringValue::operator+=( const std::string &s )
{
#ifdef HAVE_REGEX_H

  if( match_only_once )
    if( already_have( s ) )
      return;

  regex_t re;

  if( regcomp( &re, s.c_str(), REG_EXTENDED ) == 0 )
    {
      data.push_back( s );
      regs.push_back( re );
    }
  else
    ERROR( "Warning: regular expression %s is not valid\n", s );

#else

  ERROR << "Warning: XStow was not compiled with regular expression support\n";

#endif
}


#ifdef CAN_USE_INI

void Value<std::string>::read_ini( Leo::Ini &ini )
{
  if( !ini_section.empty() && !ini_key.empty() )
    {
      std::string value;

      read_ini_value( ini, ini_section, ini_key, value );	

      if( checker )
	if( checker->check( value ) )
	  data = checker->strip( value );
    }
}

#endif

void Value<std::string>::operator()( const std::string &data_ )
{
  if( checker )
    {
      if( checker->check( data_ ) )
	data = checker->strip( data_ );
    }
  else
    data = data_;
}

std::ostream& operator<<( std::ostream& out, const vec_string &v )
{
  for( unsigned int i = 0; i < v.size(); ++i )
    out << ' ' << v[i] << '\n';
  
  return out;
}

#ifdef CAN_USE_INI

void Section::read_ini( Leo::Ini &ini )
{
  for( unsigned int i = 0; i < values.size(); ++i )
    values[i]->read_ini( ini );
}

void Section::add( IniValue *value )
{
  if( value )
    if( value->ini_section.empty() )
      if( !ini_section.empty() )
	value->ini_section = ini_section;
  
  values.push_back( value );
}

bool Section::check_ini( Leo::Ini &ini, const std::string &key )
{
    for( unsigned int i = 0; i < values.size(); ++i )
	if( values[i]->ini_key == key )
	    return true;

    return false;
}

#endif
