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
    <space>
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
    :flags <mask> <value>
           Set flags of the current fractal. Mask is bit mask of bits that will
           be set, value is the value of the bits. The meaning of the bits is
           dependant on the fractal, but usually the lowest 4 bits are coloring
           type.
    :gradient1 <gradient-name>
           Select gradient.
    :gradient1 <size> <color-points>
           Define gradient.
).";

Help::Help(const gui::Text &text_cfg) : _text(text_cfg) {
    _text.add_text(HELP_TEXT, { 10, 20 });
}

} // namespace fio::fractals