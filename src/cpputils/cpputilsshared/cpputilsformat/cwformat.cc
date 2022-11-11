/*
 * Class for formating iostream with printf style format strings.
 * std::wstring version
 * @author Copyright (c) 2001 - 2022 Martin Oberzalek
 */
#include "cwformat.h"
#include <iomanip>
#include <iostream>

#include <sstream>


namespace Tools {

void WFormat2::CWFormat::set( std::wostream& out )
{
  if( !valid )
    {
      return;
    }
  /*
  printf( "valid: %d\n", valid );
  printf( "adjust: %d\n", adjust );
  printf( "special: %d\n", special );
  printf( "precision: %d\n", precision );
  printf( "precision_explicit: %d\n", precision_explicit );
  printf( "zero: %d\n", zero );
  printf( "sign: %d\n", sign );
  printf( "width: %d\n", width );
  printf( "internal: %d\n", internal );
  printf( "setupper: %d\n", setupper );
  printf( "grouping: %d\n", grouping );
  printf( "conversion: %d\n", conversion );
  printf( "base: %d\n", base );
  printf( "floating: %d\n", floating );
  printf( "showbase: %d\n", showbase );
  printf( "strlength: %d\n", strlength );
  */
  if( base == HEX && special && showbase && zero )
  {
      // without this correction:
      // printf( "[%#08x]", 0x42 ) => [0x000042]
      // fromat( "[%#08x]", 0x42 ) => [00000x42]

      showbase = false;
      out << '0' << ( setupper ? 'X' : 'x' );
      width -= 2;
  }

  if( base == HEX && special && showbase && strlength )
  {
      /*
	 sprintf( buffer, "[%#8.3x]", 0x42 ) => [   0x042]
      */

      showbase = false;

      if( width )
      {
		  /* For M$ Compile */ {
			  for( int i = 0; i + strlength + 2 + 1 < width; ++i )
				  out << ' ';
		  }

	  width = 0;
      }

      out << '0' << ( setupper ? 'X' : 'x' );
	  /* For M$ Compile */  {
		  for( int i = 0; i + strlength < precision; ++i )
			  out << '0';
	  }
  }

  if( adjust == LEFT && zero )
  {
/*
      sprintf( buffer, "[%-#08x]", 0x42 ); => [0x42    ]
      not => [0x420000]
*/
      zero = false;
  }

  switch( adjust )
    {
    case LEFT: out.setf( std::ios::left, std::ios::adjustfield ); break;
    case RIGHT: out.setf( std::ios::right, std::ios::adjustfield ); break;
    }

  if( zero ) out << std::setfill(L'0');
  else out << std::setfill( L' ' );

  if( sign ) out.setf( std::ios::showpos );
    else out.unsetf( std::ios::showpos );

  if( internal )
    out.setf( std::ios::internal, std::ios::adjustfield );

  switch( base )
    {
    case OCT: out.setf( std::ios::oct, std::ios::basefield ); break;
    case DEC: out.setf( std::ios::dec, std::ios::basefield ); break;
    case HEX: out.setf( std::ios::hex, std::ios::basefield ); break;
    }

  if( setupper ) out.setf( std::ios::uppercase );
  else out.unsetf( std::ios::uppercase );

  switch( floating )
    {
    case FIXED: out.setf( std::ios::fixed, std::ios::floatfield ); break;
    case SCIENTIFIC: out.setf( std::ios::scientific, std::ios::floatfield ); break;
    }

  if( showbase )
    out.setf( std::ios::showbase );
  else
    out.unsetf( std::ios::showbase );

  out << std::setw( width );
  out << std::setprecision( precision );
}

} // namespace Tools

