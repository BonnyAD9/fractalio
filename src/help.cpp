#include "help.hpp"

#include "text_renderer/text_renderer.hpp"

namespace fio {

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
    `:vsync`  `on`/`off`
           Enable/disable vsync.
    `g`<char>
           Go to the given fractal:
             `h`: Help.
             `m`: Mandelbrot set.
             `j`: Julia set.
    <number>`G`
           Go to fractal/page identified by number:
             0: Help
             1: Mandelbrot set
             2: Julia set
).";

Help::Help(const TextRenderer &text) : _text(text) {
    _text.add_text(HELP_TEXT, { 10, 20 });
    _text.use();
    _text.prepare();
}

} // namespace fio