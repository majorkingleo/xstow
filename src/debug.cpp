#include "debug.h"
#include "string_utils.h"
#include "config.h"

Format::PrintF<std::ostream> out( std::cout );
Format::PrintF<std::ostream> vout( std::cout );
Format::PrintF<std::ostream> eout( std::cerr );

std::string progname = PACKAGE;

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
