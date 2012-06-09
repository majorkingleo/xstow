/*
 * $Log: range.h,v $
 * Revision 1.3  2005/07/04 21:59:42  martin
 * added logging to all files
 *
 */
#ifndef RANGE_H
#define RANGE_H

#include <exception>

// #define CORE_DUMP

template<class EClass> class EnumRange : public EClass
{    
public:
    struct Error : public std::exception {
	const char *err;
	Error( const char *err ) throw() : exception(), err(err) {}
	virtual const char * what() const throw() { return err; }
    };

    typedef typename EClass::ETYPE ETYPE;       

    static const ETYPE INVALID__ = static_cast<ETYPE>(-5000);
    ETYPE value;

    EnumRange( ETYPE value ) 
	: value( value )
	{
	    if( value <= EClass::FIRST__ || value >= EClass::LAST__ )
#ifdef CORE_DUMP
		int i = 1 / 0;
#else
		throw( Error( "EnumRange: Out of range!" ) );
#endif
	}    

    EnumRange() : value( INVALID__ ) {}

    ETYPE operator()() const {
	if( value == INVALID__ )
#ifdef CORE_DUMP
		int i = 1 / 0;
#else
	    throw( Error( "EnumRange: Uninitialized value!" ) );
#endif

	return value;
    }

    operator ETYPE () const {
	if( value == INVALID__ )
#ifdef CORE_DUMP
		int i = 1 / 0;
#else
	    throw( Error( "EnumRange: Uninitialized value!" ) );
#endif

	return value;
    }

    EnumRange<EClass>& operator=( ETYPE v ) {
	if( v <= EClass::FIRST__ || v >= EClass::LAST__ )
#ifdef CORE_DUMP
		int i = 1 / 0;
#else
	    throw( Error( "EnumRange: Out of range!" ) );
#endif
	    value = v;

	return *this;
    }

    EnumRange<EClass>& operator=( const EnumRange<EClass>& er ) {
	value = er.value;
	return *this;
    }
	
};


#endif
