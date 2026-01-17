#include "help.hpp"

#include "../gui/text.hpp"

namespace fio::fractals {

constexpr std::string_view HELP_TEXT = R".(HELP
  Drag with left mouse button to move (even here in help ;). Drag with right
  mouse button (up/down) to change the scale.
    
  Fractalio uses vim like commands. The command structure is:
    <modifier><count><command>
      <modifier> may be one of: `+`, `-`, `*`, `/`, `=`.
      <count> is signed floating point number.
  Commands will be executed immidietelly after they are typed. You can confirm
  command sooner by pressing <enter>. Pressing <escape> will cancel the
  partially type command. The special commands starting with `:` will be
  executed only after pressing enter.

  Commands:
    `?` `gh` `:h` `:help`
           Show this help.
    <esc>  Cancel currently typed command. If there is no such command, make
           the base fractal focused.
    :fractal
           Give command focus to the base fractal.
    <space>  :picker
           Give command focus to the picker (if present).
    <tab>  Toggle single/double precision.
    `r`<char>
           Reset configuration of fractal. The next character will decide what
           should be reset. Available options are `r` - reset all parameters
           except scale and position, `p` - reset position and scale, char
           corresponding to parameter to reset (`i`, `c`, `x`, `y`, `z`).
    `R`    Reset
    `:x` `:exit` `:q`  `:quit`
           Exit.
    `;`    Repeat the last command.
    `::`<characters>
           Simulate the press of the following characters.
    `z`    Set scale.
    `i`    Set number of iterations.
    `c`    Set number of colors.
    `x`    Set x (real) coordinate.
    `y`    Set y (imaginary) coordinate.
    `t`    Set time.
    `s`    Set maximum step size.
    `v`    Set simulation speed.
    `:vsync`  <int>
           Enable/disable vsync. (0 disable, 1 enable)
    `:fps` <int>
           Limit the fps.
    `g`<char>
           Go to the given fractal:
             `h`: Help
             `m`: Mandelbrot set
             `j`: Julia set
             `s`: Burning ship
             `n`: Newton fractal
             `p`: Double pendulum
             `l`: Littlewood
    <number>`G`
           Go to fractal/page identified by number:
             0: Help
             1: Mandelbrot set
             2: Julia set
             3: Burning ship
             4: Powerbrot
             5: Newton fractal
             6: Burning julia
             7: Double pendulum
             8: Three body
             9: Gravity basins
             10: Littlewood
    :flags <mask> <value>
           Set flags of the current fractal. Mask is bit mask of bits that will
           be set, value is the value of the bits. The meaning of the bits is
           dependant on the fractal, but usually the lowest 4 bits are coloring
           type.
    :flag <name>
           Named equivalient of :flags. Names are defined per fractal.
    :grad1  :gradient1 <gradient-name>|[<size>@]<color-points>
           Select/define gradient. Selectible gradients:
             `ultra-fractal`  blue - white - yellow - black - blue (default)
             `grayscale`      black - white - black
             `burn`           red - yellow - white - black - red
             `monokai`        pink - yellow - green - blue - pink
             `rgb`            red - green - blue - red
             `cmy`            cyan - magenta - yellow - cyan
           Color points format: comma separated pairs of index:#rrggbb. Index
           is value from 0 to 1. Size is the resolution, default is 256.
    :par  :parameter [index] <x> <y>
           Set value of a parameter. The default index is 0. Using idex larger
           than the current number of parameters will add new parameters up to
           that index.
    :set <name> [<x> [<y>]]
           Set attribute given by name. If there is no value, it will reset the
           value to default.
    :save [filename]
           Save the current configuration to a file. `-` is stdout. If file is
           not present, save to clipboard.
    :load [filename]
           Load configuration from file. `-` is stdin. If file is not present,
           load from clipboard.
    :precision 1|2
           Set precision to single or double.
).";

Help::Help(const gui::Text &text_cfg) : _text(text_cfg) {
    _text.add_text(HELP_TEXT, { 10, 20 });
}

std::string_view help_text() {
    return HELP_TEXT;
}

} // namespace fio::fractals