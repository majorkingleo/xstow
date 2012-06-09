/*
 * $Log: file_option.h,v $
 * Revision 1.3  2010/07/21 19:38:25  martin
 * gcc-4 Port
 *
 * Revision 1.2  2005/07/04 21:59:42  martin
 * added logging to all files
 *
 */
#ifndef file_option_h
#define file_option_h

#include "arg.h"

/* 
   This Options are located in an extra module
   cause they are depending on some CppDir functions.
*/

namespace Arg {

    struct EnumFile
    {
	  enum ETYPE {
	    FIRST__,
	    ANY,
	    DIR,
	    LINK,
	    REGULAR,
	    LINK_TO_DIR,
	    LINK_TO_REGULAR,
	    LAST__
	  };
    }; 

    typedef EnumRange<EnumFile> FILE;


  class FileOptionRule : public AnyStringRule
    {
    protected:
      FILE type;

    public:
      FileOptionRule( FILE type = FILE::ANY );

      bool match( const std::string &value ) const;
    };

  class FileOption : public StringOption
    {
    public:
      FileOption( FILE type = FILE::ANY );
      FileOption( const std::string &name, FILE type = FILE::ANY );      
    };

  class EmptyFileOption : public EmptyStringOption
    {
    public:
      EmptyFileOption( FILE type = FILE::ANY );

    };
}

#endif
