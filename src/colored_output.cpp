/*
 * colored_output.cc
 *
 *  Created on: 04.05.2014
 *      Author: martin
 */

#include "colored_output.h"
#include <sstream>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <unistd.h>

ColoredOutput::ColoredOutput()
: colored_output( true )
{
	char *pcTerm =  getenv( "TERM");

	if( pcTerm == NULL || !isatty(fileno(stdout)) )
	{
		colored_output = false;
	}
	else
	{
		// std::cout << "TERM: " << pcTerm << std::endl;
	}
}

std::string ColoredOutput::color_output( Color color, const std::string & text ) const
{
	if( !colored_output )
		return text;

	std::stringstream str;

	switch( color )
	{
	case BLACK:              str << "\033[0;30m"; break;
	case RED:                str << "\033[0;31m"; break;
	case GREEN:              str << "\033[0;32m"; break;
	case YELLOW:             str << "\033[0;33m"; break;
	case BLUE:               str << "\033[0;34m"; break;
	case MAGENTA:            str << "\033[0;35m"; break;
	case CYAN:               str << "\033[0;36m"; break;
	case WHITE:              str << "\033[0;37m"; break;
	case BRIGHT_BLACK:       str << "\033[1;30m"; break;
	case BRIGHT_RED:         str << "\033[1;31m"; break;
	case BRIGHT_GREEN:       str << "\033[1;32m"; break;
	case BRIGHT_YELLOW:      str << "\033[1;33m"; break;
	case BRIGHT_BLUE:        str << "\033[1;34m"; break;
	case BRIGHT_MAGENTA:     str << "\033[1;35m"; break;
	case BRIGHT_CYAN:        str << "\033[1;36m"; break;
	case BRIGHT_WHITE:		 str << "\033[1;37m"; break;
	}

	str << text;

	str << "\033[0m";

	return str.str();
}
