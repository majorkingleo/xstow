/*
 * $Log: leoini.cpp,v $
 * Revision 1.4  2010/07/22 21:44:21  martin
 * Release of Verion 1.0.0
 *
 * Revision 1.3  2005/07/04 21:59:42  martin
 * added logging to all files
 *
 */
#include "leoini.h"
#include <iostream>


#ifdef CAN_USE_INI

bool Leo::Ini::Element::add( Element& element )
{
  if( type == TYPE::SECTION && element.type == TYPE::KEY )
    {
      element.section = section;
      elements.push_back( element );
      return true;
    }
  
  if( type == TYPE::SECTION_LIST && element.type == TYPE::SECTION )
    {
      elements.push_back( element );
      return true;
    }

  return false;
}

void Leo::Ini::MemElement::clear()
{
  line.clear();
  type = TYPE::UNDEF;
  elements.clear();
  mem_elements.clear();
  section = "";
  key = "";
  value = "";
}

/*** creates an Element from a MemElement */
Leo::Ini::Element Leo::Ini::MemElement::get_element()
{
  Element e( type, section, key, value );

  if( type == TYPE::SECTION || type == TYPE::SECTION_LIST )
    for( mem_element_list_it it = mem_elements.begin();
	 it != mem_elements.end(); it++ )
      e.elements.push_back( it->get_element() );

  return e;
}


std::string Leo::Ini::Line::get_line()
{
  str = "";
  
  for( std::string::size_type i = 0; i < tag.pos; i++ )
    str += ' ';
  
  str += tag.str;
  
  for( std::string::size_type i = 0; i < comment.pos; i++ )
    str += ' ';
  
  if( !comment.str.empty() )
    str += ';' + comment.str;

  return str;
}

void Leo::Ini::MemElement::operator=(const Element& e )
{
  section = e.section;
  key = e.key; 
  value = e.value;
  type = e.type; 

  if( type == TYPE::SECTION )
    {
      line.tag.str = '[' + section + ']';
      line.get_line();
    }
  else if( type == TYPE::KEY )
    {
      line.tag.str = key + " = " + value;
      line.get_line();
    }

  if( type == TYPE::SECTION || type == TYPE::SECTION_LIST )
    for( element_list_it it = e.elements.begin();
	 it != e.elements.end(); it++ )
      *this = *it;
}

Leo::Ini::Ini( std::string filename, int mode )
{
  is_open = false;
  valid = false;
  eof_reached = false;
  open( filename, mode );
}

Leo::Ini::~Ini()
{
  close();
}

bool Leo::Ini::open( std::string f, int mode )
{
  if( is_open )
    return false;

  file_name = f;
  openmode = mode;
  eof_reached = false;
  file_readed = false;
  line_number = 0;
  changed = false;

  file.open( file_name.c_str() , static_cast<std::ios_base::openmode>( mode ));

  if( !file )
    {
      valid = false;
      is_open = false;
      return false;
    }

  is_open = true;
  valid = true;

  return true;
}

void Leo::Ini::close()
{
  flush();
  file.close();

  // free the memory
  clear();

  file_readed = false;
  changed = false;
}

Leo::Ini::Line Leo::Ini::read_line()
{
  Line line;

  while( true )
    {
      std::getline( file, line.str );    
      
      line.number = line_number;
      line_number++;

      if( file.eof() )
	{
	  eof_reached = true;
	  line_number = 0;
	}
      
      if( !strip( line.str ).empty() )
	{
	  // comment
	  std::string::size_type pos = find_comment( line.str );
	  if( pos != std::string::npos )
	    {
	      line.comment.pos = pos;
	      line.comment.str = line.str.substr( pos+1 );
	    }
	  
	  // tag
	  std::string::size_type end_pos;
	  if( find_tag( pos, end_pos, line.str.substr(0,pos) ) )
	    {
	      line.tag.pos = pos;
	      line.tag.str = line.str.substr( pos, end_pos - pos + 1 );
	    }
	  break; // we have a valid line
	}
      
      if( eof_reached )
	break;
    }

  return line;
}

std::string Leo::Ini::strip( std::string str, std::string what )
{
  if( str.empty() ) 
    return "";

  std::string::size_type start = str.find_first_not_of( what );
  std::string::size_type end = str.find_last_not_of( what );

  if( start == std::string::npos || end ==  std::string::npos )
    return "";

  return str.substr( start, end - start +1 );
}

/// reads the file
bool Leo::Ini::read()
{
  if( !is_open )
    return false;

  if( file_readed )
    return false;

  MemElement current_section;
  bool in_section = false;

  while( true )
    {
      Line line = read_line();

      if( eof_reached && line.str.empty() )
	break;

      if( line.tag.str.empty() )
	{
	  // simple comment
	  comments.push_back( line );
	  continue;
	}

      if( is_section( line.tag.str ) )
	{
	  if( in_section )
	    {
	      elements.push_back( current_section );
	      current_section.clear();
	    }
	  
	  // create a new section
	  current_section.line = line;
	  current_section.section = extract_section_name( line.tag.str );
	  current_section.type = Element::TYPE::SECTION;

	  in_section = true;
	}
      else if( is_key( line.tag.str ) && in_section )
	{
	  // create a new key
	  MemElement me;

	  me.line = line;
	  me.section = current_section.section;
	  me.key = extract_key_name( line.tag.str );
	  me.value = extract_key_data( line.tag.str );
	  me.type = Element::TYPE::KEY;

	  current_section.mem_elements.push_back( me );
	}
    }

  if( !current_section.section.empty() )
    elements.push_back( current_section );

  file_readed = true;

  return true;
} 

std::string::size_type Leo::Ini::find_comment( const std::string& str )
{
  // maybe I'll add later an better implementation if this
  return str.find_first_of( ";#" );
}

bool Leo::Ini::find_tag( std::string::size_type& start, std::string::size_type& end, const std::string& str )
{
  start = str.find_first_not_of( " \t" );
  if( start == std::string::npos )
    return false;

  end = str.find_last_not_of( " \t" );
  if( end == std::string::npos )
    return false;

  if( start >= end )
    return false;

  return true;
}

bool Leo::Ini::is_section( const std::string& str )
{
  std::string::size_type size = str.size();

  if( size <= 2 )
    return false;

    if( str[0] == '[' && str[size-1] == ']' )
    {
      if( !strip(str.substr( 1, size - 2 )).empty() )
	return true;
    }
  return false;
}

bool Leo::Ini::is_key( const std::string& str )
{
  if( str.size() < 2 )
    return false;

  std::string::size_type pos = str.find( '=' );

  if( pos == 0 || pos == std::string::npos )
    return false;

  return true;
}

std::string Leo::Ini::extract_key_name( const std::string& str )
{
  std::string::size_type pos = str.find_first_of( " \t=" );
  return str.substr( 0, pos );
}

std::string Leo::Ini::extract_key_data( const std::string& str )
{
  std::string::size_type start = str.find( '=' );
  std::string::size_type pos = str.find_first_not_of( " \t=", start );

  if( pos == std::string::npos )
    return "";

  return str.substr( pos );
}

std::string Leo::Ini::extract_section_name( const std::string& str )
{
  return strip( str.substr( 1, str.size()-2 ) );
}


void Leo::Ini::print_mem()
{
  for( MemElement::mem_element_list_it sit = elements.begin();
       sit != elements.end(); sit++ )
    {
      std::cout << *sit << std::endl;
      for( MemElement::mem_element_list_it kit = sit->mem_elements.begin();
	   kit != sit->mem_elements.end(); kit++ )
	std::cout << *kit << std::endl;
    }

  for( line_list_it it = comments.begin();
       it != comments.end(); it++ )
    std::cout << "Comment: " << it->str << std::endl;
  
}

bool Leo::Ini::read( Element& element )
{
  if( !file_readed )
    if( !read() )
      return false;

  if( !is_good_element( element ) )
    return false;

  switch( element.type )
    {
    case Element::TYPE::SECTION_LIST:
      // give it all
      for( MemElement::mem_element_list_it it = elements.begin();
	   it != elements.end(); it++ )
	element.elements.push_back( it->get_element() );
      return true;
      break;
    case Element::TYPE::SECTION:
      for( MemElement::mem_element_list_it it = elements.begin();
	   it != elements.end(); it++ )
	if( element.section == it->section )
	  {
	    element = it->get_element();
	    return true;
	  }
      break;
    case Element::TYPE::KEY:
      for( MemElement::mem_element_list_it it = elements.begin();
	   it != elements.end(); it++ )
	if( element.section == it->section )
	  {
	    for( MemElement::mem_element_list_it kit = it->mem_elements.begin();
		 kit != it->mem_elements.end(); kit++ )
	      if( kit->key == element.key )
		{
		  element = kit->get_element();
		  return true;
		}
	  }
      break;
    case Element::TYPE::UNDEF:
    default:
      break;
    }

  return false;
}

bool Leo::Ini::is_good_element( Element& e )
{
  switch( e.type )
    {
    case Element::TYPE::KEY:
      e.key = strip( e.key );
      e.section = strip( e.section );
      if( e.key.empty() || e.section.empty() ) return false;

    case Element::TYPE::SECTION:
      e.section = strip( e.section );
      if( e.section.empty() ) return false;
      break;

    case Element::TYPE::SECTION_LIST: break;

    case Element::TYPE::UNDEF: 

    default: 
      return false;
    }
      

  return true;
}

bool Leo::Ini::is_good_element_rec( Element& element )
{
  if( !is_good_element( element ) )
    return false;

  if( element.type == Element::TYPE::SECTION || element.type == Element::TYPE::SECTION_LIST )
    for( Element::element_list_it it = element.elements.begin();
	 it != element.elements.end(); it++ )
      if( !is_good_element_rec( *it ) )
	return false;

  return true;
}

bool Leo::Ini::write( Element element )
{
  if( is_open )
    if( openmode & std::ios_base::out != std::ios_base::out )
      return false; // file opened in read_only mode

  if( !is_good_element_rec( element ) )
    return false;

  /// the data will be changed now
  changed = true;

  MemElement me;

  switch( element.type )
    {
    case Element::TYPE::SECTION_LIST:
      clear();
      for( Element::element_list_it it = element.elements.begin();
	   it != element.elements.end(); it++ )
	write( *it );
      return true;

    case Element::TYPE::SECTION:
      for( Element::element_list_it it = element.elements.begin();
	   it != element.elements.end(); it++ )
	write( *it );
      return true;

    case Element::TYPE::KEY:
      for( MemElement::mem_element_list_it it = elements.begin();
	   it != elements.end(); it++ )
	if( it->section == element.section )
	  {
	    for( MemElement::mem_element_list_it kit = it->mem_elements.begin();
		 kit != it->mem_elements.end(); kit++ )
	      if( kit->key == element.key )
		{
		  *kit = element;
		  return true;
		}
	    // element does not exist in section
	    // lets create it
	    it->mem_elements.push_back( element );
	    return true;
	  }
      // section for the element does not exist
      me = MemElement( Element( Element::TYPE::SECTION, element.section ));
      me.mem_elements.push_back( element );
      elements.push_back( me );
      return true;
      
    case Element::TYPE::UNDEF:
    default:
      
      break;
    }

  return false;
}

void Leo::Ini::clear()
{
  elements.clear();
  comments.clear();

  changed = true;
}

bool Leo::Ini::write()
{
  if( !is_open )
    return false;

  if( openmode & std::ios_base::out != std::ios_base::out )
    return false;

  flush();
      
  return true;
}

void Leo::Ini::flush()
{
  if( !is_open )
    return;

  if( openmode & std::ios_base::out != std::ios_base::out )
    return;
  
  if( !changed )
    return;

  file.close();

  // erase the file
  file.open( file_name.c_str(), std::ios_base::trunc | std::ios_base::in | std::ios_base::out );
  file.close();

  file.open( file_name.c_str(), static_cast<std::ios_base::openmode>(openmode) );

  int last_line = 1;

  Line line;

  MemElement::mem_element_list_it mit, bak_mit;
  mit = bak_mit = elements.begin();
  line_list_it cit = comments.begin();


  while( true )
    {
      int next_comment_at = 0;

      if( cit != comments.end() )
	next_comment_at = cit->number;
      else
	next_comment_at = -1;

      int next_element_at = 0;

      if( mit != elements.end() )
	next_element_at = mit->line.number;
      else
	next_element_at = -1;

      if( next_comment_at == -1 && next_element_at == -1 )
	break;

      int next = 0;

      if( ((next_comment_at < next_element_at) || next_element_at == -1 ) && next_comment_at != -1 )
	{
	  next = next_comment_at;
	  line = *cit;
	  
	  cit++;
	}
      else if( next_element_at != -1 )
	{
	  next = next_element_at;	  
	  line = mit->line;
	  
	  if( mit->type == Element::TYPE::SECTION )
	    {
	      bak_mit = mit;
	      mit = mit->mem_elements.begin(); 
	    }
	  else
	    {
	      mit++;
	      if( mit == bak_mit->mem_elements.end() )
		{
		  mit = bak_mit;
		  mit++;
		}
	    }
	}

      write_line( line, last_line );

      last_line = line.number;
      
    }

  changed = false;
}

bool Leo::Ini::write_line( const Line& line, int last_line )
{
  for( int i = last_line+1; i < line.number; i++)
    file << std::endl;

  file << line.str << std::endl;

  return true;
}

bool Leo::Ini::erase()
{
  if( is_open )
    if( openmode & std::ios_base::out == std::ios_base::out )
      {
	clear();

	return true;
      }
  return false;
}

bool Leo::Ini::erase( Element element )
{
  if( !is_open ||( openmode & std::ios_base::out != std::ios_base::out ) )
    return false;
 
  if( !is_good_element_rec( element ) )
    return false;

  changed = true;

  MemElement me;

  switch( element.type )
    {
    case Element::TYPE::SECTION_LIST:
      clear();
      for( Element::element_list_it it = element.elements.begin();
	   it != element.elements.end(); it++ )
	erase( *it );
      return true;

   case Element::TYPE::SECTION:
      for( MemElement::mem_element_list_it it = elements.begin();
	   it != elements.end(); it++ )
	if( it->section == element.section )
	  {
	    for( Element::element_list_it kit = element.elements.begin();
		 kit != element.elements.end(); kit++ )
	      erase( *kit );

	    if( is_section_empty( *it ) )
	      {
		// delete section only if it is empty
		elements.remove( *it );
	      }
	    return true;
	  }
      // section not found, so return false
      return false;


    case Element::TYPE::KEY:
      for( MemElement::mem_element_list_it it = elements.begin();
	   it != elements.end(); it++ )
	{
	  if( it->section == element.section )
	    {
	      for( MemElement::mem_element_list_it kit = it->mem_elements.begin();
		   kit != it->mem_elements.end(); kit++ )
		{
//		  cout << kit->key << " == " << element.key << " => "; cout.flush();
		  
		  if( kit->key == element.key )
		    {
//		      cout << "true" << endl; cout.flush();
		      it->mem_elements.remove( *kit );
		      if( is_section_empty( *it ) )
			elements.remove( *it );
		      return true;
		    }
		  
//		  cout << "false" << endl; cout.flush();
		}
	      // element does not exist in section
	      return false;
	    }
	}
      // section for the element does not exist
      return false;
      
    case Element::TYPE::UNDEF:
    default:
      
      break;
    }

  return false;
}

bool Leo::Ini::is_section_empty( MemElement& element )
{
  if( element.type == Element::TYPE::SECTION )
    {
       MemElement::mem_element_list_it it = element.mem_elements.begin();
       if( it == element.mem_elements.end() )
	 return true;;
    }
  return false;
}

bool Leo::operator == ( const Ini::MemElement& a, const Ini::MemElement& b )
{
  if( a.type == b.type && 
      a.section == b.section &&
      a.key == b.key )
    return true;

  return false;
}

std::ostream& operator << ( std::ostream& out, const Leo::Ini::Element& e )
{
  out << "Section: " << e.section;

  if( !e.key.empty() )
    out << " Key: " << e.key;
    
  if( !e.value.empty() )
    out << " Value: " << e.value;

  switch( e.type )
    {
    case Leo::Ini::Element::TYPE::SECTION_LIST: out << " type: SECTION_LIST "; break;
    case Leo::Ini::Element::TYPE::SECTION: out << " type: SECTION "; break;
    case Leo::Ini::Element::TYPE::KEY: out << " type: KEY "; break;
    case Leo::Ini::Element::TYPE::UNDEF: out << " type: UNDEF"; break;
	case Leo::Ini::Element::TYPE::LAST__:
	case Leo::Ini::Element::TYPE::FIRST__:
		break;
    }
  
  return out;
}

#endif

