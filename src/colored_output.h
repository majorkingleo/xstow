/*
 * colored_output.h
 *
 *  Created on: 04.05.2014
 *      Author: martin
 */

#ifndef COLORED_OUTPUT_H_
#define COLORED_OUTPUT_H_

#include <string>

class ColoredOutput
{
	bool colored_output;

public:
	enum Color {
		BLACK,
		RED,
		GREEN,
		YELLOW,
		BLUE,
		MAGENTA,
		CYAN,
		WHITE,
		BRIGHT_BLACK,
		BRIGHT_RED,
		BRIGHT_GREEN,
		BRIGHT_YELLOW,
		BRIGHT_BLUE,
		BRIGHT_MAGENTA,
		BRIGHT_CYAN,
		BRIGHT_WHITE,
	};
public:
	ColoredOutput();

	std::string color_output( Color color, const std::string & text ) const;

	void setColoredOutput( bool colored_output_ ) {
			colored_output = colored_output_;
	}
};

#endif /* COLORED_OUTPUT_H_ */
