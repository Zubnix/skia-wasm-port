#!/usr/bin/env bash

#eg /home/john/emsdk-portable
EMSDK="path/to/em/sdk"
#eg 1.37.27 or incoming
EM_VERSION="0.0.0"

#export EMCC_DEBUG=1

printf "Compiling bindings to bitcode\n"
${EMSDK}/emscripten/${EM_VERSION}/emcc -O3 -std=c++11 \
-Iinclude/android \
-Iinclude/atlastext \
-Iinclude/c \
-Iinclude/codec \
-Iinclude/config \
-Iinclude/core \
-Iinclude/effects \
-Iinclude/encode \
-Iinclude/gpu \
-Iinclude/gpu/gl \
-Iinclude/pathops \
-Iinclude/ports \
-Iinclude/svg \
-Iinclude/utils \
-Iinclude/utils/mac \
skia_bindings.cpp -o skia_bindings.bc

printf "Generating final wasm\n"
${EMSDK}/emscripten/${EM_VERSION}/emcc -O3 -std=c++11 --bind skia_bindings.bc libskia_bitcode.a -s WASM=1 -s MODULARIZE=1 -o skia.js