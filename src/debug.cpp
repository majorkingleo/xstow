#include "debug.h"
#include "string_utils.h"
#include "config.h"
#include "colored_output.h"

class ColorModule : public Format::PrintF<std::ostream>::FinalizeString
{
	static ColoredOutput CO;

public:

	virtual std::string operator()( int module, const std::string & s ) override
	{
		std::string module_name = module2string( module );
		std::string colored_module_name = CO.color_output(module2color( module ), module_name );

		std::string debug_string = Tools::fill_trailing( colored_module_name, " ", 15 );

		return debug_string + " " + s;
	}

private:
	std::string module2string( int module )
	{
		MODULE m( static_cast<MODULE::ETYPE>(module) );
		std::string module_name = x2s( m );

		return module_name;
	}

	ColoredOutput::Color module2color( int module )
	{
		MODULE m( static_cast<MODULE::ETYPE>(module) );

		switch( m )
		{
		case MODULE::ALL:        return ColoredOutput::GREEN;
		case MODULE::ARG:        return ColoredOutput::GREEN;
		case MODULE::MAIN:       return ColoredOutput::YELLOW;
		case MODULE::TREE:       return ColoredOutput::YELLOW;
		case MODULE::CPPDIR:     return ColoredOutput::GREEN;
		case MODULE::SETUP:      return ColoredOutput::GREEN;
		case MODULE::NIGNORE:    return ColoredOutput::MAGENTA;
		case MODULE::MERGE_INFO: return ColoredOutput::MAGENTA;
		}

		return ColoredOutput::GREEN;
	}
};

ColoredOutput ColorModule::CO;

Format::PrintF<std::ostream> out( std::cout, -1, -1, new ColorModule() );
Format::PrintF<std::ostream> vout( std::cout, -1, -1, new ColorModule() );
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
