#ifndef debug_h
#define debug_h

#include "printf_format.h"
#include "range.h"
#include "backtrace.h"

using namespace Tools;

struct EnumModule {
  enum ETYPE {
	FIRST__ = - 2,
	ALL  = -1,
	ARG  = 1,
	MAIN,
	TREE,
	CPPDIR,
	SETUP,
	NIGNORE,
	MERGE_INFO,
	LAST__
  };
};

typedef EnumRange<EnumModule> MODULE;
typedef MODULE DModule;

extern Format::PrintF<std::ostream> out;
extern Format::PrintF<std::ostream> vout;
extern Format::PrintF<std::ostream> eout;

/*
DModule string2module( std::string module );
*/
MODULE::ETYPE string2module( std::string module );

std::string x2s( MODULE module );

extern std::string progname;

#define ERROR eout( MODULE::ALL )[0]( "%s: ", progname )[0]

#ifdef NDEBUG
#  define DEBUG( expr )
#  define DEBUG_OUT( level, module ) out(module)[level]
#else
#  define DEBUG( expr ) expr
#  define DEBUG_OUT( level, module ) out(module)[level]( "%s:%s: ", __FILE__, __LINE__ )[level]
#endif

#endif
