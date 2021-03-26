/**
 * @file
 * @todo describe file content
 * @author Copyright (c) 2019 Salomon Automation GmbH
 */

#ifndef _wamas_CFORMAT_H
#define _wamas_CFORMAT_H

namespace Tools {
  namespace Format {

    class CFormat
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

      std::string format;

      bool numerical_representation; // cast a character to int
      bool character_representation; // cast a int to char

    public:
      CFormat() :
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
        format(""),
        numerical_representation(false),
        character_representation(false)
    { }

      void set( std::ostream& out );
    };

  } // /namespace Format

  inline std::ostream& operator<<( std::ostream& out, Format::CFormat cf )
  {
    cf.set( out );
    return out;
  }

} // /namespace Tools

#endif  /* _wamas_CFORMAT_H */
