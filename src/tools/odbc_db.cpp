#include "odbc_db.h"

#ifdef TOOLS_USE_ODBC

#include "format.h"
#include "string_utils.h"

#ifdef WIN32
#  include <windows.h>
#endif

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <mbstring.h>
#include "debug.h"

#define H( x ) (SQLHENV*)x
#define C( x ) (SQLHDBC*)x

using namespace Tools;

ODBCDB::ODBCDB()
  : DB(),
	handle(0),
	connection(0)
{
  handle     = new SQLHENV;
  connection = new SQLHDBC;
  
  SQLAllocHandle( SQL_HANDLE_ENV, SQL_NULL_HANDLE, H(handle) );
  SQLSetEnvAttr( *H(handle), SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC2, 0 );
  SQLAllocHandle( SQL_HANDLE_DBC, *H(handle), C(connection) );
}

ODBCDB::~ODBCDB()
{
  close();

  SQLFreeHandle( SQL_HANDLE_DBC, *C(connection) );
  SQLFreeHandle( SQL_HANDLE_ENV, *C(handle ) );
  
  delete C(connection);
  delete H(handle);
}

const char* ODBCDB::error()
{
  return err.c_str();
}

bool ODBCDB::connect( const char *hostname, const char* username, const char* password )
{
  if( SQL_SUCCESS != SQLConnectA( *C(connection), 
								 (SQLCHAR*)hostname, SQL_NTS,
								 (SQLCHAR*)username, SQL_NTS,
								 (SQLCHAR*)password, SQL_NTS ) )
    {
	  err = "connection failed";
	  return false;
    }
  
  return true;	
}

bool ODBCDB::select_db( const char *db )
{
  return true;
}

DBErg<DBRowList> ODBCDB::select( const std::string & sql, bool table_names )
{
  SQLHSTMT statement;
  SQLRETURN ret;
  SQLINTEGER e;
  
  ret = SQLAllocHandle( SQL_HANDLE_STMT, *C(connection), &statement );     

  ret = SQLExecDirectA( statement, (SQLCHAR*) sql.c_str(), SQL_NTS );
  
  if( ret == SQL_ERROR )
    {
	  printf( "here\n" );
	  printf( sql.c_str() );
	  printf( "\n" );
	  err = format( "sql error\nquery: %s", sql );
	  SQLFreeHandle( SQL_HANDLE_STMT, statement );
	  return DBErg<DBRowList>();
    } 
  else if( ret == SQL_INVALID_HANDLE ) 
	{
	  err = "invalid handle";
	  SQLFreeHandle( SQL_HANDLE_STMT, statement );
	  return DBErg<DBRowList>();
	}
  
  SQLSMALLINT cols;
  
  SQLNumResultCols( statement, &cols );
  
  if( !cols )
	{
	  DBErg<DBRowList> erg;
	  erg.success=1;
	  SQLFreeHandle( SQL_HANDLE_STMT, statement );
	  return erg;
	}
  
  ret = SQLFreeHandle( SQL_HANDLE_STMT, statement );
	
	
  SQLAllocHandle( SQL_HANDLE_STMT, *C(connection), &statement );
	
  char buffer[cols][200];
	
  for( int i = 0; i < cols; ++i )
	SQLBindCol( statement, i+1, SQL_C_CHAR, &buffer[i], 150, &e );

  SQLExecDirectA( statement, (SQLCHAR*) sql.c_str(), SQL_NTS );
  
  std::vector<std::string> names;  

  for( int i = 0; i < cols; i++ )
	{
	  char acBuffer[100] = {'\0'};
	  SQLSMALLINT length;
	  SQLSMALLINT type;
	  SQLUINTEGER size;
	  SQLSMALLINT digits;
	  SQLSMALLINT isNull;
	  int num;
	  std::string name;

	  SQLDescribeColA( statement,
					  i+1, 
					  (SQLCHAR*)&acBuffer[0], 
					  sizeof(acBuffer),
					  &length,
					  &type,
					  &size,
					  &digits,
					  &isNull );

	  acBuffer[length]='\0';	  
	  // std::cout << "fields:" << acBuffer << std::endl;
	  name = acBuffer;
	  

	  if( table_names ) {

		length = 0;
		std::string table;

		SQLColAttributeA( statement,
						 i+1,
						 SQL_DESC_BASE_TABLE_NAME,
						 (SQLCHAR*)&acBuffer[0],
						 sizeof(acBuffer),
						 &length,
						 &num);
		
		
		acBuffer[length] = '\0';	   	   
		table = acBuffer;

        DEBUG( format( "Buffer: %s length: %d", acBuffer, length ) );

		if( !table.empty() ) 
		  {
			name = table + '.' + name;
		  }

		// std::cout << acBuffer << "." << name << std::endl;
	  }

	  names.push_back( name );
	}

  DBErg<DBRowList> sl(names);

  long erg = SQLFetch( statement );
  int count = 0;
  while( erg != SQL_NO_DATA )
    {
	  count++;
	  sl.row_list.values.push_back( std::vector<std::string>() );
	
	  for( int i = 0; i < cols; ++i )
		{
		  std::string s;
		  s = buffer[i];
		  sl.row_list.values[count-1].push_back( s );
		}
	  
	  erg = SQLFetch( statement );
    }
  
  SQLFreeHandle( SQL_HANDLE_STMT, statement );
  
  sl.success=1;

  return sl;
}

void ODBCDB::close()
{
  SQLDisconnect( *C(connection) );
}

int ODBCDB::insert_id()
{
  DBErg<DBRowList> sl = select( "SELECT LAST_INSERT_ID()", false );
  
  if( !sl.row_list.values.empty() )
	{
	  int d = 0;
	  d = s2x<int>( sl.row_list.values[0][0], 0 );
	  return d;
	}
  
  return 0;
}

DBErg<DBRowList> ODBCDB::query( const std::string &sql )
{
  return select( sql, false );
}


#endif
