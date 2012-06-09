/*
 * $Log: nignore.cpp,v $
 * Revision 1.3  2005/07/04 21:59:42  martin
 * added logging to all files
 *
 */
#include "nignore.h"

#ifdef CAN_USE_NIGNORE

#include "tree.h"
#include "leoini.h"
#include "debug.h"
#include "setup.h"

extern "C" {
#  include <fnmatch.h>
}


#define OUT(level) DEBUG_OUT(level, MODULE::NIGNORE)

NIgnore::NIgnore( const std::string &name, Leo::Ini &ini  )
    : name( name ),
      is_valid( true )
{
    OUT(1) << "HERE\n";

    read_ini_value( ini, name, "dir", dir );

    std::string foo;

    read_ini_value( ini, name, "follow", foo );
    follow = s2x<bool>( foo );
}

NIgnore::MATCH NIgnore::match( Ref<Node> node, std::string & strip )
{
    return match_dir( node, strip );
}

NIgnore::MATCH NIgnore::match_dir( Ref<Node> node, std::string & strip )
{    
    std::string path = node->getFullPathName();

    DEBUG( OUT(1) << "nignore path full: " << path << '\n' );

    std::string wd = setup.working_dir();
    std::string cp = setup.current_package();
    std::string foo;

    CppDir::split_name( cp, foo, cp );

    DEBUG( OUT(3) << "wd: " << wd << " cp: " << cp << '\n' );

    if( path.size() > CppDir::concat_dir( wd, cp ).size() )
	path = path.substr( CppDir::concat_dir( wd, cp ).size() );

    if( !strip.empty() && strip.size() < path.size() )
	path = path.substr( strip.size() );

    if( path.empty() )
	return MATCH::TRUE;

    OUT(2) << "nignore path: " << path << '\n';

    for( unsigned i = 0; i < dir.size(); ++i )
    {
	if( path == dir[i] )
	{
	    strip = dir[i];
	    OUT(1) << "match equal (strip): " << dir[i] << '\n';
	    return MATCH::TRUE;
	}
    }

    for( unsigned i = 0; i < dir.size(); ++i )
    {
	if( fnmatch( dir[i].c_str(), path.c_str(), FNM_PATHNAME | FNM_PERIOD ) == 0 )
	{
	    OUT(2) << "match (fnmatch): " << dir[i] << '\n';
	    strip = path;
	    return MATCH::TRUE;
	}  
    }


    unsigned max_length = 0;

    for( unsigned i = 0; i < dir.size(); ++i )
    {
	if( path.find( dir[i] ) == 0 )
	{
	    if( dir[i].size() > max_length )
	    {
		strip = dir[i];
		OUT(2) << "match (strip): " << dir[i] << '\n';
		max_length = dir[i].size();
	    }
	}
    }

    if( ( max_length > 0 ) && follow )
	return MATCH::TRUE;

    if( max_length > 0 )
	return MATCH::REQUIRE_NEXT;

    return MATCH::FALSE;
}

std::ostream & operator<<( std::ostream &out, Ref<NIgnore> ni )
{
    out << format( "nignore: %s\n", ni->name );
    out << format( "\tdir: %s\n", ni->dir );
    out << format( "\tfollow: %s\n", ni->follow );

    return out;
}

std::ostream & operator<<( std::ostream &out, Ref<NIgnoreChain> nc )
{
    std::vector< Ref<NIgnore> > & ni = nc->nignores;

    out << "NIgnoreChain: ";

    for( unsigned i = 0; i < ni.size(); ++i )
    {
	out << ni[i]->getName() << '/';
    }

    out << '\n';

    for( unsigned i = 0; i < ni.size(); ++i )
	out << ni[i];

    return out;
}

bool NIgnoreChain::match( Ref<Node> node )
{
    std::string strip;
   
    for( unsigned i = 0; i < nignores.size(); ++i )
    {       
	if( !strip.empty() )
	    OUT(2) << "strip: " << strip << '\n';

	switch( nignores[i]->match( node, strip ) )
	{
	    case NIgnore::MATCH::TRUE: return true;
	    case NIgnore::MATCH::FALSE: return false;
	    case NIgnore::MATCH::REQUIRE_NEXT: break;
		case NIgnore::MATCH::FIRST__:
		case NIgnore::MATCH::LAST__:	
			break;
	}
    }
    
    return false;
}

std::string NIgnoreChain::getName()
{
    std::string ret;

    for( unsigned i = 0; i < nignores.size(); ++i )
	ret+= nignores[i]->getName() + '/';

    return ret;
}

#endif
