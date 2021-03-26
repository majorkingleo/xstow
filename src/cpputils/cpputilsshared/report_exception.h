/**
 * @file
 * @todo describe file content
 * @author Copyright (c) 2009 Salomon Automation GmbH
 */

#ifndef _wamas_REPORT_EXCEPTION_H
#define _wamas_REPORT_EXCEPTION_H

#include <iostream>

#include <backtrace.h>
#include <ref.h>

#ifndef NOWAMAS
# include <dbsql.h>
#endif

class ReportException : public std::exception
{

protected:
  std::string err;
  std::string simple_err;
  bool		  bSqlException;
  int         iSqlError;

public:

  ReportException( const std::string & err_ ) :
	  err( err_ ),
	  simple_err(std::string()),
	  bSqlException(false),
	  iSqlError(0)
  {
	  write_warning_message();
  }

  ReportException( const std::string & err_, const std::string & simple_err_ ) :
	  err( err_ ),
	  simple_err(simple_err_),
	  bSqlException(false),
	  iSqlError(0)
  {
	  write_warning_message();
  }

  // Required for gcc 4.8.4
  // error: looser throw specifier for 'virtual ReportException::~ReportException()
  virtual ~ReportException() throw() {
  }

  virtual bool isSqlException () const {
	  return bSqlException;
  }
  
  virtual int getSqlError() const {
    return iSqlError;
  }

  virtual const char *what() const throw() {
	  return err.c_str();
  }
  virtual const char *simple_what() const throw() {
	  return simple_err.c_str();
  }

protected:
  ReportException( const std::string & err_,
		  const std::string & simple_err_,
		  bool bSqlException_,
		  int sql_error ) :
			  err( err_ ), simple_err(simple_err_),
			  bSqlException(bSqlException_),
			  iSqlError(sql_error){
	  write_warning_message();
  }

  void write_warning_message() {

	  if (bSqlException == true) {
		  std::cerr << "SqlException thrown! Message: " << err << std::endl;
	  } else {
		  std::cerr << "ReportException thrown! Message: " << err << std::endl;
	  }
  }

};

/**
 * instantiates a ReportException
 * err ... 		the user-text with back-trace
 * simple_err.. the user-text without back-trace
 */
#define REPORT_EXCEPTION( what ) \
	ReportException( Tools::format( "Exception from: %s:%d:%s message: %s%s", \
		__FILE__, __LINE__, __FUNCTION__, what, Tools::BackTraceHelper::bt.bt() ), \
		Tools::format("%s", what ) )


#endif  /* _wamas_REPORT_EXCEPTION_H */
