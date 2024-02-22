DEC VT240
=========

This is a somewhat accurate emulation of a DEC VT240 terminal with fancy
graphics. This repository also includes a web port using emscripten, but you
will have to provide some useful rx/tx function to get a meaningful program and
due to inherent limitations of browsers, there are certain restrictions on the
keyboard behavior.

Special care was taken to create a visually appealing appearance which
explicitly tries to look similar to a real VT240 instead of inventing random
"graphics effects" (more like "distortions") like certain other terminal
emulators with "fancy graphics" do.


Hardware Requirements
---------------------

- Graphics card with OpenGL 3.2+


Build Requirements
------------------

For the native version:
- Linux with git, gcc and make
- freeglut
- glslang
- [bin2o](https://github.com/hackyourlife/bin2o)

For the web version:
- Linux with git and make
- emscripten
- glslang


Building
--------

Just run `make` to get the native binary. If you want to get the web binary
instead, run `make` in the `web` folder.


Usage
-----

`vt240` without arguments launches the VT240 in "local mode", that is, the RX
and TX pin on the virtual terminal is connected and everything you type is
directly displayed. This is useful if you want to manually design UI prototypes.

`vt240 -s` starts the default shell, `vt240 -s /path/to/the/binary` runs an
arbitrary program in the VT240. The `TERM` environment variable is set to
`vt220` and if the `LANG` environment variable is set to an UTF-8 language, it
is reset to `LANG=C` to avoid problems.

`vt240 host port` connects to the telnet server `host` at port `port`.


Keyboard
--------

A VT240 does not have all the keys you usually find on a PC keyboard and some
keys have a very different meaning. In particular, the function keys F1-F5 are
"local" keys which configure the terminal and have no associated sequence that
could be sent to the host.

The VT240 emulator uses the following map for local keys:
- F2 = toggle fullscreen
- F3 = setup
- F5 = exit the terminal emulator


Completeness
------------

Feel free to test this emulator with
[vttest](https://invisible-island.net/vttest/) to see how good this emulator is.

Compared to a real VT240, the following features are currently missing:
- 132 character mode
- UDK ("user defined keys")
- DLCS ("custom character set")
- ReGIS
- Tektronix 4014
- color mode

This VT240 emulator is implemented purely using the VT240 reference manual,
because a real VT240 is currently not available to the developer. If the
emulator behaves differently than a real VT240, even if it is just some edge
case, it is a bug. Please report such bugs with the necessary input and a
photo of how the result should look like as well as a short explanation of what
is wrong, in case this is not directly obvious from the input/photo.


Windows Version
---------------

Although there are some code fragments which look like there might be Windows
support, there is currently no such support. This is simply caused by the fact
that no Windows based machine with C compiler and graphics card is currently
available to the developer. If you are bored and want to provide proper Windows
support, feel free to open a pull request.

Only the TELNET library supports Windows at the moment since it was written
on a Windows computer many years ago, but even this is currently untested.


Screenshots
-----------

vim in a screen session:

![VIM in a screen session](https://raw.githubusercontent.com/unknown-technologies/vt240/master/doc/vt240-vim.png)

Sixel graphics in action:

![Sixel graphics](https://raw.githubusercontent.com/unknown-technologies/vt240/master/doc/vt240-e1.png)
