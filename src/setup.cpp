/*
 * $Log: setup.cpp,v $
 * Revision 1.4  2010/07/21 19:38:25  martin
 * gcc-4 Port
 *
 * Revision 1.3  2005/07/04 21:59:42  martin
 * added logging to all files
 *
 */
#include <stdlib.h>
#include "setup.h"
#include "string_utils.h"
#include "leoini.h"
#include "cppdir.h"
#include "debug.h"
#include "nignore.h"

#undef OUT

#define OUT( level ) DEBUG_OUT( level, MODULE::SETUP )

Setup setup;

Setup::Debug::Debug( const std::string &s )
  : Section( s ),
    level( s, "level" ),
    module( s, "module" )
{
  ON_INI( add( &level ) ); 
  ON_INI( add( &module ) );
}

Setup::TraversableLinks::TraversableLinks( const std::string &s )
  : Section( s ),
    checker( true ),
    keep_targets( s, "keep-targets" ),
    links( s, "link", &checker ),
    add_if_targets( s, "add-if-target" ),
    add_if_targets_regex( s, "add-if-target-regex" )
{
  ON_INI( add( &keep_targets ) );
  ON_INI( add( &links ) );
  ON_INI( add( &add_if_targets ) );
  ON_INI( add( &add_if_targets_regex ) );

  links.match_only_once = true;
  add_if_targets.match_only_once = true;
  add_if_targets_regex.match_only_once = true;
}

Setup::KeepDirs::KeepDirs( const std::string &s )
  : Section( s ),    
    dirs( s, "dir" )
{
  ON_INI( add( &dirs ) );

  dirs.match_only_once = true;
}

Setup::Matches::Matches( const std::string &s )
  : Section( s ),
    ignore( s, "ignore" ),
    ignore_regex( s, "ignore-regex" ),
    copy( s, "copy" ),
    copy_regex( s, "copy-regex" ),
    nignore( s, "nignore" ),
    ignore_file_in_dir( s, "ignore-file-in-dir" ),
    copy_file_in_dir( s, "copy-file-in-dir" )

{
  ON_INI( add( &ignore ) );
  ON_INI( add( &ignore_regex ) );
  ON_INI( add( &copy ) );
  ON_INI( add( &copy_regex ) );
  ON_INI( add( &nignore ) );
  ON_INI( add( &ignore_file_in_dir ) );
  ON_INI( add( &copy_file_in_dir ) );

  ignore.match_only_once = true;
  ignore_regex.match_only_once = true;
  copy.match_only_once = true;
  copy_regex.match_only_once = true;
  nignore.match_only_once = true;
  ignore_file_in_dir.match_only_once = true;
  copy_file_in_dir.match_only_once = true;
}

Setup::StowDirs::StowDirs( const std::string &s )
  : Section( s ),
    checker( false ),
    dirs( s, "dir", &checker ),
    auto_add_dirs( s, "auto-add-dirs" ),
    auto_add_dirs_regex( s, "auto-add-dirs-regex" )
{
  ON_INI( add( &dirs ) );
  ON_INI( add( &auto_add_dirs ) );
  ON_INI( add( &auto_add_dirs_regex ) );

  dirs.match_only_once = true;
  auto_add_dirs.match_only_once = true;
  auto_add_dirs_regex.match_only_once = true;
}

Setup::ProtectDirs::ProtectDirs( const std::string &s )
  : Section( s ),
    checker( false ),
    dirs( s, "dir", &checker ),
    auto_add_dirs( s, "auto-add-dirs" ),
    auto_add_dirs_regex( s, "auto-add-dirs-regex" ),
    targets( s, "target" ),
    targets_add_traversable( s, "target-add-traversable-links" )
{
  ON_INI( add( &dirs ) );
  ON_INI( add( &auto_add_dirs ) );
  ON_INI( add( &auto_add_dirs_regex ) );
  ON_INI( add( &targets ) );
  ON_INI( add( &targets_add_traversable ) );

  dirs.match_only_once = true;
  auto_add_dirs.match_only_once = true;
  auto_add_dirs_regex.match_only_once = true;
  targets.match_only_once = true;
}

Setup::ConfigFiles::ConfigFiles( const std::string &s )
  : Section( s ),
    files( s, "file" ),
    in_home( s, "in-home" ),
    in_stow_dir( s, "in-stow-dir" ),
    in_other_stow_dirs( s, "in-other-stow-dirs" )
{
  ON_INI( add( &files ) );
  ON_INI( add( &in_home ) );
  ON_INI( add( &in_stow_dir ) );
  ON_INI( add( &in_other_stow_dirs ) );  

  files.match_only_once = true;
}

Setup::Links::Links( const std::string &s )
  : Section( s ),
    absolute_paths( s, "absolute-paths" )
{
  ON_INI( add( &absolute_paths ) );
}

#ifdef CAN_USE_INI
Setup::Exec::Exec()
    : Section( "exec", false ),
    match( "exec", "match" ),
    exec( "exec", "exec" ),
    exec_unstow( "exec", "exec-unstow" )
{
    add( &match );
    add( &exec );
    add( &exec_unstow );
}
#endif

#ifdef CAN_USE_NIGNORE
Setup::IniNIgnore::IniNIgnore( const std::string &s )
    : Section( s, false ),
      dir( s, "dir" ),
      follow( s, "follow" )
{
    add( &dir );
    add( &follow );
}
#endif

Setup::Setup()
  : debug( "debug" ),
    traversable_links( "traverse-links" ),
    keep_dirs( "keep-dirs" ),
    matches( "matches" ),
    stow_dirs( "stow-dirs" ),
    protect_dirs( "protect-dirs" ),
    config_files( "config-files" ),
    links( "links" )
{
  ON_INI( add( &debug ) );
  ON_INI( add( &traversable_links ) );
  ON_INI( add( &keep_dirs ) );
  ON_INI( add( &matches ) );
  ON_INI( add( &stow_dirs ) );
  ON_INI( add( &protect_dirs ) );
  ON_INI( add( &config_files ) );
  ON_INI( add( &links ) );
}

void Setup::add( Section* s )
{
  if( s )
    sections.push_back( s );
}

void Setup::read_ini()
{
#ifdef CAN_USE_INI
  for( unsigned int i = 0; i < config_files.files.size(); ++i )
    {
      Leo::Ini ini( config_files.files[i], std::ios_base::in );

      if( !ini )
	continue;

      OUT(1)( "reading config file %s\n", config_files.files[i] );

      ini.read();

      check_ini( ini );

      for( unsigned int j = 0; j < sections.size(); ++j )
	sections[j]->read_ini( ini );

#ifdef CAN_USE_NIGNORE
      read_nignore( ini );
#endif

      read_exec( ini );
#endif

      // add functions that do not require config file support here

      handle_keep_dirs();
      handle_protect_dirs();
      

#ifdef CAN_USE_INI

      // add functions that require config file support here

      handle_config_files();

    }
#endif
}

void Setup::handle_keep_dirs()
{
  if( traversable_links.keep_targets() )
    {
      for( unsigned int i = 0; i < traversable_links.links.size(); ++i )
	{
	  CppDir::File file( traversable_links.links[i] );
	  
	  if( !file || !file.is_link() )
	    continue;

	  keep_dirs.dirs += CppDir::get_full_path( file.get_link_buf(), file.get_path() );
	}
    }
}

void Setup::handle_protect_dirs()
{
    if( protect_dirs.targets_add_traversable() )
    {
	for( unsigned int i = 0; i < traversable_links.links.size(); ++i )
	{
	    CppDir::File file( traversable_links.links[i] );

	    if( !file || !file.is_link() )
		continue;

	    protect_dirs.targets += CppDir::get_full_path( file.get_link_buf(), file.get_path() );
	}
    }
}

void Setup::handle_config_files()
{
#ifdef CAN_USE_INI

  if( config_files.in_home() )
    {
      char *home = getenv( "HOME" );

      if( home )
	{
	  config_files.files += CppDir::concat_dir( home, ".xstow.ini" );
	  config_files.files += CppDir::concat_dir( home, "xstow.ini" );
	}
    }

  if( config_files.in_stow_dir() )
    {
      config_files.files += CppDir::concat_dir( working_dir(), "xstow.ini" );
    }

  if( config_files.in_other_stow_dirs() )
    {
      for( unsigned int i = 0; i < stow_dirs.dirs.size(); ++i )
	{
	  config_files.files += CppDir::concat_dir( stow_dirs.dirs[i], "xstow.ini" );
	}
    }
#endif  
}

std::ostream& operator<<( std::ostream& out, const Setup &s )
{
  out << "debug_level:    " << s.debug.level           << '\n';
  out << "debug_module:   " << s.debug.module          << '\n';
  out << "no_changes:     " << s.no_changes            << '\n';
  out << "conflicts:      " << s.conflicts             << '\n';
  out << "working_dir:    " << s.working_dir           << '\n';
  out << "target_dir:     " << s.target_dir            << '\n';
  out << "verbose_level:  " << s.verbose_level         << '\n';
  out << "unstow:         " << s.unstow                << '\n';
  out << "restow:         " << s.restow                << '\n';
  out << "simulate:       " << s.simulate              << '\n';
  out << "force:          " << s.force                 << '\n';
  out << "use_curses:     " << s.use_curses            << '\n';
  out << "absolute_paths: " << s.links.absolute_paths  << '\n';

  out << "traverse_links:\n";
  out << setup.traversable_links.links;

  out << "auto_traverse_matches:\n";
  out << s.traversable_links.add_if_targets;

  out << "auto_traverse_regex_matches:\n";
  out << s.traversable_links.add_if_targets_regex;

  out << "keep_dirs:\n";
  out << s.keep_dirs.dirs;

  out << "ignore_matches:\n";
  out << s.matches.ignore;

  out << "ignore_regex_matches:\n";
  out << s.matches.ignore_regex;

#ifdef CAN_USE_NIGNORE
  out << "nignore_matches:\n";
  out << s.matches.nignore;

  out << "nignore chains:\n";
  for( unsigned i = 0; i < s.nignores.size(); ++i )
      out << s.nignores[i];
#endif

  out << "copy_matches:\n";
  out << s.matches.copy;

  out << "copy_regex_matches:\n";
  out << s.matches.copy_regex;

  out << "ignore_file_in_dir:\n";
  out << s.matches.ignore_file_in_dir;

  out << "copy_file_in_dir:\n";
  out << s.matches.copy_file_in_dir;

  out << "stow_dirs:\n";
  out << s.stow_dirs.dirs;

  out << "auto_stow_matches:\n";
  out << s.stow_dirs.auto_add_dirs;

  out << "auto_stow_regex_matches:\n";
  out << s.stow_dirs.auto_add_dirs_regex;

  out << "protect_dirs:\n";
  out << s.protect_dirs.dirs;

  out << "auto_protect_matches:\n";
  out << s.stow_dirs.auto_add_dirs;

  out << "auto_protect_regex_matches:\n";
  out << s.protect_dirs.auto_add_dirs_regex;

  out << "protect_targets:\n";
  out << s.protect_dirs.targets;

  out << "protect_targets_add_traversable: " << s.protect_dirs.targets_add_traversable << '\n';
  
  out << "packages:\n";
  out << s.packages;

  out << "config_files:\n";
  out << s.config_files.files;  

#ifdef CAN_USE_INI
  for( unsigned i = 0; i < s.exec.size(); i++ )
  {
      out << "exec:\n";
      out << " match: " << s.exec[i].match << '\n';
      out << " exec: " << s.exec[i].exec << '\n';
      out << " exec_unstow: " << s.exec[i].exec_unstow << '\n';
  }      
#endif

  return out;
}

#ifdef CAN_USE_NIGNORE

void Setup::read_nignore( Leo::Ini &ini )
{
    vec_string nrules;

    if( read_ini_value( ini, "matches", "nignore", nrules ) )
    {
	for( unsigned j = 0; j < nrules.size(); ++j )
	{
	    std::string rule = nrules[j];

	    std::vector<std::string> rules = split_simple( rule, "/" );
	    
	    Ref<NIgnoreChain> nc = new NIgnoreChain();
	    
	    bool invalid = false;
	    
	    for( unsigned i = 0; i < rules.size(); ++i )
	    {
		Ref<NIgnore> ni = new NIgnore( rules[i], ini  );
		
		if( ni->valid() )		
		{
		    nc->add( ni );
		}
		else
		{		
		    invalid = true;
		    eout( "invalid nignore %s in %s\n", rules[i], ini.get_file_name() );
		    break;
		}
	    }
	    
	    if( invalid || nc->empty() )
		nignores.push_back( nc );
	}
    }
}

#endif


#ifdef CAN_USE_INI
void Setup::read_exec( Leo::Ini &ini )
{
    if( !ini )
	return;

    Leo::Ini::Element isections( Leo::Ini::Element::TYPE::SECTION_LIST );

    if( !ini.read( isections ) )
	return;

    for( Leo::Ini::Element::element_list_it it = isections.elements.begin();
	it != isections.elements.end(); ++it )
    {
	if( it->type == Leo::Ini::Element::TYPE::SECTION &&
	    strip( it->section ) == "exec" )	    
	{
	    Exec e;

	    for( Leo::Ini::Element::element_list_it sit = it->elements.begin();
		 sit != it->elements.end(); ++sit )
	    {
		if( sit->key == "match" )
		    e.match( strip( sit->value ) );
		else if( sit->key =="exec"  )
		    e.exec( strip( sit->value  ) );
		else if( sit->key == "exec-unstow" )
		    e.exec_unstow( strip( sit->value ) );
	    }

	    if( !e.exec().empty() &&
		!e.match().empty() )
		exec.push_back( e );
	    else
		ERROR( "incomplete exec section in ini file %s\n", ini.get_file_name() );
	}
    }
}
#endif


#ifdef CAN_USE_INI
void Setup::check_ini( Leo::Ini &ini )
{
    if( !ini )
    {
	ERROR( "Cannot read ini file %s\n", ini.get_file_name() );
	return;
    }

    std::vector< Ref<Section> > sections;
    sections.reserve( Setup::sections.size() + 1 );

    for( unsigned i = 0; i < Setup::sections.size(); ++i )
	sections.push_back( Ref<Section>( Setup::sections[i], false ) );

    sections.push_back( new Exec() ); // add it for syntax check

#ifdef CAN_USE_NIGNORE
    // add ignore sections
    vec_string nrules;   
    read_ini_value( ini, "matches", "nignore", nrules );

    // add already existing ones
    for( unsigned i = 0; i < nignores.size(); i++ )
    	nrules.push_back( nignores[i]->getName() );
    

    for( unsigned j = 0; j < nrules.size(); ++j )
    {
	std::string rule = nrules[j];
	std::vector<std::string> rules = split_simple( rule, "/" );
	
	for( unsigned k = 0; k < rules.size(); k++ )
	    sections.push_back( new IniNIgnore( rules[k] ) );
    }

#endif

    bool reported = false;
    bool value_error = false;

    Leo::Ini::Element isections( Leo::Ini::Element::TYPE::SECTION_LIST );

    if( !ini.read( isections ) )
	return;

    for( Leo::Ini::Element::element_list_it it = isections.elements.begin();
	 it != isections.elements.end(); ++it )
    {
	if( it->type == Leo::Ini::Element::TYPE::SECTION )
	{
	    std::string s = strip( it->section );
	    bool found = false;	    

	    for( unsigned j = 0; j < sections.size(); j++ )
	    {
		if( sections[j]->get_name() == s )
		{
		    found = true;
		    
		    for( Leo::Ini::Element::element_list_it sit = it->elements.begin();
			 sit != it->elements.end(); ++sit )
		    {

			DEBUG( OUT(2)( "sit->key %s\n", sit->key ) );

			if( !sections[j]->check_ini( ini, sit->key ) )
			{
			    if( !reported )
			    {
				ERROR( " %s\n", ini.get_file_name() );
				reported = true;
			    }

			    eout( "  [%s]\n    %s = %s <= wrong key in this section\n", 
				   s, sit->key, sit->value );
			    value_error = true;
			    break;
			}
		    }

		    if( found )
			break;
		}
	    }

	    if( !found )
	    {
		if( !reported )
		{
		    ERROR( " %s\n", ini.get_file_name() );
		    reported = true;
		}
		if( value_error )
		    eout( "\n" );
		
		eout( "  unknown section [%s]\n", s );
	    }
	}
    }
}
#endif
