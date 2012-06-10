/*
 * $Log: cppdir.cpp,v $
 * Revision 1.4  2010/07/21 19:38:25  martin
 * gcc-4 Port
 *
 * Revision 1.3  2005/07/04 21:59:42  martin
 * added logging to all files
 *
 */
#include "cppdir.h"
#include "debug.h"

#define OUT(level) DEBUG_OUT( level, MODULE::CPPDIR)

extern "C" {

#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

}

#include <vector>
#include <stdlib.h>

extern "C" { int lstat(const char *file_name, struct stat *buf); }

std::string CppDir::concat_dir( std::string path, std::string name )
{
  if( name.size() > 0 )
    if( name[0] == '/' && !path.empty() )
      name = name.substr( 1 );

  if( path.size() > 0 )
    {
      if( path[path.size()-1] == '/' )
	path = path.substr( 0, path.size() - 1 );

      path += "/" + name;
    }
  else
    path = name;

  std::string::size_type pos = 0;

  while( true )
    {
      pos = path.find( "/./" );
      if( pos == std::string::npos )
	break;

      path = path.substr( 0, pos ) + path.substr( pos + 2 );
    } 


  DEBUG( OUT(4) << "before beautify: " << path << std::endl );

  path = beautify_path( path );

  DEBUG( OUT(4) << "after beautify: " << path << std::endl );

    /*
  if( path.rfind( '/' ) == path.size() - 1 )
    path = path.substr( 0, path.size() - 1 );
    */

  return path;
}

CppDir::File::File( struct dirent d, std::string p)
{
  inode_number = d.d_ino;
  name = d.d_name;
  path = p;
  link = false;
  

  type = get_type( concat_dir( path, name ) );

  fullpath = concat_dir( path, name );

  valid = !err;
}

CppDir::File::File( std::string path, std::string name )
  : name( name ), path( path )
{
  inode_number = 0;
  link = false;

  type = get_type( concat_dir( path, name ) );

  fullpath = concat_dir( path, name );

  valid = !err;
}


CppDir::File::File( std::string f )
  : fullpath( f )
{
  inode_number = 0;
  link = false;

  type = get_type( f );

  valid = !err;

  split_name( f, path, name );
}


CppDir::EFILE CppDir::File::get_type( const std::string& name )
{
  struct stat stat_buf;

  err = false;

  if( lstat( name.c_str(), &stat_buf ) == -1 )
    {
      err = true;
      return EFILE::UNKNOWN;
    }

  if( S_ISLNK( stat_buf.st_mode ) )
    {
      link = true;

      if( stat( name.c_str(), &stat_buf ) == -1 )
	{
	  err = true;
	  return EFILE::UNKNOWN;
	}
    }


  // extract some other informations
  file_size = stat_buf.st_size;
  date  = stat_buf.st_mtime;
  access_date = stat_buf.st_atime;
  inode_number = stat_buf.st_ino;

  /* getting ids */
  const uid_t fuid = stat_buf.st_uid;
  const gid_t fgid = stat_buf.st_gid;

  static const uid_t uid   = geteuid();
  static const gid_t gid   = getegid();

  static gid_t* gid_list = 0;
  static int gid_list_size = 0;

  if( !gid_list )
    {      
      gid_list_size = getgroups( 0, gid_list );

      /* 
	 That's not a memory whole since the gid_list pointer is static.
	 When the program has finished execution the memory will be freed
	 by the OS automatically.
	 So in theory it is a memory whole, but in practice not.
      */
      gid_list = new gid_t[ gid_list_size ];
      
      getgroups( gid_list_size, gid_list );
      
      if( !in_groups( gid, gid_list_size, gid_list ) )
	{
	  gid_list_size++;

	  gid_t* list = new gid_t[ gid_list_size ];
	  list[ gid_list_size - 1 ] = gid;

	  delete[] gid_list;

	  gid_list = list;
	}
    }
  
#define IS( mode ) \
 ( ( (stat_buf.st_mode) & mode) == (mode) )

  /* checking access to the file */
  if( (fuid == uid) || (uid == 0 ) )
    {
      can_read = IS( S_IRUSR );
      can_write = IS( S_IWUSR );
      can_exec = IS( S_IXUSR );
    }
  else if( in_groups( fgid, gid_list_size, gid_list ) )
    {
      can_read = IS( S_IRGRP );
      can_write = IS( S_IWGRP );
      can_exec = IS( S_IXGRP );
    }
  else 
    {
      can_read = IS( S_IROTH );
      can_write = IS( S_IWOTH );
      can_exec = IS( S_IXOTH );
    }

#undef IS

 if( S_ISREG( stat_buf.st_mode ) )
    return EFILE::REGULAR;

  if( S_ISDIR( stat_buf.st_mode ) )
    return EFILE::DIR;
    
  if( S_ISCHR( stat_buf.st_mode ) )
    return EFILE::CHAR;

  if( S_ISBLK( stat_buf.st_mode ) )
    return EFILE::BLOCK;

  if( S_ISFIFO( stat_buf.st_mode ) )
    return EFILE::FIFO;

  if( link )
    return EFILE::LINK;

  return EFILE::UNKNOWN;
}

inline bool CppDir::File::in_groups( gid_t gid, const int size, gid_t list[] )
{
  for( int i = 0; i < size; ++i )
    if( gid == list[i] )
      return true;
  
  return false;
}

std::string CppDir::File::get_link_buf() const 
{
  if( is_link() )
    return readlink( concat_dir( path, name ) );
  
  return std::string();
}

CppDir::Directory::Directory( std::string pname )
{
  is_open = false;

  valid = open( pname );
}


CppDir::Directory::Directory( File file )
{
  is_open = false;

  valid = open( file );
}

CppDir::Directory::~Directory()
{
  if( is_open )
    close();
}

bool CppDir::Directory::open( File file )
{
  if( is_open )
    {
      error = "Directory already open";
      return false;
    }

  if( !file )
    {
      error = "file not valid";
      return false;
    }

  if( file.get_type() != EFILE::DIR )
    {
      error = "file is not a direcory";
      return false;
    }

  DEBUG( OUT(3)( "file path: %s \tfile name: %s\n", file.get_path(), file.get_name() ) );

  return open( concat_dir( file.get_path(), file.get_name() ) );
}

bool CppDir::Directory::open( std::string fname )
{
  files.clear();

  if( is_open )
    {
      error = "Directory already open";
      return false;
    }

  if( (dir = opendir( fname.c_str() )) == 0 )
    {
      error = "commad opendir failed on file name \"" + fname + "\"";
      
      switch( errno )
	{
	case EACCES: error += "\nEACCES Permission denied."; break;
	case EMFILE:  error += "\nEMFILE Too many file descriptors in use by process."; break;
	case ENFILE:  error += "\nENFILE Too many files are currently open in the system."; break;
	case ENOENT:  error += "\nENOENT Directory does not exist, or name is an empty string."; break;
	case ENOMEM:  error += "\nENOMEM Insufficient memory to complete the operation."; break;
	case ENOTDIR: error += "\nENOTDIR name is not a directory."; break;
	default:      error += "\nundocumented setting of errno."; break;
	}

      return false;
    }

  is_open = true;

  name = fname;

  while( true )
    {
      struct dirent* d = readdir( dir );
      if( d == 0 )
	break;

      files.push_back( File( *d , fname ) );
    }

  close();

  return true;
}

void CppDir::Directory::close()
{
  if( is_open )
    {
      closedir( dir );
      is_open = false;
    }
}

std::ostream& CppDir::operator << ( std::ostream& out , EFILE type )
{
  switch( type )
    {
    case EFILE::UNKNOWN: out << "UNKNOWN"; break;
    case EFILE::FIFO   : out << "FIFO"   ; break;
    case EFILE::CHAR   : out << "CHAR"   ; break;
    case EFILE::DIR    : out << "DIR"    ; break;
    case EFILE::BLOCK  : out << "BLOCK"  ; break;
    case EFILE::REGULAR: out << "REGULAR"; break;
    case EFILE::LINK   : out << "LINK"   ; break;
	case EFILE::FIRST__:
	case EFILE::LAST__:
		break;
    }
  return out;
}

std::string CppDir::pwd()
{
#ifndef PATH_MAX
  const unsigned int PATH_MAX = 100;
#endif
  const unsigned int PATH_INC = PATH_MAX;

  unsigned int path_max = PATH_MAX;
  path_max += 2;		/* The getcwd docs say to do this. */

  char* cwd = static_cast<char*>(malloc(path_max));

  errno = 0;
  char* ret;
  while ((ret = getcwd (cwd, path_max)) == NULL && errno == ERANGE)
    {
      path_max += PATH_INC;
      cwd = static_cast<char*>(realloc (cwd, path_max));
      errno = 0;
    }

  if (ret == NULL)
    {
      int save_errno = errno;
      free (cwd);
      errno = save_errno;
      return "";
    }

  std::string s = cwd;
  free( cwd );

  return s;
}

std::string CppDir::get_full_path( std::string file, std::string current_dir )
{
  if( file.empty() )
    return std::string();

  // absolute path name
  if( file[0] == '/' )
    current_dir = file;
  else
    if( !current_dir.empty() )
      {
	if( current_dir[0] == '/' )
	  {
	    DEBUG( OUT(4) << "current_dir: " << current_dir << std::endl );
	    DEBUG( OUT(4) << "file: " << file << std::endl );
	    current_dir = concat_dir( current_dir, file );
	  }
	else
	  {
	    current_dir = concat_dir( exec_path, current_dir );
	  }
      }
    else
      current_dir = exec_path;

  // simplify path

  DEBUG( OUT(5) << "current_dir1: " << current_dir << std::endl );

  current_dir = beautify_path( current_dir );

  DEBUG( OUT(5) << "current_dir2: " << current_dir << std::endl );

  return current_dir;
}

void CppDir::split_name(std::string file_name, std::string & path, std::string & name )
{
  if( file_name == "/" )
    {
      name = file_name;
      path = file_name;
      return;
    }

  if( file_name.rfind( '/' ) == file_name.size() - 1 )
    file_name = file_name.substr( 0, file_name.size() - 1 );
    
  std::string::size_type p = file_name.rfind( '/' );
  if( p == std::string::npos )
    name = file_name;
  else
    {
      name = file_name.substr( p + 1 );
      
      if( p == 0 )
	p = 1;

      path = file_name.substr( 0, p );
    }
} 

std::string CppDir::simplify_path( std::string path )
{
  
  path = beautify_path( path );

  /*
  while( true )
    {
      std::string::size_type first, second;

      if( (first = path.find( "/../" ) ) == std::string::npos )
	break;

      if( (second = path.rfind( '/', first - 1 ) ) != std::string::npos )
	{
	  std::string left = path.substr( 0, second );
	  std::string right = path.substr( first + 4 );

	  path = CppDir::concat_dir( left, right );
	}
    }
  */ 

  if( path.find( CppDir::exec_path ) == 0 )
    {
      if( path == exec_path )
	path = std::string();
      else
	path = path.substr( exec_path.size() + 1);
    }
  
  return path;
}

std::string CppDir::readlink( const std::string &path )
{
#ifndef PATH_MAX
  const unsigned int PATH_MAX = 100;
#endif
  const unsigned int PATH_INC = PATH_MAX;
  
  unsigned int path_max = PATH_MAX;

  path_max += 2;		/* The getcwd docs say to do this. */
  
  char* buffer = static_cast<char*>(malloc(path_max));

  errno = 0;

  int ret;

  while( ( (ret = ::readlink( path.c_str(), buffer, path_max ) ) == -1 ) && 
           ( errno == EINVAL || errno == 0 ) )
    {
      path_max += PATH_INC;
      buffer = static_cast<char*>(realloc(buffer, path_max) );
      errno = 0;
    }

  std::string s( buffer, ret );

  free( buffer );

  return s;
}

std::string CppDir::beautify_path( std::string path )
{
  while( true )
    {
      std::string::size_type first, second;
      
      if( (first = path.find( "/../" ) ) == std::string::npos )
	break;

      if( (second = path.rfind( '/', first - 1 ) ) != std::string::npos )
	{
	  std::string left = path.substr( 0, second );
	  std::string right = path.substr( first + 4 );

	  path = CppDir::concat_dir( left, right ); 

	  if( second == 0 )
	    path = '/' + path;
	}
    }

  std::string::size_type pos = 0;

  while( true )
    {
      pos = path.find( "/./" );
      if( pos == std::string::npos )
	break;

      path = path.substr( 0, pos ) + path.substr( pos + 2 );
    } 

  if( path.rfind( "/." ) == path.size() - 2 )
    path = path.substr( 0, path.size() - 2 );
  else
    {

      if( path.rfind( '/' ) == path.size() - 1 )
	path = path.substr( 0, path.size() - 1 );

    }

  return path;
}

std::string CppDir::make_relative( std::string path, std::string dir )
{
  typedef std::vector<std::string> vec_string;

  // split path into it's components

  DEBUG( OUT(4) << "path: " << path << std::endl );
  DEBUG( OUT(4) << "dir: " << dir << std::endl );

  vec_string lpath;

  while( true )
    {
      std::string::size_type first;

      if( (first = path.find( '/' ) ) == std::string::npos )
	{
	  if( !path.empty()  )
	    {
	      DEBUG( OUT(3) << "push from path: " << path + '/'  << std::endl );
	      lpath.push_back( path + '/' );
	    }

	  break;
	}

      std::string left = path.substr( 0, first + 1 );

      DEBUG( OUT(4) << "push from path: " << left << std::endl );

      lpath.push_back( left );

      path = path.substr( first + 1 );
    }

  vec_string ldir;

  while( true )
    {
      std::string::size_type first;

      if( (first = dir.find( '/' ) ) == std::string::npos )
	{
	  if( !dir.empty()  )
	    {
	      DEBUG( OUT(3) << "push from dir: " << dir + '/' << std::endl );
	      ldir.push_back( dir + '/' );
	    }

	  break;
	}

      std::string left = dir.substr( 0, first + 1 );

      DEBUG( OUT(4) << "push from dir: " << left << std::endl );

      ldir.push_back( left );

      dir = dir.substr( first + 1 );
    }
  
  unsigned int i = 0;

  while( i < ldir.size() && i < lpath.size() && ldir[i] == lpath[i] )
    ++i;

  std::string res;

  for( unsigned int k = i; k < lpath.size(); ++k )
    res += "../";

  unsigned int k;

  for( k = i; k < ldir.size(); ++k )
    {
      res += ldir[k];
      DEBUG( OUT(4) << "res: " << res << std::endl );
    }

  // remove '/' 
  
  if( res.rfind( '/' ) == res.size() - 1 )
    res = res.substr( 0, res.size() - 1 );
  
  DEBUG( OUT(4) << "res: " << res << std::endl );

  return res;
}

/**

The function can differ /foo/barfoo from /foo/bar 

*/
bool CppDir::is_in_dir( const std::string &path, const std::string &dir )
{
  if( path.empty() || dir.empty() )
    {
      DEBUG( OUT(1) << "WARNING: dir or path is empty\n" );
      return false;
    }

  if( path.find( dir ) != 0 )
    return false;

  if( dir[ dir.size() -1 ] != '/' )
    {
      if( path.size() <= dir.size() )
	return true;

      if( path[dir.size()] == '/' )
	return true;      
    }  

  if( dir == "/" )
      return true;

  DEBUG( OUT(4)( "failed is_in_dir: path: %s, dir %s\n", path, dir ) );

  return false;
}
