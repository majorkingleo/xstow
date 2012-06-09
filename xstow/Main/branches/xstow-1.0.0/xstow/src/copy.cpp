/*
 * $Log: copy.cpp,v $
 * Revision 1.2  2005/07/04 21:59:42  martin
 * added logging to all files
 *
 */
#include "tree.h"
#include <cstdio>
#include "debug.h"

extern "C" {
#include <sys/stat.h>
#include <sys/types.h>
}

// logically this is still the tree module
#define OUT(level) DEBUG_OUT(level, MODULE::TREE)

/*
  On some systems ferror() and feof() are not in the namespace std.  
  But since I do not "like" the 'using' directive, but I wanted to 
  limit the damn #ifdefs too, I putted the stuff into one cpp file.

  Do not wonder that I still write std::string here... that's just my coding style.

  On 32 Bit systems this code has a 2GB file size limit. But who the hell has 
  a > 2GB 'dir', or configuration file to copy?
*/
using namespace std;

bool file_copy( std::string origin, std::string target )
{
    FILE *o,*t;       

    DEBUG( OUT( 3 )( "file_copy: %s => %s\n", origin, target ) );

    if( ( o = fopen( origin.c_str(), "rb" ) ) == NULL )
	return false;
    
    DEBUG( OUT( 3 ) << "opening origin succeeded\n"  );

    if( ( t = fopen( target.c_str(), "w+b" ) ) == NULL )
	return false;

    DEBUG( OUT( 3 ) << "opening target succeeded\n"  );

    const int SIZE = 1024*1024;
    char buffer[SIZE];

    if( ferror( o ) || ferror( t ) )
	return false;

    DEBUG( OUT( 3 ) << "no ferror\n"  );

    do
    {
	size_t rsize = fread( buffer, 1, SIZE, o );
	fwrite( buffer, 1, rsize, t );

	DEBUG( OUT(3)( "wrote %d: bytes error: %d\n", rsize, ferror( o ) ) );


    } while( !feof( o ) && !ferror( o ) && !ferror( t ) );

    fclose( o );
    fclose( t );

    return true;
}

bool dir_copy( std::string origin, std::string target )
{
    CppDir::Directory dir( target );
    
    if( !dir )
    {
	// directory does not exist, create it
	if( mkdir( target.c_str(), 0777 ) != 0 )
	    return false;

	dir = CppDir::Directory( target );
    }

    CppDir::Directory odir( origin );    

    CppDir::Directory::file_list fl = odir.get_files();

    for( CppDir::Directory::file_list_it it = fl.begin(); it != fl.end(); ++it )
    {
	if( it->get_name() == "." ||
	    it->get_name() == ".." )
	    continue;

	std::string no = CppDir::concat_dir( origin, it->get_name() );
	std::string nt = CppDir::concat_dir( target, it->get_name() );

	if( it->get_type() == CppDir::EFILE::DIR )
	{
	    if( !dir_copy( CppDir::concat_dir( origin, it->get_name() ),
			   CppDir::concat_dir( target, it->get_name() ) ) )
		return false;

	} else {

	    OUT(1)( "COPY: %s => %s\n", no, nt );
	
	    if( !file_copy(  no, nt  ) )
		return false;

	}
    }
   
    return true;
}
