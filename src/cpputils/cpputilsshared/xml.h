#ifndef TOOLS_XML_H
#define TOOLS_XML_H

#include <string>
#include <vector>
#include <iostream>

#include "ref.h"

namespace Tools {

namespace XML {

struct Item
{
  struct Option
  {
    std::string name;
    std::string value;

    Option()
    : name(),
      value()
    {}

    Option( const std::string &name_, const std::string &value_ )
      : name( name_ ), value( value_ )
    {}

    Option( const Option & other )
    : name( other.name ),
      value( other.value )
      {

      }

    Option & operator=( const Option & other )
    {
    	name = other.name;
    	value = other.value;
    	return *this;
    }

  };

  std::string name;

  bool beg_close; 
  bool end_close;

  std::vector<Option> options;

  std::vector<Ref<Item> > subitems;
  Item *parent;

  std::string text;
  std::string::size_type start_pos, end_pos;
  unsigned line;

  Item()
  :
	name(),
	beg_close( false ),
	end_close( false ),
	options(),
	subitems(),
	parent(0),
	text(),
	start_pos(0),
	end_pos(0),
	line(0)
   {}

  Item( const Item & other )
  : name( other.name ),
    beg_close( other.beg_close ),
	end_close( other.end_close ),
	options(other.options),
	subitems(other.subitems),
	parent(other.parent),
	text(other.text),
	start_pos(other.start_pos),
	end_pos(other.end_pos),
	line(other.line)
  {
  }

  Item & operator=( const Item & other )
  {
	  name = other.name;
	  beg_close = other.beg_close;
	  end_close = other.end_close;
	  options = other.options;
	  subitems = other.subitems;
	  parent = other.parent;
	  text = other.text;
	  start_pos = other.start_pos;
	  end_pos = other.end_pos;
	  line = other.line;

	  return *this;
  }

  std::string get_option( const std::string &name_ )
  {
    std::string s;
    get_option( name_, s );
    return s;
  }

  bool is_set( const std::string &name_ )
  {
    std::string s;
    return get_option( name_, s );
  }

  bool change_option( const std::string &name_, std::string value )
  {
    return get_option( name_, value, true );
  }
  
  Item* clone( Item* parent = 0);

  void set_option( const std::string &name, const std::string &value );

  private:
  bool get_option( const std::string &name, std::string &value, bool change = false );
};

bool parse_option( const std::string &s, Item::Option &option );
bool parse_tag( std::string s, Item &item );
bool parse( const std::string &s, Item &item );
bool read_file( const std::string &file, std::string &s );
std::string print_tree( Item &item );

bool operator==( const Item &a, const Item &b );
inline bool operator!=( const Item &a, const Item &b )
  {
    return !( a == b );
  }

inline bool operator==( const Item::Option &a, const Item::Option &b )
  {
    return a.name == b.name && a.value == b.value;
  }

inline bool operator!=( const Item::Option &a, const Item::Option &b )
  {
    return !( a == b );
  }

std::ostream & operator<<( std::ostream & out, const Item &item );
std::ostream & operator<<( std::ostream & out, const Item::Option &option );

} // namespace XML

} // namespace Tools

#endif
