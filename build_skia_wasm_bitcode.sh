#!/bin/bash

patch -p0 < float_nan.patch
patch -p0 < wasm_compatible_build.patch

#eg /home/john/emsdk-portable
EMSDK="/home/zubzub/emsdk-portable"
#eg 1.37.27
EM_VERSION="incoming"

source ${EMSDK}/emsdk_env.sh

./bin/gn gen ./out/Build-wasm-Release/Release --args="cc=\"${EMSDK}/emscripten/${EM_VERSION}/emcc\" extra_cflags_cc=[\"-frtti\",\"-s\",\"USE_FREETYPE=1\"] cxx=\"${EMSDK}/emscripten/${EM_VERSION}/em++\" extra_cflags=[\"-Wno-unknown-warning-option\",\"-s\",\"USE_FREETYPE=1\",\"-s\",\"USE_LIBPNG=1\"] \
is_debug=false \
is_official_build=true \
is_component_build=false \
target_cpu=\"wasm\" \
\
skia_use_egl=true \
skia_use_vulkan=false \
skia_use_libwebp=false \
skia_use_libpng=true \
skia_use_lua=false \
skia_use_dng_sdk=false \
skia_use_fontconfig=false \
skia_enable_skottie=false \
skia_use_libjpeg_turbo=false \
skia_use_libheif=false \
skia_use_expat=false \
skia_use_vulkan=false \
skia_use_freetype=true \
skia_use_icu=false \
skia_use_expat=false \
skia_use_piex=false \
skia_use_zlib=true \
skia_use_skcms=false \
\
skia_enable_gpu=true \
skia_enable_pdf=false"

ninja -k 0 -C out/Build-wasm-Release/Release
