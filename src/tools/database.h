#ifndef TOOLS_DATABASE_H
#define TOOLS_DATABASE_H

#include "db.h"

#ifdef TOOLS_USE_DB

namespace Tools {

class Database
{
 private:
  DB *db;
  std::string sql;
  std::string err;

  Database() {}
  Database( const Database &db_ ) {}

 public:
  
#ifdef TOOLS_USE_MYSQL
  static const unsigned DB_MYSQL;
#endif

#ifdef TOOLS_USE_ORACLE
  static const unsigned DB_ORACLE;
#endif

#ifdef TOOLS_USE_ODBC
  static const unsigned DB_ODBC;
#endif

 public:
  Database( const std::string &host, 
	    const std::string &user, 
	    const std::string &passwd, 
	    const std::string &instance, unsigned type );
  ~Database();

  bool operator!() const { return !db; }
  bool valid() const { return db != NULL; }

  std::string get_error() const { return db ? db->error() : err; }

  DBErg<DBRowList> exec( const std::string &query )
    {
      sql = query;
      return db->query( query );
    }

  DBErg<DBRowList> select( const std::string &query, bool table_names = true )
    {
      sql = query;
      return db->select( query, table_names );
    }

  void rollback()
  {
    exec( "rollback;" );
  }

  void commit()
  {
    exec( "commit;" );
  }

  std::string get_sql() const { return sql; }
  int  get_insert_id() { return db->insert_id(); }

  DBErg<DBRowList> select( const std::string &table, const DBRow &which, const std::string &extra ); 
  DBErg<DBRowList> insert( const std::string &table, const DBRow &row );
  DBErg<DBRowList> insert( const std::string &table, const DBRowList &rows );

  DBErg<DBRowList> update( const std::string &table, const DBRow &row, const std::string extra = std::string() );

 private:
  std::string create_values_list( const std::string &table, const std::vector<std::string> &names );
};

} // namespace Tools

#endif

#endif
