/*
 * $Log: setup_config.h,v $
 * Revision 1.2  2005/07/04 21:59:42  martin
 * added logging to all files
 *
 */
#ifndef setup_config_h
#define setup_config_h

#include <string>
#include <iostream>

#include <vector>
#include "debug.h"
#include "string_utils.h"
#include "local_config.h"

#ifdef HAVE_REGEX_H
extern "C" {
#  include <sys/types.h>
#  include <regex.h>
}
#endif

/**
   The Setup class is now much more complex as the simple structure before,
   but it does much more work.

   The required informations from the ini file will be read if possible.
   And all checking (eg.: File is link) is done here.

   The next step will be lettin this class reading the informations from
   the command line options itself.
*/

typedef std::vector<std::string> vec_string;

#ifdef CAN_USE_INI
# define ON_INI( expr ) expr
#else
# define ON_INI( expr )
#endif

#ifdef HAVE_REGEX_H
# define ON_REGEX( expr )  expr
#else
# define ON_REGEX( expr )
#endif

#ifdef CAN_USE_INI

namespace Leo {

  class Ini;

}

 
bool read_ini_value( Leo::Ini &ini, const std::string &section, const std::string &key, std::string &value );
bool read_ini_value( Leo::Ini &ini, const std::string &section, const std::string &key, vec_string &values );

#endif


struct IniValue
{
  IniValue() {}
  virtual ~IniValue() {}

#ifdef CAN_USE_INI
  IniValue( const std::string &is, const std::string &ik )
    : ini_section( is ), ini_key( ik ) {}

  std::string ini_section;
  std::string ini_key;

  virtual void read_ini( Leo::Ini &ini ) = 0; 

#else

  IniValue( const std::string &is, const std::string &ik ) {}

#endif
};



template<typename T> struct Checker
{
  virtual ~Checker() {}

  virtual bool        check( const T &t ) { return true; }
  virtual bool        check( const std::string &s ) { return true; }
  virtual T           strip( const T &t ) { return t; }
  virtual std::string strip( const std::string &s ) { return s; }
};

template<> struct Checker<std::string>
{
  virtual ~Checker() {}

  virtual bool        check( const std::string &s ) { return true; }
  virtual std::string strip( const std::string &s ) { return s; }
};

struct DirChecker : public Checker<std::string>
{
  bool must_be_link;

  DirChecker( bool mbl = false ) : must_be_link( mbl ) {}

  bool check( const std::string &s );
  std::string strip( const std::string &s );
};

struct VecDirChecker : public Checker<vec_string>
{
  DirChecker checker;

  VecDirChecker( bool mbl = false ) : checker( mbl ) {}

  bool        check( const vec_string &v );
  bool        check( const std::string &s );
  std::string strip( const std::string &s );
};

template<typename T> struct Value : public IniValue
{
  Value() : IniValue(), checker(0) {}

  Value( const std::string &is, const std::string &ik, Checker<T> *c = 0 )
    : IniValue( is, ik ), checker( c )
  {}

  T data;
  
  Checker<T> *checker;

  T& operator()() { return data; }
  void operator()( const T &data_ ) 
  {
    if( checker )
      {
	if( checker->check( data_ ) )
	  data = checker->strip( data_ ); 
      }
    else
      data = data_;
  }

  void operator()( const char* s ) { operator()( std::string( s ) ); }
  void operator()( const std::string &s ) 
  { 
    T data_ = string2data( s ); 

    if( checker )
      {
	if( checker->check( data_ ) )
	  data = checker->strip( data_ );
      }
    else
      data = data_;
  }
  
  virtual T string2data( const std::string &s ) = 0;

#ifdef CAN_USE_INI
  void read_ini( Leo::Ini &ini )
  { 
    if( !ini_section.empty() && !ini_key.empty() )
      {
	std::string value;

	if( read_ini_value( ini, ini_section, ini_key, value ) )
	  {
	    T d = string2data( value );
	    
	    if( checker )
	      {
		if( checker->check( d ) )
		  data = checker->strip( d );
	      }
	    else
	      data = d;
	  }
      }
  }

#endif

};

template<typename T> std::ostream& operator<<( std::ostream &out, const Value<T> &v ) { return out << x2s( v.data ); }

struct BoolValue : public Value<bool>
{
  BoolValue() : Value<bool>() {}
  BoolValue( const std::string &is, const std::string &ik )
    : Value<bool>( is, ik ) {}

  bool string2data( const std::string &s );
};

struct VecStringValue : public Value<vec_string>
{
  bool match_only_once;

  VecStringValue() : Value<vec_string>(), match_only_once( false ) {}
  VecStringValue( const std::string &is, const std::string &ik, Checker<vec_string> *c = 0 )
    : Value<vec_string>( is, ik, c ), match_only_once( false ) {}

  void operator+=( const vec_string &v );
  virtual void operator+=( const std::string &s );
  vec_string string2data( const std::string &s );
  std::string& operator[]( unsigned int i ) { return data[i]; }
  unsigned int size() const { return data.size(); }
  ON_INI( void read_ini( Leo::Ini &ini ); )

  bool already_have( const std::string &s );
};

struct RegExVecStringValue : public VecStringValue
{
protected:
  ON_REGEX( std::vector<regex_t> regs; )

public:
  RegExVecStringValue() : VecStringValue() {}
  RegExVecStringValue( const std::string &s, const std::string &k )
    : VecStringValue( s, k )
  {}

  void operator+=( const vec_string &v ) { VecStringValue::operator+=( v ); }
  void operator+=( const std::string &s );

  ON_REGEX( regex_t& regex( unsigned int i ) { return regs[i]; } )
};


struct IntValue : public Value<int>
{
  IntValue() : Value<int>() {}
  IntValue( const std::string &is, const std::string &ik, Checker<int> *c = 0 )
    : Value<int>( is, ik, c ) {}

  int string2data( const std::string &data_ ) { return s2x<int>( data_ ); }
};

template<> struct Value<std::string> : public IniValue
{
  Value() : IniValue(), checker(0) {}
  Value( const std::string &is, const std::string &ik, Checker<std::string> *c = 0 )
    : IniValue( is, ik ),
       checker( c )
  {}

  Checker<std::string> *checker;

  std::string data;

  std::string& operator()() { return data; }
  void operator()( const std::string &data_ );
  std::ostream& operator<<( std::ostream &out ) { return out << x2s( data ); }
  ON_INI( void read_ini( Leo::Ini &ini ); )
};

class Section
{
#ifdef CAN_USE_INI

  std::vector<IniValue*> values;  
  std::string ini_section;
  bool        auto_read_ini;

 public:

  Section() {}
  Section( const std::string &section, bool auto_read_ini = true )
    : ini_section( section ), auto_read_ini( auto_read_ini ) {}

  virtual ~Section() {}

  virtual void read_ini( Leo::Ini &ini );
  void add( IniValue *value );

  bool check_ini( Leo::Ini &ini, const std::string &key );

  std::string get_name() const { return ini_section; }
#else

 public:
  Section() {}
  Section( const std::string &section ) {}

#endif
};

#endif
