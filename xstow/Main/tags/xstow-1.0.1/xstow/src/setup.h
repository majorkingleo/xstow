/*
 * $Log: setup.h,v $
 * Revision 1.2  2005/07/04 21:59:42  martin
 * added logging to all files
 *
 */
#ifndef setup_h
#define setup_h

#include "setup_config.h"
#include "ref.h"

#ifdef CAN_USE_NIGNORE
class NIgnoreChain;
#endif

struct Setup
{
  struct Debug : public Section
  {
    struct WrapFreeze
    {
      struct Freeze
      {
	bool freeze;
	
	void operator()( bool f ) { freeze = f; }
	bool operator()() { return freeze; }
	
	Freeze() : freeze( false ) {}
      } freeze;
    };

    struct DIntValue : public IntValue, public WrapFreeze
    {
      DIntValue( const std::string &s, const std::string &k ) : IntValue( s, k ), WrapFreeze() {}

      ON_INI( void read_ini( Leo::Ini &ini ) { if( !freeze() ) IntValue::read_ini( ini ); } )
    } level;

    struct MValue : public Value<DModule>, public WrapFreeze
    {      
      MValue( const std::string &s, const std::string &k ) : Value<DModule>( s, k ), WrapFreeze() {}
      DModule string2data( const std::string &s ) { return string2module( s ); }

      ON_INI( void read_ini( Leo::Ini &ini ) { if( !freeze() ) Value<DModule>::read_ini( ini ); } )
    };

    MValue module;

    Debug( const std::string &section );

  } debug;

  struct TraversableLinks : public Section
  {
    VecDirChecker checker;

    BoolValue           keep_targets;
    VecStringValue      links;
    VecStringValue      add_if_targets;
    RegExVecStringValue add_if_targets_regex;

    TraversableLinks( const std::string &s );

  } traversable_links;

  struct KeepDirs : public Section
  {    
    VecStringValue dirs;

    KeepDirs( const std::string &s );

  } keep_dirs;

  struct Matches : public Section
  {
    VecStringValue      ignore;
    RegExVecStringValue ignore_regex;
    VecStringValue      copy; 
    RegExVecStringValue copy_regex;   
    VecStringValue      nignore;
    VecStringValue      ignore_file_in_dir;
    VecStringValue      copy_file_in_dir; 

    Matches( const std::string &s );

  } matches;

  struct StowDirs : public Section
  {
    VecDirChecker checker;

    VecStringValue      dirs;
    VecStringValue      auto_add_dirs;
    RegExVecStringValue auto_add_dirs_regex;

    StowDirs( const std::string &s );

  } stow_dirs;

  struct ProtectDirs : public Section
  {
    VecDirChecker checker;

    VecStringValue      dirs;
    VecStringValue      auto_add_dirs;
    RegExVecStringValue auto_add_dirs_regex;
    VecStringValue      targets;
    BoolValue           targets_add_traversable;

    ProtectDirs( const std::string &s );

  } protect_dirs;

  struct ConfigFiles : public Section
  {
    VecStringValue files;
    BoolValue      in_home;
    BoolValue      in_stow_dir;
    BoolValue      in_other_stow_dirs;    

    ConfigFiles( const std::string &s );

  } config_files;

  struct Links : public Section
  {
    BoolValue          absolute_paths;

    Links( const std::string &s );
  } links;

#ifdef CAN_USE_INI
  struct Exec : public Section
  {
    Value<std::string>  match;
    Value<std::string>  exec;     
    Value<std::string>  exec_unstow;

    Exec();
  };

  std::vector<Exec> exec;
#endif

#ifdef CAN_USE_NIGNORE
    struct IniNIgnore : public Section // for syntax check only
    {
	Value<std::string> dir;
	BoolValue          follow;

	IniNIgnore( const std::string &section );
    };
#endif

  BoolValue no_changes;
  BoolValue conflicts;
  Value<std::string> working_dir;
  Value<std::string> target_dir;
  IntValue           verbose_level;
  VecStringValue     packages;
  BoolValue          unstow;
  BoolValue          restow;
  BoolValue          simulate;
  BoolValue          force;
  BoolValue          use_curses;
  Value<std::string> current_package;

#ifdef CAN_USE_NIGNORE
  std::vector< Ref<NIgnoreChain> > nignores;
#endif

  void read_ini();

  Setup();

  void handle_keep_dirs();
  void handle_protect_dirs();

private:

  std::vector<Section*> sections;
  
  void add( Section* s );

  void handle_config_files();

#ifdef CAN_USE_NIGNORE
  void read_nignore( Leo::Ini &ini );
#endif

#ifdef CAN_USE_INI
  void read_exec( Leo::Ini &ini );

  void check_ini( Leo::Ini &ini );
#endif

public:
  
  friend std::ostream& operator<<( std::ostream& out, const Setup &setup );
  
};

extern Setup setup;

std::ostream& operator<<( std::ostream& out, const Setup &setup );
std::ostream& operator<<( std::ostream& out, const vec_string &v );

#endif
