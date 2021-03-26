#ifndef TOOLS_MYSQL_DB_H
#define TOOLS_MYSQL_DB_H

#include "../tools_config.h"

#ifdef TOOLS_USE_MYSQL

#include "db.h"

namespace Tools {
  
class MySqlDB : public DB
{
 private:
    void *db;

 public:
    MySqlDB();
    ~MySqlDB();

    bool connect(  const char *hostname, const char* username, const char *passwd );
    bool select_db( const char *db_name );
    const char* error();
    void close();

    DBErg<DBRowList> query( const std::string &sql );
    DBErg<DBRowList> select( const std::string &sql, bool table_names );

    int insert_id();
};

} // namespace Tools

#endif

#endif
