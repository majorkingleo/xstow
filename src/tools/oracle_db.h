#ifndef TOOLS_ORACLE_DB_H
#define TOOLS_ORACLE_DB_H

#include "../tools_config.h"

#ifdef TOOLS_USE_ORACLE

#include "db.h"

namespace Tools {

class OracleDB: public DB
  {
  private:
	void *db;
	static bool done_init;
	std::string err;

  public:
	OracleDB();
	~OracleDB();

	bool connect( const char *hostname, const char *username, const char *passwd );
	bool select_db( const char *db_name );
	const char *error();
	void close();
	
	DBErg<DBRowList> query( const std::string &sql );
	DBErg<DBRowList> select( const std::string &sql, bool table_names );

	int insert_id();
  };

}

#endif

#endif
