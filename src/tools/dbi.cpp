#include "string_utils.h"
#include "format.h"
#include "db.h"
#include "dbi.h"
#include "debug.h"

#ifdef TOOLS_USE_DB

namespace Tools {

std::string DBTypeInt::str( const std::string & fstr ) const
{
	return format( fstr, data );
}

void DBTypeInt::load_from_db( const std::string &data_ )
{ 
      data = s2x<int>( data_, 0 ); 
}

std::string DBTypeInt::save_to_db() const
{
  return x2s( data );
}

std::string DBTypeDouble::str( const std::string & fstr  ) const
{
	return format( fstr, data );
}

void DBTypeDouble::load_from_db( const std::string &data_ )
{ 
      data = s2x<double>( data_, 0 ); 
}

std::string DBTypeDouble::save_to_db() const
{
  return x2s( data );
}

void DBTypeVarChar::load_from_db( const std::string &data_ )
{
  data = data_;
}

std::string DBTypeVarChar::save_to_db() const
{
  return data;
}

void DBTypeDateTime::load_from_db( const std::string &data_ )
{
  data = data_;
}

std::string DBTypeDateTime::save_to_db() const
{
  return data;
}

bool DBBindType::load_from_db( const DBRow &row )
{
  bool found = false;

  for( unsigned i = 0; i < row.names.size(); i++ )
    {
      for( unsigned j = 0; j < type_list.size(); j++ )
	{
	  if( row.names[i] == type_list[j]->get_name() || 
	      row.names[i] == table_name + '.' + type_list[j]->get_name()
	      )
	    {
	      type_list[j]->load_from_db( row.values[i] );
	      found = true;
	      break;
	    } else {
           DEBUG( format( "%s != %s && %s != %s.%s", row.names[i], 
                  type_list[j]->get_name(),
                  row.names[i],
                  table_name,
                  type_list[j]->get_name() ) );
        }
	}
    }

  return found;
}

std::vector<std::string> DBBindType::get_values() const
{
  std::vector<std::string> sl;

  for( unsigned i = 0; i < type_list.size(); i++ )
    {
      sl.push_back( type_list[i]->save_to_db() );
    }

  return sl;
}

DBType* DBBindType::get_cell_by_name( const std::string &name )
{
  for( unsigned i = 0; i < type_list.size(); i++ )
    {
      if( name == type_list[i]->get_name() )
	return type_list[i];
    }

  return 0;
}

const DBType* DBBindType::get_cell_by_name( const std::string &name ) const
{
  for( unsigned i = 0; i < type_list.size(); i++ )
    {
      if( name == type_list[i]->get_name() )
	return type_list[i];
    }

  return 0;
}

bool DBBindType::load_from_db( const std::string &name, const std::string &data )
{
  DBType *t = get_cell_by_name( name );

  if( t )
    {
      t->load_from_db( data );
      return true;
    }

  return false;
}

static int StdSqlSelect( Database &db, const std::string &sql, DBInList<DBType> &in, DBInLimit *limit )
{
  std::string extra;

  if( limit )
    extra = format( " limit %d, %d", limit->current, limit->max_data );

  DBErg<DBRowList> erg = db.exec( sql + extra );

  if( !erg )
    return -1;

  if( limit )
    limit->current += erg.row_list.values.size();

  unsigned count = 0;

  for( unsigned j = 0; j < erg.row_list.values.size(); j++ )
    {
      for( unsigned i = 0; i < erg.row_list.values[j].size(); i++ )
	{
	  if( count >= in.size() )
	    return j + 1;

	  in[count++]->load_from_db( erg.row_list.values[j][i] );
	}
    }

  return erg.row_list.values.size();
}


int StdSqlSelect( Database &db, const std::string &sql, DBInList<DBType> &in, DBInLimit & limit )
{
  return StdSqlSelect( db, sql, in, &limit );
}

int StdSqlSelect( Database &db, const std::string &sql, DBInList<DBType> &in )
{
  return StdSqlSelect( db, sql, in, 0 );
}

template<class Storage> static std::string parse_sql( std::string sql, Storage &in )
{
  for( unsigned i = 0; i < in.types_count(); i++ )
    {
      const DBBindType *bt = in.get_type(i);

      if( !bt )
	continue;
	
      std::vector<std::string> sl = split_string( sql, '%' + bt->get_table_name() );
      
      if( sl.size() > 1 )
	{
	  for( unsigned j = 0; j < sl.size(); j++ )
	    {
	      sql = sl[j] + ' ';
	      
	      std::vector<std::string> names = bt->get_names();
	      
	      for( unsigned k = 0; k < names.size(); k++ )
		{
		  if( k )
		    sql += ',';
		  
		  sql += bt->get_table_name() + '.' + names[k];
		}
	      
	      if( sl.size() > j + 1 )
		{
		  sql += sl[j+1];
		  j++;
		}
	    }
	}
    }

  return sql;
}

static int StdSqlSelect( Database &db, std::string sql, DBInList<DBBindType> &in, DBInLimit *limit )
{
  std::string extra;

  if( limit )
    extra = format( " limit %d, %d", limit->current, limit->max_data );

  sql = parse_sql( sql, in );

  DBErg<DBRowList> erg = db.select( sql + extra );

  if( !erg )
    return -1;

  if( limit )
    limit->current += erg.row_list.values.size();

  unsigned i_count = 0;

  for( unsigned j = 0; j < erg.row_list.values.size(); j++ )
    {
      unsigned ic = 0;

      DBRow row( erg.row_list.names, erg.row_list.values[j] );
      for( unsigned i = 0; i < in.types_count(); i++ )
	{
	  if( i + i_count >= in.size() )
	      return j;

	  in[i_count + i]->load_from_db( row );
	  ic++;
	}

      i_count += ic;
    }

  return erg.row_list.values.size();
}

int StdSqlSelect( Database &db, const std::string &sql, DBInList<DBBindType> &in  )
{
  return StdSqlSelect( db, sql, in, 0 );
}

int StdSqlSelect( Database &db, const std::string &sql, DBInList<DBBindType> &in, DBInLimit &limit )
{
  return StdSqlSelect( db, sql, in, &limit );
}


void DBInArrayList::prepare()
{
  resize( types.size() * a_size );
  
  for( unsigned i = 0; i < types.size(); i++ )
    {
      for( unsigned j = 0, count = i; j < types[i].size(); j++, count += types.size() )
	{	  
	  operator[](count) = types[i][j];
	}
    }
}

static int StdSqlSelect( Database &db, std::string sql, DBInArrayList &in, DBInLimit * limit )
{
  std::string extra;

  if( limit )
    extra = format( " limit %d, %d", limit->current, limit->max_data );

  sql = parse_sql( sql, in );

  DBErg<DBRowList> erg = db.select( sql + extra  );
  
  if( !erg )
    return -1;
  
  if( limit )
    limit->current += erg.row_list.values.size();

  unsigned i_count = 0;
  
  in.prepare();
  
  for( unsigned j = 0; j < erg.row_list.values.size(); j++ )
    {
      unsigned ic = 0;
      
      DBRow row( erg.row_list.names, erg.row_list.values[j] );
      for( unsigned i = 0; i < in.types_count(); i++ )
	{
	  if( i + i_count >= in.size() )
	    return j;
	  
	  in[i_count + i]->load_from_db( row );
	  ic++;
	}
      
      i_count += ic;
    }
  return erg.row_list.values.size();
}

int StdSqlSelect( Database &db, const std::string & sql, DBInArrayList &in, DBInLimit & limit )
{
  return StdSqlSelect( db, sql, in, &limit );
}

int StdSqlSelect( Database &db, const std::string & sql, DBInArrayList &in )
{
  return StdSqlSelect( db, sql, in, 0 );
}

bool StdSqlInsert( Database &db, const DBBindType &in )
{
  DBRow row( in.get_names(), in.get_values() );
  
  DBErg<DBRowList> erg = db.insert( in.get_table(), row );

  if( !erg && std::string( db.get_error() ).empty() )
    return true;
    
  return erg.success;
}

bool StdSqlUpdate( Database &db, const DBBindType &in, const std::string &where )
{
  DBRow row( in.get_names(), in.get_values() );
  
  DBErg<DBRowList> erg = db.update( in.get_table(), row, where );

  if( !erg && std::string( db.get_error() ).empty() )
    return true;
    
  return erg.success;
}

} // namespace Tools

#endif
