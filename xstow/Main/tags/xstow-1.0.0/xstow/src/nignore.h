/*
 * $Log: nignore.h,v $
 * Revision 1.3  2010/07/21 19:38:25  martin
 * gcc-4 Port
 *
 * Revision 1.2  2005/07/04 21:59:42  martin
 * added logging to all files
 *
 */
#ifndef NIGNORE_H
#define NIGNORE_H

#include "local_config.h"

#ifdef CAN_USE_NIGNORE

#include <string>
#include <vector>
#include "ref.h"
#include "leoini.h"
#include "range.h"
#include <iostream>

class Node;

class NIgnore
{
 public:
    struct EnumMatch
    {
	  enum ETYPE
		{
		  FIRST__ = -1,
		  FALSE = 0,
		  TRUE,
		  REQUIRE_NEXT,
		  LAST__
		};
    };

    typedef EnumRange<EnumMatch> MATCH;

 private:
    std::string name;

    std::vector<std::string> dir;
    bool                     follow;


    bool is_valid;

 public:
    NIgnore( const std::string &name, Leo::Ini &ini );

    MATCH match( Ref<Node> node, std::string & strip );

    bool valid() const { return is_valid; }

    std::string getName() const { return name; }

    friend std::ostream & operator<<( std::ostream &out, Ref<NIgnore> ni );
    
 private:
    MATCH match_dir( Ref<Node> node, std::string & strip );
};

std::ostream & operator<<( std::ostream &out, Ref<NIgnore> ni );

class NIgnoreChain
{
    std::vector< Ref<NIgnore> > nignores;
 public:

    NIgnoreChain() {}

    void add( Ref<NIgnore> ni ) { nignores.push_back( ni ); }

    bool match( Ref<Node> node );

    unsigned size() const { return nignores.size(); }
    bool empty() const { return nignores.size(); }

    std::string getName();

    friend std::ostream & operator<<( std::ostream &out, Ref<NIgnoreChain> nc );
};

std::ostream & operator<<( std::ostream &out, Ref<NIgnoreChain> nc );

#endif

#endif
