# Fractalio

Project to the subject PGR - rendering fractals.

## Controls

```
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
```

## Fractals

### Set fractals.

The view is complex plane; each pixel represents a complex number. The pixel is
colored by the properties of the complex number. The fractal represents set of
complex numbers. Complex number $p$ belongs to the set if
$x_{n+1}=f(x_n, p, \theta)$ is bounded as $n \rightarrow \infty$ for some given
$f(x, p, \theta)$ where $p$ is number given by the pixel position and $\theta$
is vector of parameters.

In most cases, it is true that if $|x_n| > B$ than $p$ doesn't belong to the
set. In practice there is maximum iteration limit after which the number will
be considered part of the set.

If number given by the pixel belongs to the set, it is colred black. If it
doesn't belong to the set, it is colored based on the value of $n$ for the
first $x_n > B$. The color is chosen from a gradient.

Showcase of how the fractal forms with increasing the maximum number of
iterations on mandelbrot set:

![](assets/set-max-iter.gif)

Flags availeble for set fractals (showcased on mandelbrot set):
- `0xF`: Coloring method:
  - `0x0`, `basic`: Basic stepped coloring.
  - ![](assets/set-basic.png)
  - `0x1`, `smooth`: Smooth coloring (smoothing is not perfect).
  - ![](assets/set-smooth.png)
  - `0x2`, `shade-step`: Shade gradient steps.
  - ![](assets/set-shade-step.png)
  - `0x3`, `color-step`: Color gradient steps.
  - ![](assets/set-color-step.png)
  - `0x4`, `log-shade-step`: Shade gradient steps with logarithmic color
    change.
  - ![](assets/set-log-shade-step.png)

#### Mandelbrot set

$
x_0 = 0 \\
x_{n+1} = x_n^2+p \\
B = 2
$

Overall fractal:

![](assets/mandelbrot.png)

Zoomed part:

![](assets/mandelbrot-zoom.png)

Zoom process:

![](assets/mandelbrot-zoom.gif)

#### Julia set

$
x_0 = p \\
x_{n+1} = x_n^2 + \theta \\
B = 2
$

Transofrmation of the set with change of the parameter $\theta$

![](assets/julia.gif)

Julia for one of the parameters:

![](assets/julia.png)

#### Powerbrot

$
x_0 = 0 \\
x_{n+1} = x_n^\theta + p \\
B = max(|\theta|, 2)
$

Note that the value of $B$ is not correct from the view of derermining the set
of numbers because for some values of $\theta$ the set contains all the complex
numbers.

This is equivalent to mandelbrot set when $\theta = 2$. The reason that this is
separate fractal is that this generic version is much harder to compute and so
it is more limiting when trying to zoom in (only single precision, more
demanding).

Showcase of how the fractal changes with the parameter $\theta$ first in real
positive values and than in complex values:

![](assets/powerbrot.gif)

#### Burning ship

$
x_0 = 0 \\
x_{n+1} = |\Re(x_n^2 + p)| + i|\Im(x_n^2 + p)| \\
B = 2
$

Overall fractal:

![](assets/burning-ship.png)

Zoomed portion:

![](assets/burning-ship-zoom.png)

#### Burning julia

$
x_0 = p \\
x_{n+1} = |\Re(x_n^2 + \theta)| + i|\Im(x_n^2 + \theta)| \\
B = 2
$

Showcase of how the fractal changes with change of the parameter $\theta$:

![](assets/burning-julia.gif)

Burning julia for one of the parameters $\theta$:

![](assets/burning-julia.png)

Zoom into fractal with the same $\theta$:

![](assets/burning-julia-zoom.png)

### Newton

The newton fractal shows complex plane. Each pixel coresponds to a complex
number and it is colored according to how that number behaves.

Newton's fractal is defined by a polynom $f(x) = (x-r_0)(x-r_1)...(x-r_n)$
where $r_m$ is the mth root of the polynom. To show the fractal we use the
number given by pixel position as initial estimate of a root when finding root
of the polynom with newton's method. The color of the pixel is given by the
root wich is found and how fast is the root found.

The newton's method for finding root of a polynomial with initial estimate
$x_0$ is:

$
x_{m+1}=x_m- \frac{f(x_m)}{f'(x_m)}
$

Flags available for newton's fractal:
- `0xF`: coloring method:
  - `0x0`, `flat`: color is given by the final root
  - ![](assets/newton-flat.png)
  - `0x1`, `bright`: color is given by the final root and is brighter for roots
    that took longer to find.
  - ![](assets/newton-bright.png)
  - `0x2`, `dark`: color is given by the final root and is darker for roots
    that took longer to find.
  - ![](assets/newton-dark.png)
  - `0x3`, `iter`, `iteration`: color is given by how long it took to find the
    root
  - ![](assets/newton-iter.png)
  - `0x4`, `log-iter`, `log-iteration`: color is given by logarithm of how long
    it took to find the root.
  - ![](assets/newton-log-iter.png)
- `0x10`, `smooth`: enable/disable smooth coloring
- ![](assets/newton-smooth.png)
- `0x20`, `overlay`, `par`, `parameters`: enable/disable parameter position
  overlay
- ![](assets/newton-par.png)

Showcase of how the fractal changes by changing the position of roots and
adding more roots:

![](assets/newton.gif)

### Double pendulum

TODO

### Three body

TODO

### Gravity basins

TODO

### Littlewood

TODO
