#ifndef TOOLS_DBI_H
#define TOOLS_DBI_H

#include <string>
#include <vector>
#include "ref.h"
#include "database.h"
#include "string_utils.h"
#include "range.h"
#include "format.h"

#ifdef TOOLS_USE_DB

namespace Tools {

class DBRow;

class DBType
{
  const std::string name;

 public:
  struct EnumType
  {
    enum ETYPE {
      FIRST__,
      INT,
      VARCHAR,
      ENUM,
	  DOUBLE,
	  DATETIME,
      LAST__
    };
  };

  typedef EnumRange<EnumType> TYPE;

  const TYPE type;

 protected:
  DBType() {}
  DBType( const std::string &name_, TYPE type_ )
    : name( name_ ),
    type( type_ )
    {}  

 public:
  virtual ~DBType() {}

  const std::string & get_name() const { return name; }

  virtual void load_from_db( const std::string &data ) = 0;
  virtual std::string save_to_db() const = 0;
  virtual unsigned get_size() { return 20; }
  virtual std::string str( const std::string & fstr = "%s" ) const = 0;

  DBType & operator=( const DBType &t )
    {
      // Do nothing
      return *this;
    }
};

class DBBindType
{
  std::vector<DBType*> type_list;
  std::vector<std::string> names;
  std::string table_name;

 private:
  DBBindType( const DBBindType &t );

 public:
  DBBindType( const std::string &table_name_ ) : table_name( table_name_ ) {}
	virtual ~DBBindType(){}
  
  void add( DBType *db_type )
  {
    names.push_back( db_type->get_name() );
    type_list.push_back( db_type );
  }

  bool load_from_db( const DBRow &row );  
  bool load_from_db( const std::string &name, const std::string &data );
  const std::vector<std::string> & get_names() const { return names; }

  const std::string & get_table() const { return table_name; }

  std::vector<std::string> get_values() const;

  DBBindType & operator=( const DBBindType & t )
    {
      // Do exactly nothing. Addresses are still the same.

	  for( unsigned int i = 0; i < type_list.size(); i++ )
		{
		  type_list[i]->load_from_db( t.get_cell_by_name( type_list[i]->get_name() )->save_to_db() );
		}

      return *this;
    }

  const std::string & get_table_name() const { return table_name; }
  
  const std::vector<DBType*> & get_types() const { return type_list; }

  DBType* get_cell_by_name( const std::string &name );
  const DBType* get_cell_by_name( const std::string &name ) const;
}; 


class DBTypeInt : public DBType
{
 public:
  int data;

 public:
  DBTypeInt( DBBindType* parent, const std::string &name_ )
    : DBType( name_, TYPE::INT ),
    data(0)
    {
      parent->add( this );
    }
    DBTypeInt() : DBType( "", TYPE::INT ), data(0) {}

    void load_from_db( const std::string &data_ );
    std::string save_to_db() const;

    int operator()() const { return data; }
    std::string str( const std::string & fstr = "%d" ) const;
    DBTypeInt & operator=( int d ) { data = d; return *this; }
    DBTypeInt & operator=( const std::string &s ) { data = s2x<int>( s, data ); return *this; }
};

class DBTypeDouble : public DBType
{
 public:
  double data;

 public:
  DBTypeDouble( DBBindType* parent, const std::string &name_ )
    : DBType( name_, TYPE::DOUBLE ),
    data(0)
    {
      parent->add( this );
    }
    DBTypeDouble() : DBType( "", TYPE::DOUBLE ), data(0) {}

    void load_from_db( const std::string &data_ );
    std::string save_to_db() const;

    double operator()() const { return data; }
    std::string str( const std::string & fstr = "%lf" ) const;
    DBTypeDouble & operator=( double d ) { data = d; return *this; }
    DBTypeDouble & operator=( const std::string &s ) { data = s2x<double>( s, data ); return *this; }
};

class DBTypeEnumBase : public DBType
{
 public:
  DBTypeEnumBase( DBBindType* parent, const std::string &name_ )
    : DBType( name_, DBType::TYPE::ENUM )
    {
      parent->add( this );
    }

	virtual unsigned get_first_case() const = 0;
	virtual unsigned get_last_case() const = 0;
	virtual unsigned get_cases() const = 0;
	virtual std::string get_case( int i ) const = 0;
};

template<class ETYPE> class DBTypeEnum : public DBTypeEnumBase
{
 public:
  ETYPE data;
  typedef ETYPE TYPE;

 public:
  DBTypeEnum( DBBindType* parent_, const std::string &name_ )
    : DBTypeEnumBase( parent_, name_ )
    {

    }

  DBTypeEnum( DBBindType* parent_, const std::string &name_, TYPE init_type )
    : DBTypeEnumBase( parent_, name_ )
    {
      data = init_type;
    }

    DBTypeEnum() : DBType() {}

    void load_from_db( const std::string &data_ )
    {
      for( int i = TYPE::FIRST__ + 1; i < TYPE::LAST__; i++ )
	{
	  if( data.STYPES[i] == data_ )
	    {
	      data = static_cast<typename TYPE::ETYPE>(i);
	      break;
	    }
	}
    }

    std::string save_to_db() const
      {
	if( data != TYPE::FIRST__ || data != TYPE::LAST__ )
	  {
	    return data.STYPES[data];
	  }

	return std::string();
      }

    unsigned get_size()
    {
      unsigned size = 0;

      for( unsigned i = TYPE::FIRST__ + 1; i < TYPE::LAST__; i++ )
	{
	  unsigned s = data.STYPES[i].size();
	  if( s > size )
	    size = s;
	}

      return size;
    }

    unsigned get_first_case() const
	{
	  return TYPE::FIRST__ + 1;
	}

    unsigned get_last_case() const
	{
	  return TYPE::LAST__-1;
	}

    unsigned get_cases() const
    {
      return TYPE::LAST__ - TYPE::FIRST__;
    }

    std::string get_case( int i ) const
      {
	if( i + 1 + TYPE::FIRST__ >= TYPE::LAST__ )
	  return std::string();

	return data.STYPES[TYPE::FIRST__ + i + 1];
      }

    ETYPE operator()() const { return data; }
    std::string str( const std::string & fstr = "%s" ) const
		{
			if( fstr == "%s" )
				return save_to_db();

			return format( fstr, data.value );
		}

    DBTypeEnum<ETYPE> & operator=( ETYPE d ) { data = d; return *this; }
    DBTypeEnum<ETYPE> & operator=( const std::string &s ) { load_from_db( s ); return *this; }
};


class DBTypeVarChar : public DBType
{
 public:
  std::string data;
  unsigned size;

 public:
  DBTypeVarChar( DBBindType* parent, const std::string &name_, unsigned size_ = 256 )
    : DBType( name_, TYPE::VARCHAR ), size( size_ )
    {
      parent->add( this );
    }

  DBTypeVarChar( const std::string &name_ = "", unsigned size_ = 256 )
	: DBType( name_, TYPE::VARCHAR ), size( size_ )
	{
	}

    void load_from_db( const std::string &data_ );
    std::string save_to_db() const;

    std::string operator()() const { return data; }
    std::string str( const std::string &fstr = "%s" ) const { return format( fstr, data ); }
    operator const char* () const { return data.c_str(); }

    DBTypeVarChar & operator=( const std::string & d ) { data = d; return *this; }

    unsigned get_size() { return size; }
};

class DBTypeDateTime : public DBType
{
 public:
  std::string data;
  unsigned size;

 public:
  DBTypeDateTime( DBBindType* parent, const std::string &name_ )
    : DBType( name_, TYPE::DATETIME ), size( 19 )
    {
      parent->add( this );
    }

  DBTypeDateTime( const std::string &name_ = "" )
	: DBType( name_, TYPE::DATETIME ), size( 19 )
	{
	}

    void load_from_db( const std::string &data_ );
    std::string save_to_db() const;

    std::string operator()() const { return data; }
    std::string str( const std::string &fstr = "%s" ) const { return format( fstr, data ); }
    operator const char* () const { return data.c_str(); }

    DBTypeDateTime & operator=( const std::string & d ) { data = d; return *this; }

    unsigned get_size() { return size; }
};

template<class T> int StdSqlSelect( Database &db, const std::string &table_name, 
				     std::vector< Ref<T> > &rows, 
				     const std::string extra = std::string() )
{
  T type;

  DBRow what( type.get_names() );

  DBErg<DBRowList> erg = db.select( table_name, what, extra );

  if( !erg )
    return -1;

  for( unsigned j = 0; j < erg.row_list.values.size(); j++ )
    {
      for( unsigned i = 0; i < erg.row_list.names.size(); i++ )
	{
	  if(!type.load_from_db( erg.row_list.names[i], erg.row_list.values[j][i] ) )
	    return -1;
	}

      Ref<T> rt = new T();

      *rt = type;

      rows.push_back( rt );
    }

  return rows.size();
}

template<class Storage=DBType> struct DBInList : public std::vector<Storage*>
{
  DBInList() : std::vector<Storage*>() {}
};

template<> struct DBInList<DBType> : public std::vector<DBType*>
{
  DBInList() : std::vector<DBType*>() {}

  template<class T> DBInList<DBType> & operator>>( T &a )
  {
    push_back( &a );
    return *this;
  }
};

template<> struct DBInList<DBBindType> : public std::vector<DBBindType*>
{
  unsigned types;

  DBInList() : std::vector<DBBindType*>(), types(0) {}

  template<class T> DBInList<DBBindType> & operator>>( T &a )
  {
    types++;
    push_back( &a );
    return *this;
  }

  const DBBindType* get_type( unsigned i ) const 
    { 
      if( types > i ) 
	return operator[](i); 
      return 0;
    }

  unsigned types_count() const { return types; }
};

class DBInArrayList : public std::vector<DBBindType*>
{
  private:
  std::vector< std::vector<DBBindType*> > types;
  unsigned a_size;

 public:
 DBInArrayList() : std::vector<DBBindType*>(), a_size(0) { }

  template<class T> DBInArrayList & operator>>( T &a )
  {
    const unsigned array_size =  sizeof(a) / sizeof( a[0] );

    if( a_size && a_size != array_size )
      throw BaseException( "Different array_sizes are not allowed!" );

    a_size = array_size;

    std::vector<DBBindType*> bt( array_size );

    for( unsigned i = 0; i < array_size; i++ )
      {
	bt[i] = &a[i];
      }

    types.push_back( bt );
    
    return *this;
  }

  unsigned types_count() const { return types.size(); }

  const DBBindType* get_type( unsigned i ) const 
    { 
      if( types.size() > i && types[i].size() ) 
	return types[i][0]; 
      return 0;
    }

  void prepare();
};


struct DBInLimit
{
  const unsigned max_data;
  unsigned current;

  DBInLimit( unsigned max_data_ = 1 ) : max_data ( max_data_ ), current( 0 ) {}
  
  void reset() { current = 0; }
};

int StdSqlSelect( Database &db, const std::string &sql, DBInList<DBType> &in );
int StdSqlSelect( Database &db, const std::string &sql, DBInList<DBType> &in, DBInLimit & limit );
						       
int StdSqlSelect( Database &db, const std::string &sql, DBInList<DBBindType> &in );
int StdSqlSelect( Database &db, const std::string &sql, DBInList<DBBindType> &in, DBInLimit & limit );

int StdSqlSelect( Database &db, const std::string & sql, DBInArrayList &in );
int StdSqlSelect( Database &db, const std::string & sql, DBInArrayList &in, DBInLimit & limit );

bool StdSqlInsert( Database &db, const DBBindType &in );
bool StdSqlUpdate( Database &db, const DBBindType &in, const std::string &where = "" );

} // namespace Tools
		  
#endif

#endif
