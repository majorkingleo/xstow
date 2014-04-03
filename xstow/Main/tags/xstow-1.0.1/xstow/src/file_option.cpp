/*
 * $Log: file_option.cpp,v $
 * Revision 1.3  2005/07/04 21:59:42  martin
 * added logging to all files
 *
 */
#include "file_option.h"
#include "cppdir.h"

using namespace CppDir;

Arg::FileOptionRule::FileOptionRule( FILE type )
  : AnyStringRule(),
    type( type )
{
  continue_on_fail = false;

  error = "value is not a valid ";

  switch( type )
    {
    case FILE::DIR:     description = "DIR";  error += " directory";         break;
    case FILE::LINK:    description = "LINK"; error += " symlink";           break;
    case FILE::REGULAR: description = "FILE"; error += " regular file";      break;
    case FILE::ANY:     description = "FILE"; error += " file or directory"; break;
    case FILE::LINK_TO_DIR: description = "LINK"; error += " link to directory"; break;
    case FILE::LINK_TO_REGULAR: description = "LINK"; error += " link to a regular file"; break;
	case FILE::FIRST__:
	case FILE::LAST__:
		break;
    }
}

bool Arg::FileOptionRule::match( const std::string &name ) const
{
  File file( name );

  if( !file )
    return false;

  switch( type )
    {
    case FILE::LINK_TO_DIR: if( (file.get_type() == EFILE::DIR) && file.is_link() ) return true; break;
    case FILE::LINK_TO_REGULAR: if( file.get_type() == EFILE::REGULAR && file.is_link() ) return true; break;
    case FILE::DIR:     if( file.get_type() == EFILE::DIR ) return true; break;
    case FILE::LINK:    if( file.is_link() ) return true; break;
    case FILE::REGULAR: if( file.get_type() == EFILE::REGULAR ) return true; break;
    case FILE::ANY:     return true;
	case FILE::FIRST__:
	case FILE::LAST__:
		break;
    }

  return false;
}


Arg::FileOption::FileOption( FILE type )
  : StringOption()
{
  value_rule = new FileOptionRule( type );

  setMaxValues( 1 );
  setMinValues( 1 );
}

Arg::FileOption::FileOption( const std::string &name, FILE type )
  : StringOption( name )
{
  value_rule = new FileOptionRule( type );

  setMaxValues( 1 );
  setMinValues( 1 );
}

Arg::EmptyFileOption::EmptyFileOption( FILE type )
  : EmptyStringOption()
{
  value_rule = new FileOptionRule( type );

  setMaxValues( 1 );
  setMinValues( 1 );
}
