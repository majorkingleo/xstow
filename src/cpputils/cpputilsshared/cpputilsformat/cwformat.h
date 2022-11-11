/**
 * Helper class for formating iostream with printf style format strings.
 * std::wstring version
 * @author Copyright (c) 2001 - 2022 Martin Oberzalek
 */

#ifndef _TOOLS_CWFORMAT_H
#define _TOOLS_CWFORMAT_H

#include <string>

namespace Tools {
  namespace WFormat2 {

    class CWFormat
    {
    public:

      enum Adjust
      {
        LEFT,
        RIGHT
      };

      enum Base
      {
        OCT,
        DEC,
        HEX
      };

      enum Floating
      {
        FIXED,
        SCIENTIFIC
      };

      bool valid;
      Adjust adjust;
      bool special;
      bool sign;
      bool grouping; // SUSv2 extension
      bool conversion; // glibc 2.2 extension
      bool zero;
      bool precision_explicit;
      bool internal;
      Base base;
      bool setupper;
      Floating floating;
      bool showbase;

      int width;
      int precision;
      int strlength;

      std::wstring format;

      bool numerical_representation; // cast a character to int
      bool character_representation; // cast a int to char

    public:
      CWFormat() :
        valid(false),
        adjust(RIGHT),
        special(false),
        sign(false),
        grouping(false),
        conversion(false),
        zero(false),
        precision_explicit(false),
        internal(false),
        base(DEC),
        setupper(false),
        floating(FIXED),
        showbase(false),
        width(0),
        precision(6),
        strlength(0),
        format(),
        numerical_representation(false),
        character_representation(false)
    { }

      void set( std::wostream& out );
    };

  } // /namespace Format

  inline std::wostream& operator<<( std::wostream& out, WFormat2::CWFormat cf )
  {
    cf.set( out );
    return out;
  }

} // /namespace Tools

#endif  /* _TOOLS_CFORMAT_H */
