#!/bin/bash

#eg /home/john/emsdk-portable
EMSDK="path/to/em/sdk"
#eg 1.37.27 or incoming
EM_VERSION="0.0.0"

source ${EMSDK}/emsdk_env.sh

./bin/gn gen ./out/Build-wasm-Release/Release --args="cc=\"${EMSDK}/emscripten/${EM_VERSION}/emcc\" extra_cflags_cc=[\"-frtti\",\"-s\",\"USE_FREETYPE=1\"] cxx=\"${EMSDK}/emscripten/${EM_VERSION}/em++\" extra_cflags=[\"-Wno-unknown-warning-option\",\"-s\",\"USE_FREETYPE=1\"] is_debug=false skia_enable_gpu=false skia_use_dng_sdk=false skia_use_fontconfig=false skia_enable_pdf=true skia_use_freetype=true skia_use_icu=false skia_use_expat=false target_cpu=\"wasm\""

ninja -k 0 -C out/Build-wasm-Release/Release
