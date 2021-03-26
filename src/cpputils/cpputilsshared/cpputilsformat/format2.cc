/**
 * @file
 * @todo describe file content
 * @author Copyright (c) 2019 Salomon Automation GmbH
 */

#include "format2.h"

#if __cplusplus - 0 >= 201103L

using namespace Tools::Format;

namespace Tools {
  namespace Format2 {

    Format2::Format2()
    : args(),
      format(),
      num_of_args(0),
      s()
    {

    }

    Format2::Format2(const Format2 & f)
    : args(),
      format(),
      num_of_args(0),
      s()
    {

    }

    Format2 & Format2::operator=(const Format2 & f)
    {
      return *this;
    }

    Format2::Format2( const std::string &format_, std::vector<BaseArg*> & args_ )
    : args(args_),
      format(format_),
      num_of_args(args.size()),
      s()
    {
      parse();
    }

    int Format2::get_int_arg( int num )
    {
      if( static_cast<unsigned int>(num) > num_of_args - 1 )
        throw BaseException( "The arg you wan't to use is out of range" );

      if( num < 0 )
        throw BaseException( "negativ number for arg number not allowed" );

      if( args[num]->isInt())
        {
          return args[num]->get_int();
        }
      else
        throw BaseException( "expecting int arg" );

      return 0; // should never be reached
    }

    std::string Format2::use_arg( unsigned int i, const Format::CFormat &cf )
    {
      if( i > num_of_args )
        throw BaseException( "out of arg range" );

      std::string s = args[i]->doFormat(cf);

      return s;
    }

    void Format2::parse()
    {
      if( format.empty() )
        return;

      unsigned int par = 0;
      unsigned int use_par = 0;
      std::string::size_type pos = 0;
      std::string::size_type  len = format.size();
      s = "";

      while( par < num_of_args && pos < len )
        { // while
          bool had_precision = false;

          use_par = par;

          if( pos >= len )
            break;

          if( format[pos] != '%' )
            {
              s += format[pos];
              pos++;
              continue;
            }

          // % digit found
          pos++;

          if( !(pos < len ) || (format[pos] == '%') )
            {
              // %% -> %
              s += format[pos];
              pos++;
              continue;
            }

          // format string found

          std::string::size_type start = pos - 1;
          CFormat cf;

          // process flags

          while( (pos < len) )
            {
              bool finished = false;

              switch( format[pos] )
              {
              case '-' : cf.adjust = CFormat::LEFT; break;
              case '+' : cf.sign = true; break;
              case ' ' : cf.zero = false; break;
              case '#' : cf.special = true; break;
              case '\'': cf.grouping = true; break;
              case 'I' : cf.conversion = true; break;
              case '0' : cf.zero = true; break;
              default: finished = true; break;
              }

              if( finished )
                break;

              pos++;
            } //       while( (pos < len) )

          // get argument number
          if( pos < len )
            {
              // search for the $ digit
              unsigned int dp = pos;

              while( dp < len && isdigit( format[dp] ) )
                dp++;

              if( dp < len && format[dp] == '$' )
                {
                  use_par = skip_atoi( format, pos, pos ) - 1;
                  pos = dp + 1;
                }
            }

          // get field with
          if( pos < len )
            {
              if( isdigit( format[pos] ) )
                cf.width = skip_atoi( format, pos, pos );
              else if( format[pos] == '*' )
                {
                  pos++;

                  // search for the $ digit
                  unsigned int dp = pos;

                  while( dp < len && isdigit( format[dp] ) )
                    dp++;

                  if( dp < len && format[dp] == '$' )
                    {
                      cf.width = get_int_arg( skip_atoi( format, pos, pos ) - 1 );
                      // skip $ sign
                      pos++;
                    }
                  else
                    {
                      cf.width = get_int_arg( par );

                      if( use_par == par )
                        use_par++;

                      par++;
                    }

                  if( cf.width < 0 )
                    {
                      cf.width *= -1;
                      cf.adjust = CFormat::LEFT;
                    }
                }
            }

          // precision
          if( pos < len )
            {
              if( format[pos] == '.' )
                {
                  pos++;
                  if( !(pos < len) )
                    return;

                  had_precision = true;

                  if( isdigit( format[pos] ) )
                    cf.precision = skip_atoi( format, pos, pos );
                  else if( format[pos] == '*' )
                    {
                      pos++;


                      // search for the $ digit
                      unsigned int dp = pos;

                      while( dp < len && isdigit( format[dp] ) )
                        dp++;

                      if( dp < len && format[dp] == '$' )
                        {
                          cf.precision = get_int_arg( skip_atoi( format, pos, pos ) - 1 );
                          // skip $ sign
                          pos++;
                        }
                      else
                        {
                          cf.precision = get_int_arg( par );

                          if( use_par == par )
                            use_par++;

                          par++;
                        }

                      if( cf.precision == 0)
                        cf.precision_explicit = true;

                      if( cf.precision < 0 )
                        cf.precision = 0;
                    }
                  else
                    cf.precision = 0;
                }

            }

          // lenght modifier
          /*
         they will be ignored
         cause we know the types of the parameter
           */
          if( (pos < len) )
            {
              bool hh = false;
              bool ll = false;
              bool found = false;

              switch( format[pos] )
              {
              case 'h': hh = true; found = true; break;
              case 'l': ll = true; found = true; break;
              case 'L':
              case 'q':
              case 'j':
              case 'z':
              case 't': found = true; break;
              default: break;
              }

              if(found )
                {
                  pos++;

                  if( pos < len )
                    {
                      if( hh == true )
                        {
                          if( format[pos] == 'h' )
                            pos++;
                        }
                      else if( ll == true )
                        {
                          if( format[pos] == 'l' )
                            pos++;
                        }
                    } // if
                } // found
            }

          // conversion specifier

          if( pos < len )
            {
              bool invalid = false;
              switch( format[pos] )
              {
              case 'd':
              case 'u':
              case 'i':
                cf.numerical_representation = true;
                cf.base = CFormat::DEC;
                if( cf.zero && (cf.adjust != CFormat::LEFT) )
                  cf.internal = true;
                break;

              case 'X': cf.setupper = true;
              case 'x':
                cf.numerical_representation = true;
                cf.base = CFormat::HEX;
                if( cf.special )
                  cf.showbase = true;
                break;

              case 'o':
                cf.numerical_representation = true;
                cf.base = CFormat::OCT;
                if( cf.special )
                  cf.showbase = true;
                break;


              case 'E':
                cf.setupper = true;

              case 'e':
                if( cf.special )
                  cf.sign = true;
                cf.floating = CFormat::SCIENTIFIC;
                break;

              case 'F': // not supported
              case 'f':
                if( cf.special )
                  cf.sign = true;
                cf.floating = CFormat::FIXED;
                break;

              case 's':
                if( cf.zero )
                  cf.zero = false;
                break;


              case 'p':
                cf.base = CFormat::HEX;
                cf.showbase = true;
                break;

                // unsupported modifiers


              case 'G':
              case 'g':

              case 'A':
              case 'a':
                break;

              case 'c':
                cf.character_representation = true;
                break;

              case 'C':
              case 'S':
              case 'P':
              case 'n': break;

              default: invalid = true;
              }

              if( !invalid )
                cf.valid = true;
            }

          if( cf.valid )
            {
              std::string str;
              int upar = par;

              if( use_par != par )
                upar = use_par;

              if( cf.base == CFormat::HEX && had_precision && cf.special )
                {
                  CFormat f2;
                  f2.base = cf.base;
                  std::string ss = use_arg( upar, f2 );
                  cf.strlength = ss.size();
                  //        printf( "str: %s\n", s.c_str() );
                }

              str = use_arg( upar, cf );

              // cut string
              if( had_precision && args[upar]->isString() )
                str = str.substr( 0, cf.precision );

              // cut null bytes out of the string
              // can happen when std::string.resize() is called
              // eg: std::string foo="foo";
              //     foo.resize(4);
              //     std::cout << (foo + "bar").c_str();
              //     will result in only the string "foo"
              //     we avoid this by cutting zeor bytes out

              for( std::string::size_type p = 0; p < str.size(); p++ )
                {
                  if( str[p] ==  '\0' )
                    {
                      str = str.substr( 0, p );
                      break;
                    }
                }

              s += str;

              if( use_par == par )
                par++;
            }
          else
            {
              // copy the invalid format string
              for( std::string::size_type  i = start;  i<= pos; i++ )
                if( i < len )
                  s += format[i];
            }

          pos++;

        } // while

      if( pos < len )
        {
          s += substitude( format.substr(pos), "%%", "%" );
        }
    }

    int Format2::skip_atoi( std::string s, std::string::size_type start, std::string::size_type & pos ) const
    {
      pos = start;
      std::string::size_type len = s.size();

      while( (pos < len) && isdigit( s[pos] ) )
        pos++;

      return atoi( s.substr( start, start-pos ).c_str() );
    }

    std::string Format2::substitude( const std::string & str_orig, const std::string & what, const std::string & with, std::string::size_type start  ) const
    {
      std::string str( str_orig );
      std::string::size_type pos=start;

      if( what.empty() )
        return str;

      for(;;)
        {
          pos = str.find( what, pos );
          if( pos == std::string::npos )
            break;

          if( with.empty() )
            {
              std::string s = str.substr( 0, pos );
              s += str.substr( pos + what.size() );
              str = s;
              continue;
            }
          else
            {
              str.replace( pos, what.size(), with );
            }

          pos += with.size();
        }
      return str;
    }

  } // /namespace Format2
} // /namespace Tools

#endif
