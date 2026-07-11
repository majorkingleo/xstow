#include "colored_output.h"
#include <iostream>

ColoredOutput::ColoredOutput() {
    colored_output = true;
}

std::string ColoredOutput::color_output(Color color, const std::string &text) const {
    if (!colored_output) {
        return text;
    }

    switch (color) {
        case BLACK:         return "\033[30m" + text + "\033[0m";
        case RED:           return "\033[31m" + text + "\033[0m";
        case GREEN:         return "\033[32m" + text + "\033[0m";
        case YELLOW:        return "\033[33m" + text + "\033[0m";
        case BLUE:          return "\033[34m" + text + "\033[0m";
        case MAGENTA:       return "\033[35m" + text + "\033[0m";
        case CYAN:          return "\033[36m" + text + "\033[0m";
        case WHITE:         return "\033[37m" + text + "\033[0m";
        case BRIGHT_BLACK:  return "\033[90m" + text + "\033[0m";
        case BRIGHT_RED:    return "\033[91m" + text + "\033[0m";
        case BRIGHT_GREEN:  return "\033[92m" + text + "\033[0m";
        case BRIGHT_YELLOW: return "\033[93m" + text + "\033[0m";
        case BRIGHT_BLUE:   return "\033[94m" + text + "\033[0m";
        case BRIGHT_MAGENTA:return "\033[95m" + text + "\033[0m";
        case BRIGHT_CYAN:   return "\033[96m" + text + "\033[0m";
        case BRIGHT_WHITE:  return "\033[97m" + text + "\033[0m";
        default:            return text;
    }
}
