/**
 * @file
 * @todo describe file content
 * @author Copyright (c) 2009 Salomon Automation GmbH
 */

#ifndef _wamas_SQL_EXCEPTION_H
#define _wamas_SQL_EXCEPTION_H

#include <iostream>

#include <backtrace.h>
#include <ref.h>

#ifndef NOWAMAS
# include <dbsql.h>
#endif

#include "report_exception.h"

namespace Tools
{

class SqlException : public ReportException
{

public:

	SqlException( const std::string & err_, const std::string & simple_err_, int sql_error ) :
		ReportException(err_, simple_err_, true, sql_error)
    {
		write_warning_message();
	}

	virtual ~SqlException() throw() {
	}

};

} // namespace Tools

#ifndef NOWAMAS


/**
 * instantiates a SqlException
 * err ... 		the SQL error text with back-trace
 * simple_err.. SQL error text without back-trace
 */
#define SQL_EXCEPTION( tid ) \
	Tools::SqlException( Tools::format( "SqlError from: %s:%d:%s %s%s",\
		__FILE__, __LINE__, __FUNCTION__, \
		TSqlErrTxt(tid),					\
		Tools::BackTraceHelper::bt.bt() ), TSqlErrTxt(tid), TSqlError(tid) )


#endif // NOWAMAS

#endif  /* _wamas_SQL_EXCEPTION_H */
