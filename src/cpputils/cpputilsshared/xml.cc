#include "xml.h"
#include "string_utils.h"
#include <iostream>
#include <fstream>

using namespace Tools::XML;
using namespace std;

namespace Tools {

bool XML::Item::get_option( const std::string &option, std::string &s, bool change )
{
  for( unsigned i = 0; i < options.size(); i++ )
    {
      if( options[i].name == option )
	{
	  if( change )
	    {
	      options[i].value = s;
	    }
	  else
	    s = options[i].value;
	  
	  return true;
	}
    }

  for( unsigned i = 0; i < options.size(); i++ )
    {
      if( icase_cmp( options[i].name, option ) )
	{
	  if( change )
	    {
	      options[i].value = s;
	    }
	  else
	    s = options[i].value;

	  return true;
	}
    }

  return false;
}

void XML::Item::set_option( const std::string &name_, const std::string &value_ )
{
	std::string s;

	if( is_set( name_ ) )
		change_option( name_, value_ );
	else
		options.push_back( Option( name_, value_ ) );
}

XML::Item* XML::Item::clone( Item *parent_ )
{
  Item *i = new Item();

  i->parent = parent_;
  i->name = name;
  i->options = options;
  i->text = text;
  i->start_pos = start_pos;
  i->end_pos = end_pos;
  i->line = line;

  for( unsigned j = 0; j < subitems.size(); j++ )
    {
      i->subitems.push_back( subitems[j]->clone( this ) );
    }

  return i;
}

bool XML::parse_option( const std::string &s, Item::Option &option )
{
  if( s.empty() )
    return false;

  std::vector<std::string> sl = split_simple( s, "=", 2 );

  if( !sl.size() )
    return false;

  option.name = sl[0];

  if( sl.size() > 1 )
    {
      sl[1] = strip( sl[1] );
      option.value = strip( sl[1], "\"" );
    }
  else
   {
#if __GNUC__ > 2
    option.value.clear();
#else
    option.value = std::string();
#endif
   }	

  return true;
}

bool XML::parse_tag( std::string s, Item &item )
{
  if( s.empty() )
    return false;

  if( !s.size() )
    return false;

  if( !s[0] == '<' )
    return false;

  if( !s[s.size()-1] == '>' )
    return false;

  item.end_close = false;

  if( s.size() > 2 )
    item.end_close = s[s.size()-2] == '/';

  item.beg_close = s[1] == '/';
    
  std::string::size_type len = s.size() - 2;

  if( item.end_close )
    len--;
    
  std::string::size_type start = 1;

  if( item.beg_close )
    {
      start = 2;
      len--;
    }

  s = s.substr( start, len );

  std::vector<std::string> sl = split_safe( s );

  if( !sl.size() )
    return false;

  if( !sl[0].size() )
    return false;

  item.name = sl[0];

  for( unsigned i = 1; i < sl.size(); i++ )
    {
      Item::Option o;

      if( !parse_option( sl[i], o ) )
	continue;

      item.options.push_back( o );
    }

  return true;
}

bool XML::operator==( const Item &a, const Item &b )
{
  if( a.name != b.name )
    return false;

  if( a.beg_close != b.beg_close )
    return false;

  if( a.end_close != b.end_close )
    return false;

  if( a.options.size() != b.options.size() )
    return false;

  for( unsigned i = 0; i < a.options.size(); i++ )
    {
      if( a.options[i] != b.options[i] )
	return false;
    }

  return true;
}

std::ostream & XML::operator<<( std::ostream & out, const Item &item )
{
  out << '<';
  
  if( item.beg_close )
    out << '/';

  out << item.name;

  for( unsigned i = 0; i < item.options.size(); i++ )
    {
      out << ' ' << item.options[i];
    }

  if( item.end_close )
    out << '/';

  out << '>';

  return out;
}

std::ostream & XML::operator<<( std::ostream & out, const Item::Option &option )
{
  out << option.name;

  if( option.value.size() )
    {
      out << "=\"" << option.value << '\"';
    }

  return out;
}

static bool find_next_tag( const std::string &s, std::string::size_type pos,
			   std::string::size_type &start,
			   std::string::size_type &end )
{
  start = s.find( '<', pos );

  if( start == std::string::npos )
    return false;

  end = s.find( '>', start );

  if( end == std::string::npos )
    return false;

  return true;
}

static unsigned count_lines( const std::string &s, std::string::size_type pos )
{
  std::string::size_type p = 0;
  unsigned lines = 0;

  while( p < pos )
    {
      p = s.find( '\n', p + 1 );

      lines++;

      if( p == std::string::npos )
	break;
    }

  return lines;
}

bool XML::parse( const std::string &s, Item &item )
{
  std::string::size_type start, end, pos = 0;

  Item *current = &item;

  while( find_next_tag( s, pos, start, end ) )
    {
      Item i;

      if( parse_tag( s.substr( start, end - start + 1 ), i ) )
	{
	  //	  cout << "item: " << i << endl;
  
	  if( !i.beg_close && !i.end_close )
	    {
	      Item *ni = new Item(i);
	      ni->parent = current;
	      ni->start_pos = start;
	      ni->end_pos = end;
	      ni->line = count_lines( s, start );

	      current->subitems.push_back( ni );
	      current = ni;
	    }
	  else if( i.end_close )
	    {
	      Item *ni = new Item(i);
	      ni->parent = current;
	      ni->line = count_lines( s, start );

	      current->subitems.push_back( ni );
	    }
	  else if( i.beg_close )
	    {
	      for( Item *ni = current; ni != 0; ni = ni->parent )
		{
		  if( ni->name == i.name )
		    {
		      if( ni == current && !ni->subitems.size())
			ni->text = s.substr( ni->end_pos + 1, start - ni->end_pos - 1 );

		      current = ni->parent;

		      if( current == 0 )
			{
			  unsigned line = 0;

			  for( std::string::size_type p = 0; p < pos; p++ )
			    if( s[p] == '\n' )
			      line++;

			  cout << "parse error! missing tags! at line: " << line << endl;
			  
			  return false;
			}

		      break;
		    }
		}
	    }
	}

      pos = end;
    }

  //  cout << "tree: " << print_tree( item ) << endl;

  return true;
}

static std::string print_item( Ref<Item> item, unsigned inc )
{
  std::string s;

  for( unsigned i = 0; i < inc; i++ )
    s += ' ';

  s += x2s( *item ) + '\n';

  if( item->text.size() )
    s += item->text + '\n';

  if( item->subitems.size() )
    {
      for( unsigned i = 0; i < item->subitems.size(); i++ )
	s += print_item( item->subitems[i], inc + 4 );
    }

  return s;
}

std::string XML::print_tree( Item &item )
{
  return print_item( Ref<Item>( &item, false ), 0 );
}

bool XML::read_file( const std::string &file, std::string &s )
{
  std::ifstream in( file.c_str() );

  if( !in )
    {
      return false;
    }

#ifndef _WIN32	
#if __GNUC__ > 2
  in.seekg( 0, std::ios_base::end );
#else
  in.seekg( 0, std::ios::end );
#endif

#ifdef __GNUC__
  s.resize( in.tellg() );
#else
  s.reserve( in.tellg() );
#endif

#if __GNUC__ > 2
  in.seekg( 0, std::ios_base::beg );
#else
  in.seekg( 0, std::ios::beg );
#endif

  in.clear();

#ifdef __GNUC__
  // undefined behavior but works on gnu stdc++ lib
  // and is the fasted way reading a file
  in.read( &s[0], s.size() );
#else
  // not very nice, but not muchslower than the example above
  while( !in.eof() )
    {
      std::string ss;

      getline( in, ss );
      
#ifdef _WIN32	  
      s += ss + "\r\n";
#else	  
      s += ss + '\n';
#endif	  
    }
#endif 
#else
  // that's the most beatiful way reading a file
  // but is very slow on __GNUC__ <= 3 
  std::istreambuf_iterator<char> begin(in);
  std::istreambuf_iterator<char> end;

  s = std::string( begin, end );
#endif  
  return true;
}

} // namespace Tools
