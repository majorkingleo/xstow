#ifndef COLORED_OUTPUT_H_
#define COLORED_OUTPUT_H_

class ColoredOutput {
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
        BRIGHT_WHITE
    };

    ColoredOutput();

    std::string color_output(Color color, const std::string &text) const;

    void setColoredOutput(bool colored_output_) {
        colored_output = colored_output_;
    }

private:
    bool colored_output;
};

#endif // COLORED_OUTPUT_H_
