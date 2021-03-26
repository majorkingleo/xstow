#include "mysql_db.h"

#ifdef TOOLS_USE_MYSQL

#include <mysql/mysql.h>

#define C( db ) static_cast<MYSQL*>(db)

using namespace Tools;

MySqlDB::MySqlDB()
    : DB(),
      db(0)
{
    db = mysql_init(0);
}

MySqlDB::~MySqlDB()
{
    close();
}

bool MySqlDB::connect(  const char *hostname, const char* username, const char *passwd )
{
    if( db )
    {
	close();
	db = mysql_init(0);
    }

    return mysql_real_connect( C(db), hostname, username, passwd, 0, 0, 0, 0 );
}

bool MySqlDB::select_db( const char *db_name )
{
    return !mysql_select_db( C(db), db_name );
}

const char* MySqlDB::error()
{
  return mysql_error( C(db) );
}

void MySqlDB::close()
{
    mysql_close( C(db) );
    db = 0;
}

DBErg<DBRowList> MySqlDB::query( const std::string &sql )
{
  return select( sql, false );
}

int MySqlDB::insert_id()
{
    return mysql_insert_id( C(db) );
}


DBErg<DBRowList> MySqlDB::select( const std::string &sql, bool table_names )
{
  MYSQL_RES *res;
  MYSQL_ROW row;


  if( mysql_real_query( C(db), sql.c_str(), sql.size() ) )
    {     
      DBErg<DBRowList> erg;

      if( error() == NULL )
		  erg.success = true;

      return erg;
    } else if( sql.find( "delete " ) == 0 ) {
		DBErg<DBRowList> erg;
		
		if( error() == NULL || std::string( error() ).empty() || std::string( error() ) == "" )
			erg.success = true;
		
		return erg;
	}

  if( !(res = mysql_store_result( C(db) ) ) )
    {
      mysql_free_result( res );
      return DBErg<DBRowList>();
    }
  
  std::vector<std::string> names;
  
  while( MYSQL_FIELD* field = mysql_fetch_field( res ) )
    {
      if( table_names )
		names.push_back( std::string(field->table) + '.' + field->name );
      else
		names.push_back( field->name );
    }
  
  DBErg<DBRowList> sl( names );
  
  int count = 0;
  
  while( (row = mysql_fetch_row( res )) )
    {
      count++;
      sl.row_list.values.push_back( std::vector<std::string>() );
      
      int num = mysql_num_fields( res );
      
      for( int i = 0; i < num; ++i )
	{
	  std::string s;
	  
	  if( row[i] )
	    s = row[i];
	  
	  sl.row_list.values[count-1].push_back( s );
	}
    }
  mysql_free_result( res );
  
  return sl;
}

#endif
