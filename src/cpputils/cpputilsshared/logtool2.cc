#include <logtool2.h>

int _CPPLogPrintf( const std::string & fac, int level, std::string fs )
{
  return _LogPrintf( fac.c_str(), level, "%s", fs.c_str() );
}

#ifdef NOWAMAS

#include <iostream>
#include <fstream>
#include <time.h>
#include <stdlib.h>

namespace  {
  const char *local_file = "NOWHERE";
  int local_line = 0;
}

int _LogSetLocation( const char *file, int line )
{
  local_file = file;
  local_line = line;
  return 0;
}

int _LogPrintf( const char *fac, int level, const char *str, const char *msg )
{
  const char *logroot = getenv( "LOGROOT" );

  if( logroot == NULL )
	logroot = ".";

  std::ofstream out( Tools::format( "%s/log.%s", logroot, fac ).c_str(), std::ios_base::app );

  time_t now = time(NULL);

  struct tm *tm = localtime(&now);

  char buf[256];

  buf[0] = '\0';

  strftime( buf, 255, "%Y-%m-%d %H:%M:%S", tm );

  out << buf << ' ' << local_file << ':' << local_line << " " << msg << std::endl;
  return 0;
}

#endif
