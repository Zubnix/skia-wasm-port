#!/bin/bash

#eg /home/john/emsdk-portable
EMSDK="path/to/em/sdk"
#eg 1.37.27 or incoming
EM_VERSION="0.0.0"

source ${EMSDK}/emsdk_env.sh

./bin/gn gen ./out/Build-wasm-Release/Release --args="cc=\"${EMSDK}/emscripten/${EM_VERSION}/emcc\" extra_cflags_cc=[\"-frtti\"] cxx=\"${EMSDK}/emscripten/${EM_VERSION}/em++\" extra_cflags=[\"-Wno-unknown-warning-option\"] is_debug=false skia_enable_gpu=false skia_use_dng_sdk=false skia_use_fontconfig=false skia_use_freetype=false skia_use_icu=false target_cpu=\"wasm\""

ninja -k 0 -C out/Build-wasm-Release/Release
