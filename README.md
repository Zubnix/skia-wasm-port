# skia-wasm-port
Port of the Skia drawing library to wasm, for use in node &amp; browser.

[Skia](https://skia.org/) is the drawing library used by Chrome and Firefox to provide the actual rendering of web pages.

# Goal
The goal of this project is to make skia available to Javascript without the need to rely on native code.
The end result would be a library able to do fast complex 2d drawings in both node & the browser.

# State
Currently a limited set of skia functions are defined (wip), so the library is not very usable yet. You do get a
static archive that contains all llvm bitcode needed to generate the final wasm, as well as a script to generate this
llvm bitcode from Skia sources. This library will become usable as soon as enough bindings are defined.

An example is currently available under `example_star.js` that can be run with node. Running in the
browser is also possible if the png file saving is adjusted accordingly. The example exactly matches the one found
on the Skia [website](https://skia.org/user/api/creating_skcanvas).

`example_star.js` demonstrates path rendering:

![alt text](https://raw.githubusercontent.com/Zubnix/skia-wasm-port/master/out_star.png)

`example_gradient` demonstrates gradient shader:

![alt text](https://raw.githubusercontent.com/Zubnix/skia-wasm-port/master/out_gradient.png)

`example_text.js` demonstrates font rendering:

![alt text](https://raw.githubusercontent.com/Zubnix/skia-wasm-port/master/out_text.png)

# Defining bindings
Bindings are defined using [Embind](https://kripken.github.io/emscripten-site/docs/porting/connecting_cpp_and_javascript/embind.html).
Currently defined bindings live in `skia_bindings.cpp`.

# Building
This library requires [git lfs](https://git-lfs.github.com/)

Make sure you have the emscripten sdk installed. Current code has been tested on the linux-incoming-64bit version.

Make sure you define the correct paths in `build_bindings.sh` and run `build_bindings.sh`. You should get a `skia.js` 
and a `skia.wasm` file. To test the build, simply run `example_star.js`.