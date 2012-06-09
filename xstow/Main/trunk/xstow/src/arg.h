/*
 * $Log: arg.h,v $
 * Revision 1.3  2010/07/21 19:38:25  martin
 * gcc-4 Port
 *
 * Revision 1.2  2005/07/04 21:59:42  martin
 * added logging to all files
 *
 */
#ifndef arg_h
#define arg_h

/*
  Experimental Version of LeoArg 2
*/

#include <string>
#include <vector>
#include <list>

#include "ref.h"
#include "range.h"

namespace Arg {
  
  class Rule
    {
    protected:
      bool continue_on_match;
      bool continue_on_fail;

      std::string description;
      std::string error;

    public:
      
      Rule( bool com = true, bool cof = true );
      virtual ~Rule();

      virtual bool match( const std::string &value ) const = 0;
      virtual bool continueOnMatch() const;
      virtual bool continueOnFail() const;
      virtual bool matchEmpty() const;

      virtual void setDescription( const std::string &des );
      virtual std::string getDescription() const;
      virtual bool hasDescription() const;

      virtual std::string getError() const;
    };

  typedef std::vector<std::string> vec_string;

  class OptionNameRule : public Rule
    {
    protected:
      mutable Ref<vec_string> prefixes;
      mutable Ref<vec_string> names;

    public:
      OptionNameRule();
      OptionNameRule( Ref<vec_string> prefixes, Ref<vec_string> names );
      
      void setPrefixes( Ref<vec_string> p = Ref<vec_string>()) { prefixes = p; }
      void setNames( Ref<vec_string> n = Ref<vec_string>() ) { names = n; }

      Ref<vec_string> getNames() const { return names; }
      Ref<vec_string> getPrefixes() const { return prefixes; }

      bool match( const std::string &option ) const;

      std::string getDescription( unsigned int s1 = 5, unsigned int s2 = 40) const;
      bool hasDescription() const { return !names->empty(); }
    };

  template<class R, class S> bool convert( Ref<R> r, Ref<S> &s )
    {
      if( !r )
	return false;

      S *ss = dynamic_cast<S*>( r.address() );

      if( ss )
	{
	  s = Ref<S>(ss, false);
	  return true;
	}

      return false;
    }

  template<class S, class R> Ref<S> convert( Ref<R> r )
    {
      Ref<S> s;

      convert( r, s );
      
      return s;
    }

  class IsOptionRule : public Rule
    {
    protected:
      mutable Ref<vec_string> prefixes;

    public:
      IsOptionRule() : prefixes( new vec_string() ) {}
      IsOptionRule( Ref<vec_string> prefixes );

      void setPrefixes( Ref<vec_string> p = Ref<vec_string>() ) { prefixes = p; }
      Ref<vec_string> getPrefixes() const { return prefixes; }

      bool match( const std::string &option ) const;
    };
  
  class IsValueRule : public Rule
    {
    protected:
      mutable Ref<vec_string> states;
      bool cs;

    public:
      IsValueRule( bool cs = false );
      IsValueRule( Ref<vec_string> states, bool case_sensitiv = false);

      void setStates( Ref<vec_string> s ) { states = s; }
      Ref<vec_string> getStates() const { return states; }      

      bool match( const std::string &value ) const;

      std::string getDescription() const;
      bool hasDescription() const { return !states->empty(); }
    };

  class IsBoolRule : public IsValueRule
    {
    public:
      IsBoolRule();

      bool match( const std::string &value ) const;
      bool matchEmpty() const;
    };

  class AnyStringRule : public Rule
    {
    public:
      AnyStringRule() : Rule() {}

      bool match( const std::string &value ) const { return true; }
    };

  class IsIntRule : public Rule
    {
    public:
      IsIntRule();

      bool match( const std::string &value ) const;
    };

  class Option
    {
    public:

      static const unsigned int infinite = static_cast<unsigned int>( -1 );

    protected:
      
      mutable Ref< Rule > name_rule;  // check if option name is valid
      mutable Ref< Rule > value_rule; // check if the value is valid

      mutable Ref< vec_string > values;

      unsigned int min_values;
      unsigned int max_values;

      bool isset;
      bool required;

      std::string description;

      unsigned int option_id;

    public:
      Option();
      Option( Ref< Rule > name_rule, Ref< Rule > value_rule );
      virtual ~Option();

      void setNameRule( Ref< Rule > nr ) { name_rule = nr; }
      void setValueRule( Ref< Rule > vr ) { value_rule = vr; }

      Ref< Rule > getNameRule() const { return name_rule; }
      Ref< Rule > getValueRule() const { return value_rule; }

      unsigned int getMinValues() const { return min_values; }
      unsigned int getMaxValues() const { return max_values; }

      void setMinValues( unsigned int min ) { min_values = min; }
      void setMaxValues( unsigned int max ) { max_values = max; }

      bool matchName( const std::string &name ) const;
      bool matchValue( const std::string &value ) const;

      void addValue( const std::string &value ) { values->push_back( value ); }
      
      bool addName( const std::string &name );

      bool isSet() const { return isset; }
      void setSet( bool s ) { isset = s; }

      bool isRequired() const { return required; }
      void setRequired( bool r ) { required = r; }

      Ref< vec_string > getValues() const { return values; }

      void setDescription( const std::string &des ) { description = des; }
      virtual std::string getDescription( unsigned int s1 = 5, 
					  unsigned int s2 = 20, 
					  unsigned int s3 = 40, 
					  unsigned int console_width = 80 ) const;
      virtual bool hasDescription() const;

      unsigned int getId() const { return option_id; }
      void setId( unsigned int id ) { option_id = id; }

    };

  class BoolOption : public Option
    {
    public:
      BoolOption();
      BoolOption( const std::string &name );
      BoolOption( Ref< vec_string > names );

      bool getState() const;
    };

  class StringOption : public Option
    {
    public:
      StringOption();
      StringOption( const std::string &name, const std::string &value_description = "" );
      
      void setValueDescription( const std::string &description );
    };

  class EnumOption : public Option
    {
    public:
      explicit EnumOption( bool case_sensitiv = false );
      explicit EnumOption( const std::string &name, bool case_sensitiv = false );
      explicit EnumOption( const char *name, bool case_sensitiv = false );	

      void addEnum( const std::string &name );      
    };

  class EmptyOptionNameRule : public OptionNameRule
    {
    public:
      EmptyOptionNameRule();

      bool match( const std::string &option ) const;

      bool hasDescription() const { return false; }
    };

  class EmptyStringOption : public StringOption
    {
    public:
      EmptyStringOption();

      bool hasDescription() const { return false; }
    };

  class FlagOption : public BoolOption
    {
    public:
      FlagOption() : BoolOption() { setMaxValues(0); }
      FlagOption( const std::string &name ) 
	: BoolOption( name ) 
	{ 
	  setMaxValues(0); 
	}

      virtual std::string getDescription( unsigned int s1 = 5, 
					  unsigned int s2 = 20, 
					  unsigned int s3 = 40,
					  unsigned int console_width = 80 ) const;
    };

  class IntOption : public Option
    {
    public:
      IntOption();
      IntOption( const std::string &name );

    };

  struct EnumLink
  {
	enum ETYPE {
	  FIRST__,
	  AND,
	  OR,
	  LAST__
	};
  };
  
  typedef EnumRange<EnumLink> LINK;

  struct SOption
  {
    std::string option;
    vec_string  values;
  };

  typedef std::list<SOption> list_soption;

  class Arg;

  class OptionChain
    {
    public:

      typedef std::vector< Ref<Option> > rvec_option;

      struct Error
      {
	std::string err;
	std::string value;
	Ref<Option> option;	

	Error( Ref<Option> option, const std::string err, const std::string value )
	  : err( err ), value( value ), option( option )
	{}
      };

      typedef std::vector<Error> vec_err;

    protected:

      Ref<rvec_option> options;
      LINK mode;
      Ref<vec_string> prefixes;
      unsigned int min_match;

      bool continue_on_fail;
      bool continue_on_match;

      Arg *arg;

      vec_err err_options;

    public:
      OptionChain( LINK m = LINK::OR );

      void addOption( Ref<Option> option );

      template<class X> void addOption( Ref<X> option )
	{
	  addOption( Ref<Option>( *option, false ) );
	}

      template<class X> void addOptionR( X* option )
	{
	  addOption( Ref<Option>( *option, false ) );
	}
      
      Ref<vec_string> getPrefixes() const { return prefixes; }
      void setPrefixes( Ref<vec_string> p ) { prefixes = p; }

      unsigned int getMinMatch() const { return min_match; }
      void setMinMatch( unsigned int val ) { min_match = val; }

      bool parse( list_soption &arg );

      bool getContinueOnFail() const { return continue_on_fail; }
      void setContinueOnFail( bool cof ) { continue_on_fail = cof; }

      bool getContinueOnMatch() const { return continue_on_match; }
      void setContinueOnMatch( bool com ) { continue_on_match = com; }

      void setArg( Arg *arg_ ) { arg = arg_; }

      vec_err getErrOptions() const { return err_options; }
    };

  class Arg
    {
    public:

    protected:
      
      std::vector<Ref<OptionChain> > chains;
      mutable std::vector<Ref<Option> > options; // for help page

      int argc;
      char **argv;

      Ref<Rule> is_option;
      Ref<vec_string> prefixes;

      unsigned int min_match;

      unsigned int option_id;

    public:

      Arg( int argc, char **argv );

      void addPrefix( const std::string &s );

      void addChain( Ref<OptionChain> chain );

      template<class X> void addChainR( X* chain )
	{
	  addChain( Ref<OptionChain>( *chain, false ) );
	}

      void addOption( Ref<Option> option );

      template<class X> void addOption( Ref<X> option )
	{
	  addOption( Ref<Option>(  *option, false ) );
	}

      template<class X> void addOptionR( X* option )
	{
	  addOption( Ref<Option>( *option, false ) );
	}

      bool parse();

      void setMinMatch( unsigned int mm  ) { min_match = mm; }

      void addHelp( Ref<Option> option );

      template<class X> void addHelpR( X* help )
	{ 
	  addHelp( Ref<Option>( *help, false ) );
	}

      std::string getHelp( unsigned int s1 = 5, 
			   unsigned int s2 = 20, 
			   unsigned int s3 = 40, unsigned int console_width = 80 ) const;

      unsigned int getOptionId() { return option_id++; }

    private:

      list_soption parse_args();

    };

  inline bool operator==( const Option &a, const Option &b );

  std::ostream& operator<<( std::ostream& out, OptionChain::vec_err & err );


  int get_terminal_width(); 

} // namespace Arg


std::ostream& operator<<( std::ostream& out, Ref<Arg::vec_string> values );


#endif
