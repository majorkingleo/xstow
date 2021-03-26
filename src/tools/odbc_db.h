#ifndef TOOLS_ODBC_DB_H
#define TOOLS_ODBC_DB_H

#include "../tools_config.h"

#ifdef TOOLS_USE_ODBC

#include "db.h"

namespace Tools {

class ODBCDB : public DB
{
    void *handle;
    void *connection;

    std::string err;

 public:

    ODBCDB();
    ~ODBCDB();

    const char * error();
    bool connect( const char *hostname, const char* username, const char* password );
    bool select_db( const char *db );
    DBErg<DBRowList> select( const std::string & query, bool table_names );
	DBErg<DBRowList> query( const std::string &sql );
    void close();
    int insert_id();    
};

} // namespace Tools

#endif

#endif
