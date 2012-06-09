/*
 * $Log: arg.cpp,v $
 * Revision 1.3  2005/07/04 21:59:42  martin
 * added logging to all files
 *
 */
#include "arg.h"
#include "string_utils.h"
#include "debug.h"


#define OUT(level) DEBUG_OUT( level, MODULE::ARG )

#include <iostream>

#ifdef HAVE_CURSES_H
extern "C" {
#include <curses.h>
}
#endif

// yes it's a little bit senseles but very nice
int Arg::get_terminal_width()
{      
    const int DEFAULT_COLS = 80;
    
#ifdef HAVE_CURSES_H
    
    const char *tm  = getenv( "TERM" );
    
    if( !tm || std::string( tm ).empty() )
	return DEFAULT_COLS;
    
    initscr();
    endwin();
    return COLS;
    
#else
    
    return DEFAULT_COLS;
#endif
}


std::ostream& operator<<( std::ostream& out, Arg::list_soption arg )
{
  for( Arg::list_soption::iterator it = arg.begin(); it != arg.end(); ++it )
    {
      out << '[' << it->option << "]\n";
      for( unsigned int i = 0; i < it->values.size(); ++i )
	out << '\t' << it->values[i] << '\n';
    }

  return out;
}

Arg::Rule::Rule( bool com, bool cof )
{
  continue_on_match = com;
  continue_on_fail = cof;
}

Arg::Rule::~Rule()
{

}


bool Arg::Rule::continueOnMatch() const
{
  return continue_on_match;
}

bool Arg::Rule::continueOnFail() const
{
  return continue_on_fail;
}

bool Arg::Rule::matchEmpty() const
{
  return false;
}

void Arg::Rule::setDescription( const std::string &des )
{
  description = des;
}

std::string Arg::Rule::getDescription() const
{
  return description;
}

bool Arg::Rule::hasDescription() const
{
  return !description.empty();
}

std::string Arg::Rule::getError() const
{
  return error;
}

Arg::OptionNameRule::OptionNameRule()
  : Rule( true, true )
{
  names = new vec_string();
}

Arg::OptionNameRule::OptionNameRule( Ref<vec_string> prefixes, Ref<vec_string> names )
  : Rule( true, true ),
    prefixes( prefixes ), names( names )
{

}


bool Arg::OptionNameRule::match( const std::string &option ) const
{
  if( prefixes.valid() &&  names.valid() )
    {      
      for( unsigned int i = 0; i < prefixes->size(); ++i )
	for( unsigned int j = 0; j < names->size(); ++j )
	  if( option == (*prefixes)[i] + (*names)[j] )
	    {

	      DEBUG( OUT(2)( "matching option name %s\n", option ) );
	      
	      return true;
	    }
    } 
  else if( names.valid() )
    {
      for( unsigned int j = 0; j < names->size(); ++j )
	if( option == (*names)[j] )
	  {
	    //	    std::cout << "matching option name: " << option << std::endl;
	    
	    return true;
	  }
    }
  else
    {
      // no name, no prefix... simple say ok here if option is empty

      if( option.empty() )
	return true;
    }

  return false;
}

std::string Arg::OptionNameRule::getDescription( unsigned int s1, unsigned int s2 ) const
{
  std::string s;
  std::string p;

  bool start = true;

  /*
  for( vec_string::iterator it = prefixes->begin(); it != prefixes->end(); ++it )
    {
      if( !start )
	p += '|';

      start = false;

      p += *it;
    }

  if( !p.empty() )
    p = '[' + p + ']';

  */

  if( prefixes->begin() != prefixes->end() )
    p = *(prefixes->begin());

  start = true;

  for( vec_string::iterator it = names->begin(); it != names->end(); ++it )
    {
      if( !start )
	{
	  for( unsigned int i = s.size(); i < s1; ++i )
	    s += ' ';
	}

      s += p + *it;

      start = false;
    }

  for( unsigned int i = s.size(); i < s2; ++i )
    s += ' ';

  return s;
}


Arg::IsOptionRule::IsOptionRule( Ref<vec_string> prefixes )
  : Rule( true, true ),
    prefixes( prefixes )
{
  
}

bool Arg::IsOptionRule::match( const std::string &option ) const
{
  for( unsigned int i = 0; i < prefixes->size(); ++i )
    if( option.find( (*prefixes)[i] ) == 0 )
      return true;

  return false;
}

Arg::IsValueRule::IsValueRule( bool cs )
  : Rule( true, false ),
    cs(cs)
{
  states = new vec_string();
  error = "value does not matches one of the valid states";
}

Arg::IsValueRule::IsValueRule( Ref<vec_string> states, bool cs )
  : Rule( true, false ),
    states( states ),
    cs( cs )
{
  error = "value does not matches one of the valid states";
}

bool Arg::IsValueRule::match( const std::string &value ) const
{
  std::string v( value );

  if( !cs )
    v = toupper(v);

  v = strip( v );

  for( unsigned int i = 0; i < states->size(); ++i )
    if( v == (*states)[i] )
      return true;

  return false;
}

std::string Arg::IsValueRule::getDescription() const
{
  std::string s;

  bool start = true;

  for( vec_string::iterator it = states->begin(); it != states->end(); ++it )
    {
      if( !start )
	s += '|';

      s += *it;

      start = false;
    }

  if( !s.empty() )
    s =  '[' + s + ']' ;

  return s;
}

Arg::IsBoolRule::IsBoolRule()
  : IsValueRule()
{
  states->push_back( "TRUE" );
  states->push_back( "FALSE" );
  states->push_back( "0" );
  states->push_back( "1" );
}
  

bool Arg::IsBoolRule::match( const std::string &value ) const
{
  std::string v( value );

  v = toupper( v );

  return IsValueRule::match( v );
}

bool Arg::IsBoolRule::matchEmpty() const
{
  return true;
}

Arg::Option::Option()
{
  isset = false;
  min_values = 0;
  max_values = infinite;
  required = false;

  values = new vec_string;

  option_id = infinite;
}

Arg::Option::Option( Ref< Rule > name_rule, Ref< Rule > value_rule )
  : name_rule( name_rule ), value_rule( value_rule )
{
  isset = false;
  min_values = 0;
  max_values = infinite;
  required = false;

  values = new vec_string;

  option_id = infinite;
}

bool Arg::Option::matchName( const std::string &name ) const
{
  return name_rule->match( name );
}

bool Arg::Option::matchValue( const std::string &value ) const
{
  return value_rule->match( value );
}

bool Arg::Option::addName( const std::string &name ) 
{
  if( !name_rule )
    return false;

  Ref<OptionNameRule> n;

  if( convert( name_rule, n ) )
    {
      n->getNames()->push_back( name );
      return true;
    }

  return false;
}

bool Arg::Option::hasDescription() const
{
  if( name_rule.valid() )
    if( name_rule->hasDescription() )
      return true;

  if( value_rule.valid() )
    if( value_rule->hasDescription() )
      return true;

  if( !description.empty() )
    return true;

  return false;
}

std::string Arg::Option::getDescription( unsigned int s1, unsigned int s2, unsigned int s3,
					 unsigned int console_width ) const
{
  std::string s;

  if( name_rule->hasDescription() )
    {
      // try converting into a name rule
      Ref<OptionNameRule> name;

      if( convert( name_rule, name ) )
	s += name->getDescription( s1, s2 );
      else
	s += name_rule->getDescription();
    }

  if( value_rule->hasDescription() )
    {
      s += value_rule->getDescription();
    }

  for( unsigned int i = s.size(); i < s3; ++i )
    s += ' ';
  
  if( s.size() > s3 )
    s += ' ';

  if( !description.empty() )
    s += description;

  // trim to 80 characters per line
  if( s.size() > console_width )
    {
      s = text_right_format( s, console_width, s3 );
    }


  return s;
}

Arg::BoolOption::BoolOption()
  : Option( new OptionNameRule(), new IsBoolRule() )
{
  setMaxValues( 1 );
  setMinValues( 0 );
}

Arg::BoolOption::BoolOption( const std::string &name )
  : Option( new OptionNameRule(), new IsBoolRule() )
{
  addName( name );

  setMaxValues( 1 );
  setMinValues( 0 );
}

Arg::BoolOption::BoolOption( Ref< vec_string > names )
  : Option()
{
  name_rule = new OptionNameRule();

  convert<OptionNameRule>(name_rule)->setNames( names );

  value_rule = new IsBoolRule();

  setMaxValues( 1 );
  setMinValues( 0 );
}

bool Arg::BoolOption::getState() const
{
  if( values->size() == 0 )
    return isSet();

  if( toupper( (*values)[0] ) == "TRUE" || (*values)[0] == "1" )
    return true;

  return false;
}

Arg::StringOption::StringOption()
  : Option()
{
  name_rule = new OptionNameRule();

  value_rule = new AnyStringRule();

  setMinValues(1);
}

Arg::StringOption::StringOption( const std::string &name, const std::string &vd )
  : Option()
{
  name_rule = new OptionNameRule();

  addName( name );

  value_rule = new AnyStringRule();

  setMinValues(1);

  setValueDescription( vd );
}

void Arg::StringOption::setValueDescription( const std::string &des )
{
  if( value_rule.valid() )
    value_rule->setDescription( des );
}

Arg::EnumOption::EnumOption( bool cs )
  : Option( new OptionNameRule(), new IsValueRule( cs ) )
{
  setMinValues(1);
}

Arg::EnumOption::EnumOption( const std::string &name, bool cs )
  : Option( new OptionNameRule(), new IsValueRule( cs ) )
{
  setMinValues(1);

  addName( name );
}

Arg::EnumOption::EnumOption( const char* name, bool cs )
  : Option( new OptionNameRule(), new IsValueRule( cs ) )
{
  setMinValues(1);

  addName( name );
}


void Arg::EnumOption::addEnum( const std::string &name )
{
  convert<IsValueRule>( value_rule )->getStates()->push_back( name );
}

Arg::OptionChain::OptionChain( LINK m )
  : mode( m )
{
  min_match = 0;
  continue_on_fail = false;
  continue_on_match = true;
  options = new rvec_option;  

  arg = 0;
}

bool Arg::OptionChain::parse( list_soption &arg )
{
  DEBUG( OUT(1) << "=====\n" << arg << std::endl );

  err_options.clear();

  unsigned int matches = 0;
  bool not_inc = false;

  for( unsigned int j = 0; j < options->size() && arg.begin() != arg.end(); ++j )
    {
      Ref<Option> o = (*options)[j];
  
      {
	Ref<OptionNameRule> onr;
	
	if( convert( o->getNameRule(), onr ) )
	  {
	    if( onr->getNames().valid() )
	      DEBUG( OUT(1) << "option: " << onr->getNames() << std::endl );
	  }
	
      }
      
      bool found = false;
      bool first = true;
      bool failed = false;

      for( list_soption::iterator it = arg.begin(); it != arg.end(); )
	{
	  DEBUG( OUT(1) << "arg option: " << it->option << std::endl );
	  

	  if( not_inc )
	    not_inc = false;
	  else
	    if( !first )
	      {
		++it;
		if( it == arg.end() )
		  break;
	      }
	  
	  if( first )
	    first = false;

	  if( o->matchName( it->option ) )
	    {
	      DEBUG( OUT(1) << "name matched" << std::endl );

	      matches++;

	      found = true;

	      vec_string v = it->values;

	      o->setSet( true );

	      unsigned int k;

	      std::string failed_option;

	      for( k = 0; k < v.size() && k < o->getMaxValues() ; ++k )
		{
		  DEBUG( OUT(1) << "trying value: " << v[k] << std::endl );

		  if( o->matchValue( v[k] ) )
		    {		    
		      DEBUG( OUT(1) << "matched" << std::endl );

		      o->addValue( v[k] );

		      if( !o->getValueRule()->continueOnMatch() )
			break;
		    }
		  else
		    {
		      DEBUG( OUT(1) << "not matching value: " << v[k] << std::endl );

		      err_options.push_back( Error( o, o->getValueRule()->getError(), v[k] ) );

		      if( !o->getValueRule()->continueOnFail() )
			{
			  if( k < o->getMinValues() )
			    {
			      o->setSet( false );
			      matches--;
			      found = false;
			      failed = true;
			      k = 0;
			      failed_option = it->option;

			      if( o->isRequired() )
				return false;
			    }

			  break;
			}
		    }
		}
	      
	      if( k < v.size() )
		{
		  vec_string pending;

		  for( unsigned int i = k; i < v.size(); ++i )
		    pending.push_back( v[i] );

		  it->values = pending;
		  it->option = failed_option;

		  not_inc = true;

		  //		  std::cout << "ignoring additional values" << std::endl;
		}
	      else 
		{

		  if( k < o->getMinValues() )
		  {
		      ERROR << "missing some values" << std::endl;
		      o->setSet( false );
		  }


		  // erase entry and beware of an invalid iterator

		  list_soption::iterator bit = it;

		  if( bit != arg.begin() )
		    {
		      --bit;		  
		      arg.erase( it );
		    }
		  else
		    {
		      arg.erase( it );
		      bit = arg.begin();
		    }

		  it = bit;

		}

	      if( failed )
		break;

	      if( !o->getNameRule()->continueOnMatch() )
		break;

	    } // if
	  else
	    {
	      if( !o->getNameRule()->continueOnFail() )
		break;
	    }

	} // for

      if( !found && ( mode == LINK::AND ) )
	return false;

    }  

  // check if we missed some required option

  for( unsigned int i = 0; i < options->size(); ++i )
    if( (*options)[i]->isRequired() && !(*options)[i]->isSet() )
      return false;

  if( matches >= min_match )
    return true;

  return false;
}

void Arg::OptionChain::addOption( Ref<Option> option )
{
  if( !convert<OptionNameRule>( option->getNameRule() )->getPrefixes() )
    convert<OptionNameRule>( option->getNameRule() )->setPrefixes( prefixes );

  if( arg )
    {
      if( option->getId() == Option::infinite )
	option->setId( arg->getOptionId() );

      arg->addHelp( option );
    }

  options->push_back( option );
}

Arg::Arg::Arg( int argc, char** argv )
  : argc( argc ),
    argv( argv )
{
  prefixes = new vec_string();

  is_option = new IsOptionRule();

  convert<IsOptionRule>( is_option )->setPrefixes( prefixes );

  min_match = 0;
}

bool Arg::Arg::parse()
{
  list_soption so = parse_args();
  list_soption bso;

  OptionChain::vec_err err_options;

  //  std::cout << "so: ==== \n" << so << std::endl;

  unsigned int matches = 0;

  for( unsigned int i = 0; i < chains.size(); ++i )
    {
      bso = so;

      if( !chains[i]->parse( so ) )
	{
	  OptionChain::vec_err e = chains[i]->getErrOptions();

	  for( unsigned int j = 0; j < e.size(); ++j )
	    err_options.push_back( e[j] );

	  if( !chains[i]->getContinueOnFail() )
	    return false;

	  so = bso;

	} else {

	  matches++;

	  if( !chains[i]->getContinueOnMatch() )
	    return false;

	}
    }

  if( !so.empty() )
    {
      for( list_soption::iterator it = so.begin(); it != so.end(); ++it )
	{
	  ERROR << "invalid option or value: " << it->option;
	  for( unsigned int i = 0; i < it->values.size(); ++i )
	    eout << ' ' << it->values[i];

	  eout << std::endl;

	  if( !err_options.empty() )
	    eout.out << "Errors: \n" << err_options << std::endl;	    
	}

      return false;
    }

  if( matches < min_match )
    return false;

  return true;
}

void Arg::Arg::addPrefix( const std::string &prefix )
{
  prefixes->push_back( prefix );
}

Arg::list_soption Arg::Arg::parse_args()
{
  list_soption so;

  for( int i = 1; i < argc; ++i )
    {
      SOption s;

      bool extra_inc = false;

      if( is_option->match( argv[i] ) )
	{
	  s.option = argv[i];

	  // handle -foo=true

	  std::string::size_type pos;

	  if( ( pos = s.option.find( '=' ) ) != std::string::npos )
	    {
	      s.values.push_back( s.option.substr( pos + 1 ) );
	      s.option = s.option.substr( 0, pos );
	    }

	  ++i;

	  extra_inc = true;
	} 
      
      while( i < argc && !is_option->match( argv[i] ) )
	{
	  s.values.push_back( argv[i] );
	  ++i;

	  extra_inc = true;
	}
      
      so.push_back( s );
      
      if( extra_inc && i < argc && is_option->match( argv[i] ) )
	--i;      
    }

  return so;
}

void Arg::Arg::addOption( Ref<Option> option )
{
  Ref<OptionChain> oc = new OptionChain();

  oc->setPrefixes( prefixes );
  oc->setArg( this );

  oc->addOption( option );

  chains.push_back( oc );
}

void Arg::Arg::addChain( Ref<OptionChain> chain )
{
  if( !chain->getPrefixes() )
    chain->setPrefixes( prefixes );

  chain->setArg( this );

  chains.push_back( chain );
}

Arg::Option::~Option()
{

}

Arg::EmptyOptionNameRule::EmptyOptionNameRule()
  : OptionNameRule()
{
  setPrefixes();
  setNames();
}

bool Arg::EmptyOptionNameRule::match( const std::string &option ) const
{
  if( prefixes.valid() )
    prefixes = Ref<vec_string>();

  if( names.valid() )
    prefixes = Ref<vec_string>();

  return OptionNameRule::match( option );
}

Arg::EmptyStringOption::EmptyStringOption()
  : StringOption()
{
  name_rule = new EmptyOptionNameRule();
}

std::ostream& operator<<( std::ostream& out, Ref<Arg::vec_string> values )
{
  if( !values )
    return out;

  for( unsigned int i = 0; i < values->size(); ++i )
    out << (*values)[i] << ' ';

  return out;
}

Arg::IsIntRule::IsIntRule()
  : Rule( true, false ) 
{ 
  description = "INT"; 
  error = "values is not an integer";
}


bool Arg::IsIntRule::match( const std::string &name ) const
{
  return is_int( name );
}

Arg::IntOption::IntOption()
  : Option( new OptionNameRule(), new IsIntRule() )
{
  setMinValues( 1 );
}

Arg::IntOption::IntOption( const std::string &name )
  : Option( new OptionNameRule(), new IsIntRule() )
{
  setMinValues( 1 );

  addName( name );
}

void Arg::Arg::addHelp( Ref<Option> option )
{
  bool found = false;

  for( unsigned int i = 0; i < options.size(); ++i )
    if( option == options[i] )
      {
	found = true;
	break;
      }

  if( !found )
    options.push_back( option );
}

std::string Arg::Arg::getHelp( unsigned int s1, unsigned int s2, unsigned int s3, unsigned int console_width ) const
{
  std::string help;

  bool first = true;

  for( unsigned int i = 0; i < options.size(); ++i )
    {
      if( options[i]->hasDescription() )	
	{
	  if( !first )
	    help += '\n';
	  
	  help += options[i]->getDescription(s1, s2, s3, console_width );
	}

      first = false;
    }

  return help;
}

bool Arg::operator==( const Option &a, const Option &b )
{
  return a.getId() == b.getId();
}

std::ostream& Arg::operator<<( std::ostream& out, OptionChain::vec_err & err )
{
  bool first = true;

  for( unsigned int i = 0; i < err.size(); ++i )
    {
      if( !first )
	out << '\n';

      first = false;

      Ref<OptionNameRule> name;

      if( convert( err[i].option->getNameRule(), name ) )
	{
	  if( name->hasDescription() )
	    out << name->getDescription() << " : ";
	}

      out << err[i].err << " : " << err[i].value;
    }
  
  return out;
}

std::string Arg::FlagOption::getDescription( unsigned int s1, 
					     unsigned int s2, 
					     unsigned int s3,
					     unsigned int console_width ) const
{
  std::string s;

  if( name_rule->hasDescription() )
    {
      // try converting into a name rule
      Ref<OptionNameRule> name;

      if( convert( name_rule, name ) )
	s += name->getDescription( s1, s2 );
      else
	s += name_rule->getDescription();
    }

  for( unsigned int i = s.size(); i < s3; ++i )
    s += ' ';

  if( s.size() > s3 )
    s += ' ';

  if( !description.empty() )
    s += description;

  if( s.size() > console_width )
    {
      s = text_right_format( s, console_width, s3 );
    }

  return s;
}
