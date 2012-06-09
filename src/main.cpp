/*
 * $Log: main.cpp,v $
 * Revision 1.3  2010/07/21 19:38:25  martin
 * gcc-4 Port
 *
 * Revision 1.2  2005/07/04 21:59:42  martin
 * added logging to all files
 *
 */
#include <iostream>
#include <stdlib.h>

#include "local_config.h"

#include "arg.h"
#include "file_option.h"
#include "debug.h"
#include "setup.h"
#include "cppdir.h"
#include "string_utils.h"

#include "tree.h"

using namespace std;

#define OUT( level ) DEBUG_OUT( level, MODULE::MAIN )

int main( int argc, char** argv )
{
    BT;

    progname = "xstow";

  char *dl = getenv( "XSTOW_DEBUG_LEVEL" );
  char *dm = getenv( "XSTOW_DEBUG_MODULE" );
  char *uc = getenv( "XSTOW_USE_CURSES" );

  out.set_debug_level( 0 );
  vout.set_debug_level( 0 );

  if( dl )
    out.set_debug_level( s2x<int>( dl ) );

  if( dm )
    out.set_module( string2module( toupper( dm  ) ) );

  setup.use_curses( true );

  if( uc && 
      !std::string( uc ).empty() &&
      std::string( uc ) == "0" )
      setup.use_curses( false );

  Arg::Arg arg( argc, argv );

  arg.addPrefix( "-" );
  arg.addPrefix( "--" );

  arg.setMinMatch(1);

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

  Arg::EnumOption debug_module( "dm" );
  debug_module.addName( "debug-module" );
  debug_module.setDescription( "Show debug messages of a specific module" );
  debug_module.setMaxValues( 1 );
  debug_module.setMinValues( 1 );
  debug_module.addEnum( "ALL" );
  debug_module.addEnum( "ARG" );
  debug_module.addEnum( "MAIN" );
  debug_module.addEnum( "TREE" );
  debug_module.addEnum( "CPPDIR" );
  debug_module.addEnum( "SETUP" );
  debug.addOptionR( &debug_module );

#ifdef HAVE_CURSES_H

  Arg::FlagOption curses( "C" );
  curses.addName( "no-curses" );
  curses.setDescription( "Do not use curses to detect the console width" );
  arg.addOptionR( &curses );

#endif

#ifdef CAN_USE_INI
  Arg::FileOption files( "F", Arg::FILE::ANY );
  files.addName( "file" );
  files.setDescription( "Read this config file too");
  arg.addOptionR( &files );
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
  version.addName( "Version" );
  version.setDescription( "Show XStow's version number" );
  info.addOptionR( &version );

  Arg::OptionChain oc;
  arg.addChainR( &oc );

  oc.setMinMatch( 1 );
  oc.setContinueOnFail( true );

  Arg::FlagOption no_changes( "n" );
  no_changes.addName( "no" );
  no_changes.setDescription( "Do not actually make changes" );
  oc.addOptionR( &no_changes );

  Arg::FlagOption conflicts( "c" );
  conflicts.addName( "conflicts" );
  conflicts.setDescription( "Scan for conflicts, implies -n" );
  oc.addOptionR( &conflicts );

  Arg::FlagOption simulate( "s" );
  simulate.addName( "simulate" );
  simulate.setDescription( "Simulate installation (ignore permissions)" );
  oc.addOptionR( &simulate );

  Arg::FileOption working_dir( "d", Arg::FILE::DIR );
  working_dir.addName( "dir" );
  working_dir.setDescription( "Set stow dir to DIR (default is current dir)" );
  oc.addOptionR( &working_dir );

  Arg::FileOption target_dir( "t", Arg::FILE::DIR );
  target_dir.addName( "target" );
  target_dir.setDescription( "Set target to DIR (default is parent of stow dir)" );
  oc.addOptionR( &target_dir );

  
  Arg::EnumOption verbose("v");
  verbose.addName( "verbose" );
  verbose.setDescription( "Increase verboseness (levels are 0,1,2,3; -v or --verbose adds 1" );
  verbose.addEnum( "0" );
  verbose.addEnum( "1" );
  verbose.addEnum( "2" );
  verbose.addEnum( "3" );
  verbose.setMaxValues( 1 );
  verbose.setMinValues( 0 );
  oc.addOptionR( &verbose );

  Arg::StringOption traversable_links( "tr", "LINKS" );
  traversable_links.addName( "traversable" );
  traversable_links.setDescription( "A list of links pointing to diretories, that can be "
				    "ignored. eg.: /usr/local/man is a link to /usr/local/share/man. " 
				    "Warning: Breaks compatibility with Stow!"
				    );
  traversable_links.setMaxValues( 1 );
  traversable_links.setMinValues( 1 );
  oc.addOptionR( &traversable_links );

  Arg::FlagOption traversable_links_keep( "tkt" );
  traversable_links_keep.addName( "tr-keep-targets" );
  traversable_links_keep.setDescription( "Add the list of traversable links also to the keep targets list");
  oc.addOptionR( &traversable_links_keep );

#ifdef HAVE_FNMATCH_H
  Arg::StringOption traversable_links_add( "tra", "PATTERN" );
  traversable_links_add.addName( "tr-auto" );
  traversable_links_add.setDescription( "Automatically add links which target matches this pattern."
				    );
  traversable_links_add.setMaxValues( 1 );
  traversable_links_add.setMinValues( 1 );
  oc.addOptionR( &traversable_links_add );
#endif

#ifdef HAVE_REGEX_H
  Arg::StringOption traversable_links_add_regex( "tre", "REGEX" );
  traversable_links_add_regex.addName( "tr-auto-regex" );
  traversable_links_add_regex.setDescription( "Automatically add links which target matches this pattern."
				    );
  traversable_links_add_regex.setMaxValues( 1 );
  traversable_links_add_regex.setMinValues( 1 );
  oc.addOptionR( &traversable_links_add_regex );
#endif

  Arg::StringOption keep_dirs( "kd", "DIRS" );
  keep_dirs.addName( "keep-dirs" );
  keep_dirs.setDescription( "A list of directories, that should not be removed when a package will be removed. "
			    "Warning: Breaks compatibilty with Stow!" );
  keep_dirs.setMinValues( 1 );
  keep_dirs.setMaxValues( 1 );
  oc.addOptionR( &keep_dirs );

  Arg::FlagOption unstow( "D" );
  unstow.addName( "delete" );
  unstow.setDescription( "Unstow instead of stow");
  oc.addOptionR( &unstow );

  Arg::FlagOption restow( "R" );
  restow.addName( "restow" );
  restow.setDescription( "Restow (like 'xstow -D foo' followed by 'xstow foo')" );
  oc.addOptionR( &restow );

  Arg::FlagOption absolute_paths( "ap" );
  absolute_paths.addName( "absolute-path" );
  absolute_paths.setDescription( "Create symlinks with absolute path names. Warning: Breaks compatibility with Stow!" );
  oc.addOptionR( &absolute_paths );

#ifdef HAVE_FNMATCH_H
  Arg::StringOption ignore( "i", "PATTERN" );
  ignore.addName( "ignore" );
  ignore.setDescription( "Ignore file and directories matching this expression." );
  ignore.setMinValues( 1 );
  ignore.setMaxValues( 1 );
  oc.addOptionR( &ignore );
#endif

#ifdef HAVE_REGEX_H
  Arg::StringOption ignore_regex( "ire", "REGEX" );
  ignore_regex.addName( "ignore-regex" );
  ignore_regex.setDescription( "Ignore file and directories matching this expression." );
  ignore_regex.setMinValues( 1 );
  ignore_regex.setMaxValues( 1 );
  oc.addOptionR( &ignore_regex );
#endif

#ifdef CAN_USE_NIGNORE
  Arg::StringOption nignore( "ni", "PATTERN" );
  nignore.addName( "nignore" );
  nignore.setDescription( "Ignore everything execpt file and directories matching this expression." );
  nignore.setMinValues( 1 );
  nignore.setMaxValues( 1 );
  oc.addOptionR( &nignore );
#endif

#ifdef HAVE_FNMATCH_H
  Arg::StringOption copy( "cp", "PATTERN" );
  copy.addName( "copy" );
  copy.setDescription( "Copy files and directories matching this expression." );
  copy.setMinValues( 1 );
  copy.setMaxValues( 1 );
  oc.addOptionR( &copy );
#endif

  Arg::StringOption ignore_file_in_dir( "ifd", "DIR/FILE" );
  ignore_file_in_dir.addName( "i-file-in-dir" );
  ignore_file_in_dir.setDescription( "Ignore this file in this directory. (Relative to the package dir.)" );
  ignore_file_in_dir.setMinValues( 1 );
  ignore_file_in_dir.setMaxValues( 1 );
  oc.addOptionR( &ignore_file_in_dir );

  Arg::StringOption copy_file_in_dir( "cfd", "DIR/FILE" );
  copy_file_in_dir.addName( "c-file-in-dir" );
  copy_file_in_dir.setDescription( "Copy this file in this directory. (Relative to the package dir.)" );
  copy_file_in_dir.setMinValues( 1 );
  copy_file_in_dir.setMaxValues( 1 );
  oc.addOptionR( &copy_file_in_dir );

#ifdef HAVE_REGEX_H
  Arg::StringOption copy_regex( "cre", "REGEX" );
  copy_regex.addName( "copy-regex" );
  copy_regex.setDescription( "Copy files and directories matching this expression." );
  copy_regex.setMinValues( 1 );
  copy_regex.setMaxValues( 1 );
  oc.addOptionR( &copy_regex );
#endif


  Arg::FileOption stow_dirs( "sd", Arg::FILE::DIR );
  stow_dirs.addName( "stow-dirs" );
  stow_dirs.setDescription( "inform xstow about other stow directories" );
  stow_dirs.setMinValues( 1 );
  stow_dirs.setMaxValues( 1 );
  oc.addOptionR( &stow_dirs );

#ifdef HAVE_FNMATCH_H
  Arg::StringOption stow_dirs_add( "sda", "PATTERN" );
  stow_dirs_add.addName( "sd-auto" );
  stow_dirs_add.setDescription( "Automatically add directories to the list when matching this expression." );
  stow_dirs_add.setMinValues( 1 );
  stow_dirs_add.setMaxValues( 1 );
  oc.addOptionR( &stow_dirs_add );
#endif

#ifdef HAVE_REGEX_H
  Arg::StringOption stow_dirs_add_regex( "sde", "REGEX" );
  stow_dirs_add_regex.addName( "sd-auto-regex" );
  stow_dirs_add_regex.setDescription( "automatically add directories to the list when matching this expression." );
  stow_dirs_add_regex.setMinValues( 1 );
  stow_dirs_add_regex.setMaxValues( 1 );
  oc.addOptionR( &stow_dirs_add_regex );
#endif

  Arg::FileOption protect_dirs( "pd", Arg::FILE::DIR );
  protect_dirs.addName( "protect-dirs" );
  protect_dirs.setDescription( "Never install anything in this directory" );
  protect_dirs.setMinValues( 1 );
  protect_dirs.setMaxValues( 1 );
  oc.addOptionR( &protect_dirs );

#ifdef HAVE_FNMATCH_H
  Arg::StringOption protect_dirs_add( "pda", "PATTERN" );
  protect_dirs_add.addName( "pd-auto" );
  protect_dirs_add.setDescription( "Automatically add directories to the list when matching this expression." );
  protect_dirs_add.setMinValues( 1 );
  protect_dirs_add.setMaxValues( 1 );
  oc.addOptionR( &protect_dirs_add );
#endif

#ifdef HAVE_REGEX_H
  Arg::StringOption protect_dirs_add_regex( "pde", "REGEX" );
  protect_dirs_add_regex.addName( "pd-auto-regex" );
  protect_dirs_add_regex.setDescription( "automatically add directories to the list when matching this expression." );
  protect_dirs_add_regex.setMinValues( 1 );
  protect_dirs_add_regex.setMaxValues( 1 );
  oc.addOptionR( &protect_dirs_add_regex );
#endif

  Arg::StringOption protect_dirs_target( "pdt", "DIRS" );
  protect_dirs_target.addName( "pd-targets" );
  protect_dirs_target.setDescription( "Only allow installing into this directory." );
  protect_dirs_target.setMinValues( 1 );
  protect_dirs_target.setMaxValues( 1 );
  oc.addOptionR( &protect_dirs_target );

  Arg::FlagOption protect_dirs_targets_add_traversable( "pta" );
  protect_dirs_targets_add_traversable.addName( "pdt-add-traversable" );
  protect_dirs_targets_add_traversable.setDescription( "Add the list of traversable links also to the targets list");
  oc.addOptionR( &protect_dirs_targets_add_traversable );

  Arg::FlagOption force( "f" );
  force.addName( "force" );
  force.setDescription( "Skip conflicts if possible." );
  oc.addOptionR( &force );

//  Arg::EmptyFileOption packages( Arg::FILE::DIR );
  Arg::EmptyStringOption packages;
  packages.setRequired( true );
  packages.setMinValues( 1 );
  packages.setMaxValues( Arg::Option::infinite );
  oc.addOptionR( &packages );

  DEBUG( OUT(2) << "parsing arguments" << std::endl );

  if( !arg.parse() || argc <= 1 )
    {
      DEBUG( OUT(2) << "parsing failed" << std::endl );

      if( version.getState() )
	{
	  out( "XStow Version %s (C) 2002-2005 by Martin Oberzalek\n", VERSION );

#ifndef NDEBUG
	  out << "\t advanced debugging support\n";
#endif
	  
#ifdef HAVE_REGEX_H
	  out << "\t regular expression support\n";
#endif

#ifdef HAVE_FNMATCH_H
	  out << "\t shell pattern matching support\n";
#endif

#ifdef CAN_USE_INI
	  out << "\t configuration file support\n";
#endif

	  return 0;
	}

#ifdef HAVE_CURSES_H
      if( setup.use_curses() )
	  setup.use_curses( !curses.getState() );
#endif

      if( help.getState() )
	{
	  out << "Usage: xstow [OPTION ...] PACKAGE\n"; 

	  unsigned int console_width = 80;

	  if( setup.use_curses() )
	      console_width = Arg::get_terminal_width();
	      

	  out << arg.getHelp(5,20,30, console_width ) << std::endl;
	  return 0;
	}

      out << "Usage: xstow [OPTION ...] PACKAGE\n"; 
      out << " -h for help screen\n";

      return 1;
    }

  DEBUG( OUT(2) << "parsing succeded" << std::endl );


  // store all in setup

  if( debug_module.isSet() )
    {      
      setup.debug.module( *(debug_module.getValues()->begin()) );
      setup.debug.module.freeze( true );
    }
  else
    setup.debug.module( MODULE::ALL );



  if( debug_level.isSet() )
    {
      setup.debug.level( *(debug_level.getValues()->begin() ) );
      setup.debug.level.freeze( true );
    }
  else
    setup.debug.level( 0 );

#ifdef HAVE_CURSES_H

  if( setup.use_curses() )
      setup.use_curses( !curses.getState() );
  
#endif

  setup.no_changes( no_changes.getState() );
  setup.conflicts( conflicts.getState() );

  if( working_dir.isSet() )
    setup.working_dir( CppDir::get_full_path( *(working_dir.getValues()->begin()) ) );
  else
    setup.working_dir( CppDir::exec_path );

  // add working dir to stow dirs for matching all cases
  setup.stow_dirs.dirs += setup.working_dir();

  if( target_dir.isSet() )
    setup.target_dir( CppDir::get_full_path( *(target_dir.getValues()->begin()) ) );
  else
    {
      std::string path, name;

      CppDir::split_name( setup.working_dir(), path, name );

      setup.target_dir( path );
    }

  if( verbose.isSet() )
    {
      if( verbose.getValues()->empty() )
	setup.verbose_level( 1 );
      else
	setup.verbose_level( *(verbose.getValues()->begin()));
    } 
  else 
    setup.verbose_level( 0 );
    

  setup.unstow( unstow.getState() );
  setup.restow( restow.getState() );

  setup.simulate( simulate.getState() );

  setup.force( force.getState() );

  setup.links.absolute_paths( absolute_paths.getState() );
  
  Ref<Arg::vec_string> p = packages.getValues();

  for( unsigned int i = 0; i < p->size(); ++i )
  {
      // check if directories exists
      // that's not done by the option anymore since we are trying getting 
      // compatible to Stow :(

      Arg::FileOptionRule check_exist( Arg::FILE::DIR );

      std::string current_path = setup.working_dir();

      std::string pa = CppDir::get_full_path( (*p)[i], current_path );

      if( check_exist.match( pa ) )
	  setup.packages += pa;
      else
      {
	  ERROR( "cannot find package %s\n", (*p)[i] );
	  return 1;
      }
  }
    


  if( traversable_links.isSet() )
    setup.traversable_links.links += split_simple( *traversable_links.getValues()->begin() );

  setup.traversable_links.keep_targets( traversable_links_keep.getState() );

#ifdef HAVE_FNMATCH_H
  if( traversable_links_add.isSet() )
    setup.traversable_links.add_if_targets += split_simple( *traversable_links_add.getValues()->begin() );
#endif

#ifdef HAVE_REGEX_H
  if( traversable_links_add_regex.isSet() )
    setup.traversable_links.add_if_targets_regex += split_simple( *traversable_links_add_regex.getValues()->begin() );
#endif



#ifdef HAVE_FNMATCH_H
  if( ignore.isSet() )
    setup.matches.ignore += split_simple( *ignore.getValues()->begin() );
#endif

#ifdef HAVE_REGEX_H
  if( ignore_regex.isSet() )
    setup.matches.ignore_regex += split_simple( *ignore_regex.getValues()->begin() );
#endif

#ifdef CAN_USE_NIGNORE
  if( nignore.isSet() )
    setup.matches.nignore += split_simple( *nignore.getValues()->begin() );
#endif


#ifdef HAVE_FNMATCH_H
  if( copy.isSet() )
    setup.matches.copy += split_simple( *copy.getValues()->begin() );
#endif

#ifdef HAVE_REGEX_H
  if( copy_regex.isSet() )
    setup.matches.copy_regex += split_simple( *copy_regex.getValues()->begin() );
#endif

  if( copy_file_in_dir.isSet() )
      setup.matches.copy_file_in_dir += split_simple( *copy_file_in_dir.getValues()->begin() );

  if( ignore_file_in_dir.isSet() )
      setup.matches.ignore_file_in_dir += split_simple( *ignore_file_in_dir.getValues()->begin() );

  if( stow_dirs.isSet() )
    setup.stow_dirs.dirs += CppDir::get_full_path( *stow_dirs.getValues()->begin() );

#ifdef HAVE_FNMATCH_H
  if( stow_dirs_add.isSet() )
    setup.stow_dirs.auto_add_dirs += split_simple( *stow_dirs_add.getValues()->begin() );
#endif

#ifdef HAVE_REGEX_H
  if( stow_dirs_add_regex.isSet() )
    setup.stow_dirs.auto_add_dirs_regex += split_simple( *stow_dirs_add_regex.getValues()->begin() );
#endif



  if( protect_dirs.isSet() )
    setup.protect_dirs.dirs += CppDir::get_full_path( *protect_dirs.getValues()->begin() );

#ifdef HAVE_FNMATCH_H
  if( protect_dirs_add.isSet() )
    setup.protect_dirs.auto_add_dirs += split_simple( *protect_dirs_add.getValues()->begin() );
#endif

#ifdef HAVE_REGEX_H
  if( protect_dirs_add_regex.isSet() )
    setup.protect_dirs.auto_add_dirs_regex += split_simple( *protect_dirs_add_regex.getValues()->begin() );
#endif

  if( protect_dirs_target.isSet() )
    setup.protect_dirs.targets += split_simple( *protect_dirs_target.getValues()->begin() );

  setup.protect_dirs.targets_add_traversable( protect_dirs_targets_add_traversable.isSet() );



  if( keep_dirs.isSet() )
    setup.keep_dirs.dirs += split_simple( *keep_dirs.getValues()->begin() );  

#ifdef CAN_USE_INI

  // nothing would happen if we call these functions, but it simple isn't required

  setup.config_files.files += CppDir::concat_dir( SYSCONFDIR, "xstow.ini" );
  setup.config_files.files += CppDir::concat_dir( setup.working_dir(), "xstow.ini" );

  if( files.isSet() )
      setup.config_files.files += CppDir::get_full_path( *files.getValues()->begin() );

  setup.read_ini();

#endif
  
  if( setup.conflicts() )
    setup.no_changes( true );

  if( setup.simulate() )
    setup.verbose_level( 1 );

  out.set_module( setup.debug.module() );
  out.set_debug_level( setup.debug.level() );  

  vout.set_debug_level( setup.verbose_level() );

  OUT(1) << "Setup:\n" << setup << std::endl;


  // setup is done, start with the real work

  try
    {
      for( unsigned int i = 0; i < setup.packages().size(); ++i )
	{
	  setup.current_package( setup.packages[i] );

	  bool again = false;

	  if( setup.restow() )
	    {
	      again = true;
	      setup.unstow( true );
	    }
	  
	START:

	  {
	    
	    Tree tree( setup.packages[i], setup.target_dir(), setup.working_dir() );
	    
	    if( tree.scan() )
	    {     
		if( out.check( MODULE::MAIN, 1 ) )
		    out << tree.getTree() << '\n' << std::endl;
		
		if( tree.check() )
		{
		    if( !tree.exec_list() )
			return 3;
		}
		else
		    return 4;
	    }
	    else
		return 4;
	  }

	  if( again )
	    {
	      again = false;
	      setup.unstow( false );
	      goto START;
	    }
	}

    } 
  catch( std::exception &err )
    {
	ERROR( "Error: %s\n", err.what() ); 
#ifndef NDEBUG
	ERROR( "Backtrace: %s\n", BackTraceHelper::bt.bt() );
#endif
	return 2;
    }
#ifndef NFORMAT
  catch( Format::Error &err )
    {
	ERROR( "Format::Error: %s\n", err.err ); 
	return 2;
    }
#endif
  catch( ... )
    {
      ERROR << "Error: unknown Exception caught\n";
      return 2;
    }

 return 0;
}
