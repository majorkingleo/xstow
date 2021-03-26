#include "oracle_db.h"

#ifdef TOOLS_USE_ORACLE

#include <ocicpp.h>

#include "string_utils.h"

using namespace Tools;

#define C( db ) static_cast<OCICPP::Connection*>( db )

bool OracleDB::done_init = false;

OracleDB::OracleDB()
  : DB(),
	db(0)
{
  if( !done_init )
	{
	  OCICPP::db::init( OCI_DEFAULT );
	  done_init = true;
	}

  db = new OCICPP::Connection;
}

OracleDB::~OracleDB()
{
  close();
}

bool OracleDB::connect( const char *hostname, const char *username, const char *passwd )
{
  if( db )
	{
	  close();
	  db = new OCICPP::Connection;
	}

  err.clear();

  try
	{

	  OCICPP::db::connect( hostname, username, passwd, *C(db) );

	} catch( OCICPP::OraError &er ) {
	  err = er.message;
	  return false;
	}

  return true;
}

bool OracleDB::select_db( const char *db_name )
{
  return true;
}

const char* OracleDB::error()
{
  return err.c_str();
}

void OracleDB::close()
{
  delete C(db);
  db = 0;
}

DBErg<DBRowList> OracleDB::query( const std::string &sql )
{
  return select( sql, false );
}

int OracleDB::insert_id()
{
  return 0;
}

DBErg<DBRowList> OracleDB::select( const std::string &sql, bool table_names )
{
  OCICPP::Cursor cur;
  DBErg<DBRowList> erg;

  err.clear();

  std::string s_sql = sql;
  std::string limit;
  std::string::size_type pos;
  unsigned limit_current = 0;
  unsigned limit_max = 0;

  if( ( pos = s_sql.rfind( "limit" ) ) != std::string::npos )
	{
	  limit = s_sql.substr( pos );
	  s_sql = s_sql.substr( 0, pos );

	  // std::cout << "s_sql: " << s_sql << " limit: " << limit << endl;
	  
	  limit = limit.substr( 5 );

	  if( limit.find( ',' ) != std::string::npos )
		{
		  std::vector<std::string> sl = split_simple( limit, "," );

		  if( sl.size() == 2 )
			{
			  limit_current = s2x<unsigned>( sl[0], 0 );
			  limit_max = s2x<unsigned>( sl[1], 0 );
			}
		  else
			{
			  err = "parse error at limit format: " + limit;
			  return erg;
			}
		}
	  else
		{
		  limit_current = s2x<unsigned>( limit, 0 );
		}

	  // std::cout << "current: " << limit_current << " max: " << limit_max << endl;
	}
  
  try
	{
	  C(db)->execQuery( s_sql.c_str(), cur );

	  std::vector<std::string> names;

	  for(int i=0;i<cur.getNCols();i++) 
		{
		  std::string attrName;
		  cur.getColName(i,attrName);
		  /*
		  if( table_names )
			{
			  err = "table_names are not supported by OCICPP";
			  return erg;
			}		  
		  */
		  // std::cout << "attrName: " << attrName << endl;
		  names.push_back( attrName );
		}

	  DBErg<DBRowList> sl( names );

	  int count = 0;
	  unsigned lc = 0;

	  while( cur.fetch() )
		{
		  if( !limit.empty() )
			{
			  if( lc < limit_current )
				{
				  lc++;
				  continue;
				}
			}

		  count++;

		  if( !limit.empty() )
			if( count > static_cast<int>(limit_max) )
			  return sl;

		  sl.row_list.values.push_back( std::vector<std::string>() );

		  int num = cur.getNCols();

		  for( int i = 0; i < num; ++i )
			{
			  std::string s;
			  
			  cur.getStr( i, s );
			  
			  sl.row_list.values[count-1].push_back( s );
			}
		}
	  
	  return sl;
	  
	} catch( OCICPP::OraError &er ) {
	  err = er.message;
	  return DBErg<DBRowList>();
	}

  return DBErg<DBRowList>();
}


#endif
