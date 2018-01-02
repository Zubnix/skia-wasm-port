#include "skia.h"

#include <emscripten.h>
#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(skia_module) {
    // SkSurfaceProps.h ->
    enum_<SkPixelGeometry>("SkPixelGeometry")
        .value("kUnknown_SkPixelGeometry",SkPixelGeometry::kUnknown_SkPixelGeometry)
        .value("kRGB_H_SkPixelGeometry",SkPixelGeometry::kRGB_H_SkPixelGeometry)
        .value("kBGR_H_SkPixelGeometry",SkPixelGeometry::kBGR_H_SkPixelGeometry)
        .value("kRGB_V_SkPixelGeometry",SkPixelGeometry::kRGB_V_SkPixelGeometry)
        .value("kBGR_V_SkPixelGeometry",SkPixelGeometry::kBGR_V_SkPixelGeometry)
    ;
    enum_<SkSurfaceProps::Flags>("SkSurfaceProps.Flags")
        .value("kUseDeviceIndependentFonts_Flag", SkSurfaceProps::Flags::kUseDeviceIndependentFonts_Flag)
    ;
    class_<SkSurfaceProps>("SkSurfaceProps")
        .constructor<uint32_t, SkPixelGeometry>()
        .function("flags", &SkSurfaceProps::flags)
    ;

    EM_ASM(
        Module['SkSurfaceProps']['Flags'] = Module['SkSurfaceProps.Flags'];
        delete Module['SkSurfaceProps.Flags'];
    );
    // SkSurfaceProps.h ^

    // SkBitmap.h ->
    class_<SkBitmap>("SkBitmap")
    ;
    // SkBitmap.h ^

    // SkImageInfo.h ->
    class_<SkImageInfo>("SkImageInfo")
        .constructor<>()
    ;
    // SkImageInfo.h ^

    //SkImage.h ->
    class_<SkImage>("SkImage")
    ;
    //SkImage.h ^

    // SkPaint.h
    class_<SkPaint>("SkPaint")
        .constructor<>()
        .function("setAntiAlias",&SkPaint::setAntiAlias)
    ;
    // SkPaint.h ^

    // SkCanvas.h ->
    class_<SkCanvas>("SkCanvas")
        .constructor<>()
        .function("flush",&SkCanvas::flush)
    ;
    // SkCanvas ^

    // SkSurface.h ->
    class_<SkSurface>("SkSurface")
        .smart_ptr<sk_sp<SkSurface>>("sk_sp<SkSurface>")
        .class_function("MakeRasterDirect",&SkSurface::MakeRasterDirect, allow_raw_pointers())
        .class_function("MakeRasterN32Premul",&SkSurface::MakeRasterN32Premul, allow_raw_pointers())
        .class_function("MakeRasterAutoRow", select_overload<sk_sp<SkSurface>(const SkImageInfo&,const SkSurfaceProps*)>(&SkSurface::MakeRaster), allow_raw_pointers())
        .function("width",&SkSurface::width)
        .function("height",&SkSurface::height)
        .function("canvas",&SkSurface::getCanvas, allow_raw_pointers())
    ;
    // SkSurface.h ^
}
