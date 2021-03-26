#ifndef CPP_UTIL_H
#define CPP_UTIL_H

#include <cstring>

#include "logtool2.h"
#include "ref.h"
#include "format.h"
#include <vector>
#include <map>
#include <algorithm>
#include "jvector.h"
#include "report_exception.h"
#include "sql_exception.h"
#include "cpp_util_min_max.h"

#ifndef NOWAMAS
# include <dbsql.h>
# include <hist_util.h>
#endif

#ifndef NOWAMAS
# if TOOLS_VERSION > 38
#  define CAST_TO_CALLBACK_VALUE_TYPE( x ) ((uintptr_t)x)
# else
#  define CAST_TO_CALLBACK_VALUE_TYPE( x ) ((void*)x)
# endif 
#endif

# if TOOLS_VERSION > 38
#  define CAST_TOOLS_RESOURCE_TYPE( x ) ((uintptr_t)x)
# else
#  define CAST_TOOLS_RESOURCE_TYPE( x ) ((void*)x)
# endif



#include "zvector.h"

inline char * TO_CHAR( const char * s )
{
  return const_cast<char*>( s );
}

// C Kompalibitätsfunktion, damit man sich das const_cast<char*>() spart
inline char * TO_CHAR( const std::string & s )
{
  return const_cast<char*>( s.c_str() );
}

#ifndef NOWAMAS

#ifdef FIX_BUG_20809
#  define WORKAROUND_BUG_20809_TSqlNewContext 		_TSqlNewContext
#  define WORKAROUND_BUG_20809_TSqlDestroyContext 	_TSqlDestroyContext
#else
#  define WORKAROUND_BUG_20809_TSqlNewContext 		TSqlNewContext
#  define WORKAROUND_BUG_20809_TSqlDestroyContext 	TSqlDestroyContext
#endif

class CppContext
{
  SqlContext *sc;
  void *tid;

private:
	CppContext( const CppContext & ) : sc(0), tid(0) {}
	CppContext & operator=( const CppContext & ) { return *this; }

public:
  CppContext( void *tid = NULL )
	: sc ( 0 ), tid( tid )
  {
	sc = WORKAROUND_BUG_20809_TSqlNewContext ( tid, NULL );
  }

  ~CppContext()
  {
	if( sc )
	  WORKAROUND_BUG_20809_TSqlDestroyContext( tid, sc );
  }

  bool operator!() const { return sc == NULL; }

  SqlContext* operator*() { return sc; }
  operator SqlContext*() { return sc; }
};

#endif



#ifndef NOWAMAS

class AutoRollback
{
  void *tid;

private:
	AutoRollback( const AutoRollback & ) : tid(0) {}
	AutoRollback & operator=(const AutoRollback & ) { return *this; }

public:
  AutoRollback( void *tid_ = NULL )
	: tid( tid_ )
  {

  } 
  
  ~AutoRollback()
  {
	// fprintf( stderr, "AutoRollBacked\n" );
	rollback();
  }

  void* operator*() { return tid; }

  int commit() { return TSqlCommit(tid); }
  int rollback() { return TSqlRollback(tid); }

  operator void*() { return tid; }
  operator void*() const { return tid; }
};

#endif

namespace Tools {

/** template that returns the real type of a pointer */
template<typename T> struct zz_remove_pointer 
{
   typedef T result;
};

template<typename T> struct zz_remove_pointer<T*>
{
   typedef T result;
};

/** template that returns a pointer of a type */
template<typename T> struct zz_get_pointer 
{ 
  typedef T *result;
};

template<typename T> struct zz_get_pointer<T*>
{ 
  typedef T *result;
};

template<typename T> struct zz_pointer_traits
{
  typedef T* pointer;
  typedef T  value_type;
}; 

template<typename T> struct zz_pointer_traits<T*>
{
  typedef T* pointer;
  typedef T  value_type;
}; 

template <class T, typename _pointer = typename zz_remove_pointer<T>::result> class ZeroContainer
{
 public:
  typedef typename zz_pointer_traits<_pointer>::value_type   type;  
  typedef typename zz_pointer_traits<_pointer>::pointer      pointer_type;  
  typedef typename zz_pointer_traits<_pointer>::value_type & reference_type;  
  
  void do_memset( type & t )
  {
	memset( &t, 0, sizeof( type ) );
  }
};

} // namespace Tools

template <class T> void zeroit( T & t, Tools::ZeroContainer<T> cont = Tools::ZeroContainer<T>() )
{
  cont.do_memset( t );
}


template <class A> void do_not_use_a_pointer( const A & a, const A *b )
{
  /* The attribute avoids a warning from g++,
	 but the implementation of aa results in an error
	 if A is a pointer. And that's the behavoir we want.
  */

 const unsigned long min_size = 2;
 const unsigned long size = sizeof(A) < min_size ? sizeof(A) : min_size;

#if defined WIN32 || defined _WIN32
#pragma warning( suppress: 4189 )
  A  aa[size] = { {'A'} };
#else
  A __attribute__((unused)) aa[size] = { {'A'} };
#endif /* WIN32 || _WIN32 */
}

/* Simple function that makes a strcpy but compilation will fail
   if the user is using a char* of unknown size.
*/
template <class A, class B> void StrCpy( A & a, const B & b )
{  
  size_t len = Tools::CppUtilMin( sizeof(A), sizeof(B) );

  do_not_use_a_pointer(a,&a);
  do_not_use_a_pointer(b,&b);
  
  strncpy( a, b, len );

  a[len-1]='\0'; 
}

template <class A> void StrCpy( A & a, const std::string & b )
{
  size_t len = Tools::CppUtilMin( sizeof(A), b.size() + 1 );

  do_not_use_a_pointer(a,&a);
  
  strncpy( a, b.c_str(), len );

  a[len-1]='\0'; 
}

/* Copys onle the avaliable bytes that are matching into the destination */
template <class A> void StrCpyDestLen( A & a, const std::string & b )
{
  size_t len = Tools::CppUtilMin( sizeof(A), b.size() + 1 );

  do_not_use_a_pointer(a,&a);
  
  strncpy( a, b.c_str(), len );

  a[len-1]='\0'; 
}

template <class A> void StrCpyDestLenAllowNull( A & a, const char *str )
{
	if( str == NULL )
		str = "";
	
	StrCpyDestLen( a, str );
}

template <class A,class B> void StrAppend( A & dest, const B & c_str )
{
  std::string str_a = dest;
  str_a += c_str;

  StrCpy( dest, str_a );
}


#ifndef NOWAMAS
// FetchTable ist hier, um moeglichst ohne Workaround dieses Bug der da Beschrieben ist
// eben nicht zu triggern!

#ifdef FIX_BUG_20809
#  define WORKAROUND_BUG_20809_TExecSqlX _TExecSqlX
#  define WORKAROUND_BUG_20809_TExecSqlV _TExecSqlV
#else
#  define WORKAROUND_BUG_20809_TExecSqlX TExecSqlX
#  define WORKAROUND_BUG_20809_TExecSqlV TExecSqlV
#endif

namespace Tools {

class FetchTableBase
{
 protected:
  void *tid;
  SqlContext *sc;
  std::string where;
  const int BLOCKSIZE;
  bool failed;
  std::string sql;

 protected:
  FetchTableBase()
	: 
	tid(0),
	sc(0),
	where(""),
	BLOCKSIZE(100),
	failed(false),
	sql("")
	{

	}

  FetchTableBase( void *tid_, SqlContext *sc_, const std::string where_ , const unsigned int BLOCKSIZE_ )
	:
	tid(tid_), 
	sc(sc_), 
	where( where_ ),
	BLOCKSIZE( BLOCKSIZE_ ),
    failed(false),
	sql( "" )
	{}

  FetchTableBase( const FetchTableBase & table )
	:
	tid(table.tid),
	sc(table.sc),
	where(table.where),
	BLOCKSIZE(table.BLOCKSIZE),
	failed(table.failed),
	sql(table.sql)
	{}

  virtual ~FetchTableBase() {}


  void check_bug_20809()
	{
#if defined WIN32 || defined _WIN32
	  if( std::string( __FILE__ ).find(":") != std::string::npos &&
		  std::string( TSqlErrTxt(tid) ).find( "too few bind variables" ) != std::string::npos )  
		{
		  throw REPORT_EXCEPTION( Tools::format("WARNING YOU TRIGGERT BUG http://bugzilla.salomon.at/show_bug.cgi?id=20809\n"
												"Possible Workaround: recompiling your file with defined FIX_BUG_20809 before\n"
												"including this '" __FILE__ "' Header file.\n" 
												"SqlError: %d SqlError is: %s", TSqlError, TSqlErrTxt(tid) )
							  );
		}
#endif	  
	}

  FetchTableBase & operator=( const FetchTableBase & table )
	{
	  tid = table.tid;
	  sc = table.sc;
	  where = table.where;
	  failed = table.failed;
	  sql = table.sql;

	  return *this;
	}

public:
    bool operator!() const { return failed; }

	const std::string & get_sql() const { return sql; }

};

template <class Table> class FetchTable : public JVector<Table>, public FetchTableBase
{
 protected:
  std::string table_name;

 private:
  FetchTable() : FetchTableBase(), table_name("") {};

 public:
  FetchTable(const FetchTable<Table> & table )
	: JVector<Table>(table.BLOCKSIZE),
	FetchTableBase( table ),
	table_name(table.table_name)
	{
	  JVector<Table>::insert( JVector<Table>::end(), table.begin(), table.end() );
	}

  FetchTable & operator=( const FetchTable<Table> & table )
	{
	  FetchTableBase::operator=( table );

	  JVector<Table>::clear();
	  JVector<Table>::insert( JVector<Table>::end(), table.begin(), table.end() );	
	  table_name = table.table_name;	  

	  return *this;
	}

 public:
  FetchTable( void *tid_, SqlContext *sc_, const std::string & table_name_, const std::string where_ = "", const unsigned int BLOCKSIZE_ = 100 )
    : JVector<Table>(BLOCKSIZE_),
	FetchTableBase( tid_, sc_, where_, BLOCKSIZE_ ),
	table_name( table_name_ )
	{
		sql = Tools::format( "select %%%s from %s %s", table_name, table_name, where );
		exec();
      }

 protected:  

  void exec()
    {
      // fprintf( stderr, "exec()===========\n" );
      JVector<Table> vec(BLOCKSIZE);
     
	  JVector<Table>::resize(0);
	  JVector<Table>::reserve(BLOCKSIZE);
 
      int rv = 0;	  
	  
      do
		{
		  rv = ( rv == 0 ) ?  WORKAROUND_BUG_20809_TExecSqlX( tid, sc, 
										 const_cast<char*>(sql.c_str()),
										 BLOCKSIZE, 0,
										 SELSTRUCT( const_cast<char*>(table_name.c_str()), vec[0] ),
										 NULL )
			: WORKAROUND_BUG_20809_TExecSqlV( tid, sc, NULL, NULL, NULL, NULL );
		  
		  
		  if( rv < BLOCKSIZE  && TSqlError(tid) != SqlNotFound )	    
			{
			  failed = true;

			  check_bug_20809();
			  return;
			}
		  
			if( rv > 0 )
			   JVector<Table>::insert( JVector<Table>::end(), vec.begin(), vec.begin() + rv );

		} while( rv == BLOCKSIZE );      
    }

 public:
    Table & operator[]( unsigned int i ) { return JVector<Table>::operator[](i); }
};


template <class TableA, class TableB> class FetchTablePair : public JVector<std::pair<TableA,TableB> >, public FetchTableBase
{
 protected:
  std::string table1_name;
  std::string table2_name;

 private:
  FetchTablePair() : FetchTableBase(), table1_name(""), table2_name("") {};

 public:
  FetchTablePair(const FetchTablePair<TableA,TableB> & table )
	: JVector<std::pair<TableA,TableB> >(),
	FetchTableBase( table ),
	table1_name(table.table1_name),
	table2_name(table.table2_name)
	{
	  	  JVector<std::pair<TableA,TableB> >::insert( JVector<std::pair<TableA,TableB> >::end(),table.begin(), table.end() );
	}

  FetchTablePair & operator=( const FetchTablePair<TableA, TableB> & table )
	{
	  FetchTableBase::operator=( table );
	  
	  JVector<std::pair<TableA,TableB> >::insert( JVector<std::pair<TableA,TableB> >::end(), table.begin(), table.end() );
	  table1_name = table.table1_name;
	  table2_name = table.table2_name;

	  return *this;
	}

 public:
  FetchTablePair( void *tid_, 
			  SqlContext *sc_, 
			  const std::string & table1_name_, 
			  const std::string & table2_name_,
			  const std::string & where_, 
			  const unsigned int BLOCKSIZE_ = 100 )
    : JVector<std::pair<TableA,TableB> >(),
	FetchTableBase(tid_, sc_, where_, BLOCKSIZE_ ),
	table1_name( table1_name_ ), 
	table2_name( table2_name_ )
      {
		sql = Tools::format( "select %%%s, %%%s from %s, %s %s", table1_name, table2_name, table1_name, table2_name, where );
		exec();
      }

 protected:  

  void exec()
    {
      // fprintf( stderr, "exec()===========\n" );
      JVector<TableA> vec1(BLOCKSIZE);
      JVector<TableB> vec2(BLOCKSIZE);
     
	  JVector<std::pair<TableA,TableB> >::resize(0);
	  JVector<std::pair<TableA,TableB> >::reserve(BLOCKSIZE);
 
      int rv = 0;
	  
      do
		{
		  rv = ( rv == 0 ) ? WORKAROUND_BUG_20809_TExecSqlX( tid, sc, 
										 const_cast<char*>(sql.c_str()),
										 BLOCKSIZE, 0,
										 SELSTRUCT( const_cast<char*>(table1_name.c_str()), vec1[0] ),
										 SELSTRUCT( const_cast<char*>(table2_name.c_str()), vec2[0] ),
										 NULL )
			:  WORKAROUND_BUG_20809_TExecSqlV( tid, sc, NULL, NULL, NULL, NULL );
		  
		  
		  if( rv < BLOCKSIZE  && TSqlError(tid) != SqlNotFound )	    
			{
			  failed = true;

			  check_bug_20809();
			  return;
			}
		  
			if( rv > 0 )
			  {
				for( int i = 0; i < rv; i++ )
				  {
					JVector<std::pair<TableA,TableB> >::push_back( std::pair<TableA,TableB>( vec1[i], vec2[i] ) );
				  }
			  }

		} while( rv == BLOCKSIZE );      
    }

 public:
    std::pair<TableA,TableB> & operator[]( unsigned int i ) { return JVector<std::pair<TableA,TableB> >::operator[](i); }
    
};

} // namespace Tools

#endif // NOWAMAS

#ifndef NOWAMAS

template<class T> void SetHist( const std::string & table, std::vector<T> & all, 
							    HistSetTyp mode, 
							    const std::string & user = GetUserOrTaskName() )
{
  for( unsigned i = 0; i < all.size(); i++ )
	{
	  SetHist( TO_CHAR(table), &all[i], mode, TO_CHAR(user) );
	}
}

template<class T> void SetHist( const std::string & table, Tools::FetchTable<T> & all, 
							    HistSetTyp mode, 
							    const std::string & user = GetUserOrTaskName() )
{
  for( unsigned i = 0; i < all.size(); i++ )
	{
	  SetHist( TO_CHAR(table), &all[i], mode, TO_CHAR(user) );
	}
}

inline void SetHist( const std::string & table, void *pvTable, HistSetTyp mode, const std::string & user = GetUserOrTaskName() )
{
  SetHist( TO_CHAR(table), pvTable, mode, TO_CHAR(user) ); 
}

#endif


#endif
