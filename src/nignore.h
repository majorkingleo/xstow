#ifndef NIGNORE_H
#define NIGNORE_H

#include "local_config.h"
#include <range.h>

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

	typedef Tools::EnumRange<EnumMatch> MATCH;

private:
	std::string name;

	std::vector<std::string> dir;
	bool follow;

	bool is_valid;

public:
	NIgnore( const std::string &name, Tools::Leo::Ini &ini );

	MATCH match( Tools::Ref<Node> node, std::string & strip );

	bool valid() const {return is_valid;}

	std::string getName() const {return name;}

	friend std::ostream & operator<<( std::ostream &out, Tools::Ref<NIgnore> ni );

private:
	MATCH match_dir( Tools::Ref<Node> node, std::string & strip );
};

std::ostream & operator<<( std::ostream &out, Tools::Ref<NIgnore> ni );

class NIgnoreChain
{
	std::vector< Tools::Ref<NIgnore> > nignores;
public:

	NIgnoreChain() {}

	void add( Tools::Ref<NIgnore> ni ) {nignores.push_back( ni );}

	bool match( Tools::Ref<Node> node );

	unsigned size() const {return nignores.size();}
	bool empty() const {return nignores.size();}

	std::string getName();

	friend std::ostream & operator<<( std::ostream &out, Tools::Ref<NIgnoreChain> nc );
};

std::ostream & operator<<( std::ostream &out, Tools::Ref<NIgnoreChain> nc );

#endif

#endif
