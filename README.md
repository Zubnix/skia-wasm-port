# skia-wasm-port
Port of the Skia drawing library to wasm, for use in node &amp; browser.

[Skia](https://skia.org/) is the drawing library used by Chrome and Firefox to provide the actual rendering of web pages.

# Goal
The goal of this project is to make skia available to Javascript without the need to rely on native code.
The end result would be a library able to do fast complex 2d drawings in both node & the browser.

# State
Currently no WebIDL bindings are defined yet, so the library is not accessible directly from Javascript. You do get a
static archive that contains all llvm bitcode needed to generate the final wasm, as well as a script to generate this
llvm bitcode from Skia sources. This library will become usable as soon as some working WebIDL bindings are defined and
glue code is generated and included.

# Building
Cloning this library requires [git lfs](https://git-lfs.github.com/)

