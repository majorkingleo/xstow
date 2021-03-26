#ifndef TOOLS_RANGE_H
#define TOOLS_RANGE_H

#include <exception>

namespace Tools {

  template<class EClass> class CopyNone
	{
	public:
	  void operator()( EClass & dest, const EClass & source )
	  {}
	};

  template<class EClass, class Copy=CopyNone<EClass> > class EnumRange : public EClass
{    
public:
    class Error : public std::exception {
	  const char *err;
	  public:

	  Error( const Error & e ) throw() : exception(), err( e.err ) {}
	  Error & operator=( const Error & e ) throw() { err = e.err; return *this; }
	  Error( const char *e ) throw() : exception(), err(e) {}

	  virtual ~Error() throw() {}

	  virtual const char * what() const throw() { return err; }	  
    };

    typedef typename EClass::ETYPE ETYPE;       

    static const ETYPE INVALID__ = static_cast<ETYPE>(-5000);
    ETYPE value;

    EnumRange( ETYPE value_ ) 
	: EClass(),
	  value( value_ )
	{
	    if( value <= EClass::FIRST__ || value >= EClass::LAST__ )
		throw( Error( "EnumRange: Out of range!" ) );
	}    

    EnumRange(  const EnumRange<EClass>& er ) 
	: EClass(),
	  value( er.value )
	{
	    if( value <= EClass::FIRST__ || value >= EClass::LAST__ )
		throw( Error( "EnumRange: Out of range!" ) );

		Copy cp;

		cp(*this,er);
	}    

    EnumRange() : EClass(), value( INVALID__ ) {}

    ETYPE operator()() const {
	if( value == INVALID__ )
	    throw( Error( "EnumRange: Uninitialized value!" ) );

	return value;
    }

    operator ETYPE () const {
	if( value == INVALID__ )
	    throw( Error( "EnumRange: Uninitialized value!" ) );

	return value;
    }

    EnumRange<EClass,Copy>& operator=( ETYPE v ) {
	if( v <= EClass::FIRST__ || v >= EClass::LAST__ )
	    throw( Error( "EnumRange: Out of range!" ) );
	value = v;
	return *this;
    }

    EnumRange<EClass,Copy>& operator=( const EnumRange<EClass>& er ) {
	value = er.value;
	Copy cp;

	cp(*this,er);

	return *this;
    }
	
};

} // namespace Tools

#endif
