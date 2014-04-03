/*
 * $Log: merge_info.cpp,v $
 * Revision 1.4  2010/07/21 19:38:25  martin
 * gcc-4 Port
 *
 * Revision 1.3  2005/07/04 21:59:42  martin
 * added logging to all files
 *
 */
#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "local_config.h"
#include "arg.h"
#include "file_option.h"
#include "debug.h"	
#include "cppdir.h"
#include "string_utils.h"

using namespace std;

#define OUT( level ) DEBUG_OUT( level, MODULE::MERGE_INFO )

struct Section
{    
    bool pre_blabla; // everything before '* Menu:'
    std::string description;
    std::vector<std::string> items;  

    Section() : pre_blabla( false ) {}
};

struct Info
{
    std::string pre_text;  // some text before the character 037    
    std::vector<Section> sections;
};

void print_info( Info &info, std::ostream &out )
{
    out << info.pre_text << static_cast<char>(037) << '\n';

    for( unsigned i = 0; i < info.sections.size(); i++ )
    {
	if( !info.sections[i].description.empty() )
	    out << info.sections[i].description;// << " >>" << info.sections[i].pre_blabla;
	
	for( unsigned j = 0; j < info.sections[i].items.size(); j++ )
	    out << info.sections[i].items[j] << '\n';

	if( i + 1 < info.sections.size() )
	    out << '\n';
    }
}

bool parse_file( std::ifstream &in, Info &info )
{
    // search for character 037
    getline( in, info.pre_text, static_cast<char>(037) );

    if( !in || in.eof() )
    {
	// but maybe it's an info file search for 'File: dir'
	
	in.seekg( 0 );
	in.clear();	
	
	// info.pre_text.clear(); // gcc 2.95.3 does not like this
	info.pre_text = std::string();

	bool is_info = false;

	while( !in.eof() ) 
	{
	    long last_line = in.tellg();
	    std::string l;
	    getline( in, l );
	    
	    if( toupper(l).find( "FILE: DIR" ) == 0 ) 
	    {
		// seek back to the right position
		in.seekg( last_line );
		is_info = true;
		break;
	    } else if( l[0] == '*' ) {
		// not an info file		
		break;
	    }

	    info.pre_text += l + '\n';
	}
      
	if( !is_info ) 
	{
	    ERROR( "not an info file\n" );	
	    return false;
	}
    }

    Section current;
    std::string s;
    bool menu_found = false;
    current.pre_blabla = true;

    while( !in.eof() )
    {
	getline( in, s );

	if( !s.empty() )
	{
	    if( s[0] == '*' )
	    {
		if( toupper(s).find( "* MENU:" ) == 0 )
		    menu_found = true;
		
		current.items.push_back( s );
	    }
	    else if( s[0] == ' ' || s[0] == '\t' )
	    {
		if( current.items.size() )
		    current.items[current.items.size() - 1] += '\n' + s;
		else
		    current.description += '\n' + s;
	    } else {

		if( current.items.size() )
		{
		    info.sections.push_back( current );
		    current = Section();
		    current.pre_blabla = !menu_found;
		    current.description = s;
		} else {
		    if( !current.description.empty() )
			current.description += '\n';
		    current.description += s;
		}
	    }
	} else {
	    if( !current.description.empty() )
		current.description += '\n';
	}
    }

    if( !current.description.empty() || current.items.size() )
	info.sections.push_back( current );

    return true;
}

std::string get_between( const std::string &item, std::string sf, std::string ef, bool not_of )
{
    std::string::size_type start, end;
    
    if( not_of ) 
	start = item.find_first_not_of( sf );
    else
	start = item.find_first_of( sf );

    if( start == std::string::npos )
	return std::string();

    if( !not_of ) 
	start++;
    
    end   = item.find( ef, start );

    if( end == std::string::npos )
	return std::string();

    return item.substr( start, end - start );
}

std::string get_name( const std::string &item )
{
    return get_between( item, "* \t", ":", true );
}

std::string get_package( const std::string &item )
{
    return get_between( item, "(", ")", false );
}

bool item_cmp( const std::string &a, const std::string &b )
{
/*
    if( a == b || toupper( a ) == toupper( b ) )
	return true;
*/
    DEBUG( OUT(2) << "name: " << get_name( a ) << "\t" << get_name( b ) << '\n' );
    DEBUG( OUT(2) << "package: " << get_package( a ) << "\t" << get_package( b ) << '\n' );

    bool ret = ( toupper( get_name( a ) )  == toupper( get_name( b ) )  &&
		 toupper( get_package( a ) ) == toupper( get_package( b ) ) );

    DEBUG( OUT(2) << "\t" << (ret ? "true" : "false" ) << '\n' );

    return ret;
}


bool merge_files( Info &a, Info &b )
{
    for( unsigned i = 0; i < b.sections.size(); ++i )
    {
	bool found = false;

	for( unsigned j = 0; j < a.sections.size(); j++ )
	{
	    if( toupper( b.sections[i].description ) == toupper( a.sections[j].description ) )
	    {
		found = true;

		for( unsigned k = 0; k < b.sections[i].items.size(); k++ )
		{
		    bool ifound = false;

		    for( unsigned l = 0; l < a.sections[j].items.size(); l++ )
		    {
			if( item_cmp( a.sections[j].items[l] , b.sections[i].items[k] ) )
			{
			    ifound = true;
			    break;
			}
		    }

		    if( !ifound )
			a.sections[j].items.push_back( b.sections[i].items[k] );
		}

		break;
	    }
	}

	if( !found && b.sections[i].pre_blabla == false )
	    a.sections.push_back( b.sections[i] );
    }

    return true;
}

bool unmerge_files( Info &a, Info &b )
{
    Info u;

    u.pre_text = a.pre_text;

    for( unsigned i = 0; i < a.sections.size(); ++i )
    {
	bool found  = false;

	for( unsigned j = 0; j < b.sections.size(); j++ )
	{
	    if( toupper( b.sections[j].description ) == toupper( a.sections[i].description ) &&
		!b.sections[j].pre_blabla )
	    {
		found = true;

		for( unsigned k = 0; k < a.sections[i].items.size(); k++ )
		{
		    bool ifound = false;

		    for( unsigned l = 0; l < b.sections[j].items.size(); l++ )
		    {
			if( item_cmp( b.sections[j].items[l], a.sections[i].items[k] ) )
			{
			    ifound = true;
			    break;
			}
		    }

		    if( !ifound )
		    {
			bool ufound = false;

			for( unsigned m = 0; m < u.sections.size(); ++m )
			{
			    if( toupper( u.sections[m].description ) == toupper( a.sections[i].description ) )
			    {
				ufound = true;
				u.sections[m].items.push_back( a.sections[i].items[k] );
				break;
			    }
			}
			
			if( !ufound )
			{
			    Section s;
			    s.description = a.sections[i].description;
			    s.items.push_back( a.sections[i].items[k] );
			    u.sections.push_back( s );
			}
		    }
		}
	    }
	}

	if( !found )
	{
	    u.sections.push_back( a.sections[i] );
	}
    }

    a = u;

    return true;
}

void remove_duplicates( Info &a )
{
    Info u;

    u.pre_text = a.pre_text;

    for( unsigned i = 0; i < a.sections.size(); ++i )
    {
	if( a.sections[i].pre_blabla )
	{
	    u.sections.push_back( a.sections[i] );
	    continue;
	}

	for( unsigned m = 0; m < a.sections[i].items.size(); m++ )
	{
	    bool found = false;	    

	    /*
	    out(1) << get_name( a.sections[i].items[m] ) << endl;
	    if( get_name( a.sections[i].items[m] ) == "diff" || 
		a.sections[i].items[m].find( "* diff" ) == 0 )
	    {
		out << "here" << endl;		
	    }
	    */

	    for( unsigned j = 0; j < u.sections.size(); j++ )
	    {
		for( unsigned k = 0; k < u.sections[j].items.size(); k++ )
		{
		    if( item_cmp( a.sections[i].items[m], u.sections[j].items[k] ) )
		    {
			found = true;
			break;
		    }
		}
		
		if( found )
		    break;
	    }

	    if( !found )
	    {
		// have the section already?
		bool sfound = false;

		for( unsigned k = 0; k < u.sections.size(); k++ )
		{
		    if( toupper( u.sections[k].description ) == toupper( a.sections[i].description ) )
		    {
			sfound = true;

			u.sections[k].items.push_back( a.sections[i].items[m] );
			break;
		    }
		}

		if( !sfound )
		{
		    Section s;
		    s.pre_blabla = a.sections[i].pre_blabla;
		    s.description = a.sections[i].description;
		    s.items.push_back( a.sections[i].items[m] );

		    u.sections.push_back( s );
		}
	    }
	}
    }

    a = u;
}

int main( int argc, char **argv )
{
    progname = "merge-info";

    char *uc = getenv( "XSTOW_USE_CURSES" );
    bool use_curses = true;

    if( uc && 
	!std::string( uc ).empty() &&
	std::string( uc ) == "0" )
	use_curses = false;
    
    out.set_debug_level(0);
    vout.set_debug_level(0);
    out.set_module( MODULE::ALL );

    Arg::Arg arg( argc, argv );

    arg.addPrefix( "-" );
    arg.addPrefix( "--" );

    arg.setMinMatch( 1 );

    Arg::OptionChain debug;
    arg.addChainR( &debug );
    debug.setMinMatch( 1 );
    debug.setContinueOnMatch( true );
    debug.setContinueOnFail( true );
    
    Arg::IntOption debug_level( "dl" );
    debug_level.addName( "debug-level" );
    debug_level.setDescription( "Default is 0." );
    debug_level.setMaxValues( 1 );
    debug_level.setMinValues( 1 );
    debug_level.setRequired( true );
    debug.addOptionR( &debug_level );

#ifdef HAVE_CURSES_H

    Arg::FlagOption curses( "C" );
    curses.addName( "no-curses" );
    curses.setDescription( "Do not use curses to detect the console width" );
    arg.addOptionR( &curses );

#endif

    Arg::OptionChain info;
    arg.addChainR( &info );
    info.setMinMatch( 1 );
    info.setContinueOnMatch( false );
    info.setContinueOnFail( true );
    
    Arg::FlagOption help( "h" );
    help.addName( "help" );
    help.setDescription( "Show this page" );
    info.addOptionR( &help );
  
    Arg::FlagOption version( "V" );
    version.addName( "version" );
    version.setDescription( "Show merge-info's version number" );
    info.addOptionR( &version );

    Arg::OptionChain oc;
    arg.addChainR( &oc );
    
    oc.setMinMatch( 1 );
    oc.setContinueOnFail( true );

    Arg::FlagOption unmerge( "u" );
    unmerge.addName( "unmerge" );
    unmerge.setDescription( "remove duplicate entries" );
    
    oc.addOptionR( &unmerge );

    Arg::StringOption outfile( "o", "FILE" );
    outfile.addName( "out-file" );
    outfile.setMinValues( 1 );
    outfile.setMaxValues( 1 );
    outfile.setDescription( "output file (Can be the same as one of the input files.)" );    
    
    oc.addOptionR( &outfile );

    Arg::FlagOption duplicate( "d" );
    duplicate.addName( "duplicate" );
    duplicate.setDescription( "remove duplicate entries which are located in different sections" );
    
    oc.addOptionR( &duplicate );


    Arg::EmptyFileOption files( Arg::FILE::REGULAR );
    files.setRequired( true );
    files.setMinValues( 2 );
    files.setMaxValues( 2 );
    files.setDescription( "info index file" );    

    arg.addOptionR( &files );


    if( !arg.parse() || argc < 3 )
    {
	if( version.getState() )
	{
	    out( "merge-info is a package of XStow version %s (C) 2004-2005 by Martin Oberzalek\n", VERSION );
	    return 0;
	}

	out << "usage: merge-info [options] FILE FILE\n";	

	if( help.getState() )
	{
	    unsigned int console_width = 80;

#ifdef HAVE_CURSES_H	    
	    if( use_curses )
		use_curses = !curses.getState();

	    if( use_curses )
		console_width = Arg::get_terminal_width();
#endif

	    out << arg.getHelp( 5,20,30, console_width ) << std::endl;	    
	    return 0;
	} else {
	    out << " -h for help screen\n";
	}

	return 1;
    }

    if( debug_level.isSet() )
    {
	out.set_debug_level( s2x<int>(*(debug_level.getValues()->begin() )) );
	vout.set_debug_level( s2x<int>(*(debug_level.getValues()->begin() )) );
    }


    Ref<Arg::vec_string> f = files.getValues();

    std::string s_a = (*f)[0];
    std::string s_b = (*f)[1];

    std::ifstream file_a( s_a.c_str(), std::ios_base::in );
    std::ifstream file_b( s_b.c_str(), std::ios_base::in );

    if( !file_a ) 
    {
	ERROR( "cannot open file %s\n", s_a );
	return 2;
    }

    if( !file_b ) 
    {
	ERROR( "cannot open file %s\n", s_b );
	return 2;
    }

    Info info_a, info_b;

    if( !parse_file( file_a, info_a ) ) 
    {
	ERROR( "couldn't parse file %s\n", s_a );
	return 3;
    }

    if( !parse_file( file_b, info_b ) )
    {
	ERROR( "couldn't parse file %s\n", s_b );
	return 3;
    }

    file_a.close();
    file_b.close();

    if( unmerge.getState() )
	unmerge_files( info_a, info_b );
    else
	merge_files( info_a, info_b );    

    if( duplicate.getState() )
    {
	OUT(1) << "removing duplicates\n";
	remove_duplicates( info_a );
    }

    if( outfile.isSet() )
    {
	std::string s_o = *outfile.getValues()->begin();

	std::ofstream out( s_o.c_str(), std::ios_base::out | std::ios_base::trunc );

	if( !out )
	{
	    ERROR( "cannot open %s for writing\n", s_o );
	    return 4;
	}

	print_info( info_a, out );

    } else {

	print_info( info_a, std::cout );

    }

    return 0;
}
