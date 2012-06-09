/*
 * $Log: debug.cpp,v $
 * Revision 1.3  2005/07/04 21:59:42  martin
 * added logging to all files
 *
 */
#include "debug.h"
#include "string_utils.h"

Format::PrintF<std::ostream> out( std::cout );
Format::PrintF<std::ostream> vout( std::cout );
Format::PrintF<std::ostream> eout( std::cerr );

std::string progname = PACKAGE;
/*
DModule string2module( std::string m )
{
  m = toupper( m );

  if( m == "ALL" )
    return MODULE_ALL;

  if( m == "MAIN" )
    return  MODULE_MAIN;

  if( m == "ARG" )
    return MODULE_ARG;       

  if( m == "TREE" )
    return MODULE_TREE;

  if( m == "CPPDIR" )
    return MODULE_CPPDIR;
  
  if( m == "SETUP" )
    return MODULE_SETUP;

  if( m == "NIGNORE" )
    return MODULE_NIGNORE;

  if( m == "MERGE_INFO" )
      return MODULE_MERGE_INFO;

  out[0]( "unkown module name \"%s\". Using default value ALL\n", m );

  return MODULE_ALL;
}

std::string x2s( DModuleRange module )
{
  switch( module )
    {
    case MODULE_ALL:        return "ALL";
    case MODULE_ARG:        return "ARG";
    case MODULE_MAIN:       return "MAIN";
    case MODULE_TREE:       return "TREE";
    case MODULE_CPPDIR:     return "CPPDIR";
    case MODULE_SETUP:      return "SETUP";
    case MODULE_NIGNORE:    return "NIGNORE";
    case MODULE_MERGE_INFO: return "MERGE_INFO";
    }

  return "UNKNOWN";
}
*/

MODULE::ETYPE string2module( std::string m )
{
  m = toupper( m );

  if( m == "ALL" )
    return MODULE::ALL;

  if( m == "MAIN" )
    return  MODULE::MAIN;

  if( m == "ARG" )
    return MODULE::ARG;       

  if( m == "TREE" )
    return MODULE::TREE;

  if( m == "CPPDIR" )
    return MODULE::CPPDIR;
  
  if( m == "SETUP" )
    return MODULE::SETUP;

  if( m == "NIGNORE" )
    return MODULE::NIGNORE;

  if( m == "MERGE_INFO" )
      return MODULE::MERGE_INFO;

  out[0]( "unkown module name \"%s\". Using default value ALL\n", m );

  return MODULE::ALL;
}

std::string x2s( MODULE module )
{
  switch( module )
    {
    case MODULE::ALL:        return "ALL";
    case MODULE::ARG:        return "ARG";
    case MODULE::MAIN:       return "MAIN";
    case MODULE::TREE:       return "TREE";
    case MODULE::CPPDIR:     return "CPPDIR";
    case MODULE::SETUP:      return "SETUP";
    case MODULE::NIGNORE:    return "NIGNORE";
    case MODULE::MERGE_INFO: return "MERGE_INFO";
	case MODULE::FIRST__:
	case MODULE::LAST__:
		break;
    }

  return "UNKNOWN";
}
