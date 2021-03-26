#include "database.h"
#include "mysql_db.h"
#include "oracle_db.h"
#include "odbc_db.h"
#include "format.h"
#include <iostream>

#ifdef TOOLS_USE_DB

using namespace Tools;

#ifdef TOOLS_USE_MYSQL
const unsigned Tools::Database::DB_MYSQL = 1;
#endif

#ifdef TOOLS_USE_ORACLE
const unsigned Tools::Database::DB_ORACLE = 2;
#endif

#ifdef TOOLS_USE_ODBC
const unsigned Tools::Database::DB_ODBC = 3;
#endif

Database::Database( const std::string &host, 
		    const std::string &user, 
		    const std::string &passwd, 
		    const std::string &instance, unsigned type )
  : err( "No Database!" )
{
#ifdef TOOLS_USE_MYSQL
  if( type == DB_MYSQL )
	db = new MySqlDB();
#endif

#ifdef TOOLS_USE_ORACLE
  if( type == DB_ORACLE )
	db = new OracleDB();
#endif

#ifdef TOOLS_USE_ODBC
  if( type == DB_ODBC )
	db = new ODBCDB();
#endif

  if( !db )
	return;

  if( !db->connect( host.c_str(), user.c_str(), passwd.c_str() ) )
	{
	  err = db->error();
	  delete db;
	  db = 0;
	  return;
	}
  
  if( !db->select_db( instance.c_str() ) )
	{
	  err = db->error();
	  delete db;
	  db = 0;
	  return;
	}
  
  exec( "set AUTOCOMMIT=0;" );
}

Database::~Database()
{
  delete db;
}

std::string Database::create_values_list( const std::string &table, const std::vector<std::string> &cnames )
{
  std::string csql = "INSERT INTO " + table + '(';

  for( unsigned i = 0; i < cnames.size(); i++ )
    {
      if( i )
		csql += ",\n";

      csql += cnames[i];
    }

  csql += ") VALUES ( ";

  return csql;
}

DBErg<DBRowList> Database::insert( const std::string &table, const DBRow &row )
{
  sql = create_values_list( table, row.names );

  for( unsigned i = 0; i < row.values.size(); i++ )
    {
      if( i )
	sql += ",\n";

      sql += '\'' + row.values[i] + '\'';
    }

  sql += ')';

  return exec( sql );
}

DBErg<DBRowList> Database::insert( const std::string &table, const DBRowList &rows )
{
  std::string pre_sql = create_values_list( table, rows.names );
  DBErg<DBRowList> erg;

  for( unsigned i = 0; i < rows.values.size(); i++ )
    {     
      sql = pre_sql;

      for( unsigned j = 0; j < rows.values[i].size(); j++ )
	{
	  if( j )
	    sql += ",\n";

	  sql += '\'' + rows.values[i][j] + '\'';
	}

      sql += ')';      

      erg = exec( sql );

      if( !erg )
	return erg;
    }

  return erg;
}

DBErg<DBRowList> Database::update( const std::string &table,
				  const DBRow &row, 
				  const std::string extra )
{
  sql = "UPDATE " + table + " SET ";

  for( unsigned i = 0; i < row.names.size(); i++ )
    {
      if( i )
	sql += ",\n";

      sql += row.names[i] + "='" + row.values[i] + '\'';
    }

  sql += extra;

  return exec( sql );
}

DBErg<DBRowList> Database::select( const std::string &table, const DBRow &which, const std::string &extra )
{
  std::string what;

  for( unsigned i = 0; i < which.names.size(); i++ )
    {
      if( i )
	what += ',';

      what += which.names[i];
    }

  sql = format( "SELECT %s FROM %s %s", what, table, extra );

  return exec( sql );
}

#endif
