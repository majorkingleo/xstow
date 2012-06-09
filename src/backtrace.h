/*
 * $Log: backtrace.h,v $
 * Revision 1.2  2005/07/04 21:59:42  martin
 * added logging to all files
 *
 */
#ifndef BACKTRACE_H
#define BACKTRACE_H

#include <list>
#include <string>
#include <ostream>
#include "format.h"

class BackTrace
{
private:
    struct Ticket
    {
	int ticket;
	std::string message;
	std::string info;

	Ticket( int t, const std::string &m ) : ticket( t ), message( m ) {}
    };

    std::list<Ticket> stack;
    typedef std::list<Ticket>::iterator it_t;
    typedef std::list<Ticket>::reverse_iterator rit_t;
    int count;
    
public:
    BackTrace() : count( 0 ) {}


    int push( const std::string & message )
	{
	    count++;
	    stack.push_back( Ticket( count, message ) );
	    return count;
	}
    
    void pop( int ticket )
	{
	    if( stack.empty() )
		return;

	    it_t it = stack.end();
	    it--;

	    while( it != stack.end() )
	    {
		if( it->ticket == ticket )
		{
		    stack.erase( it );
		    break;
		} else {		   
		    stack.erase( it );
		    it = stack.end();

		    if( stack.empty() )
			return;

		    it--;
		}
	    }
	}

    void bt( std::ostream &out )
	{
	    for( it_t it = stack.begin(); it != stack.end(); it++ )
	    {
		out << it->ticket << ':' << it->message << ": " << it->info << '\n';
	    }
	}

    std::string bt()
	{
	    std::stringstream out;
	    bt( out );
	    return out.str();
	}


    void add_info( int ticket, const std::string &info )
	{
	    if( stack.empty() )
		return;

	    for( rit_t it = stack.rbegin(); it != stack.rend(); it++ )
	    {
		if( it->ticket == ticket )
		{
		    it->info += info;
		    return;
		}
	    }
	}
};

class BackTraceHelper
{
private:
    int ticket;

public:
    static BackTrace bt;

public:
    BackTraceHelper( const std::string &func_name )
	{
	    ticket = bt.push( func_name );
	}

    ~BackTraceHelper()
	{
	    bt.pop( ticket );
	}

    void add_info( const std::string &info )
	{
	    bt.add_info( ticket, info );
	}
};

#ifndef NDEBUG
#define BT \
  BackTraceHelper bt( format( "%s:%d:%s", __FILE__, __LINE__, __PRETTY_FUNCTION__ ) )
#else
#define BT
#endif
#endif
