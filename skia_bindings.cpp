#include "skia.h"

#include <emscripten.h>

// Hack to avoid embind creating a binding for SkData destructor
namespace emscripten {
    namespace internal {
        template<typename ClassType>
        void raw_destructor(ClassType*);

        template<>
        void raw_destructor<SkData>(SkData* ptr) {
        }
    }
}
#include <emscripten/bind.h>

using namespace emscripten;

// to map from raw memory to a uint8array
val getSkDataBytes(const SkData* data) {
    return val(typed_memory_view(data->size(), data->bytes()));
}

EMSCRIPTEN_BINDINGS(skia_module) {
    function("getSkDataBytes", &getSkDataBytes, allow_raw_pointers());

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

    // SkData.h ->
    class_<SkData>("SkData")
        .smart_ptr<sk_sp<SkData>>("sk_sp<SkData>>")
        .function("size",&SkData::size)
    ;
    // SkData.h ^

    //SkImage.h ->
    class_<SkImage>("SkImage")
        .smart_ptr<sk_sp<SkImage>>("sk_sp<SkImage>")
        .function("encodeToData",select_overload<sk_sp<SkData>()const>(&SkImage::encodeToData))
    ;
    //SkImage.h ^

    // SkPath.h ->
    class_<SkPath>("SkPath")
        .constructor<>()
        .constructor<const SkPath&>()
        .function("moveToXY",select_overload<void(SkScalar,SkScalar)>(&SkPath::moveTo))
        .function("moveToPoint",select_overload<void(const SkPoint& p)>(&SkPath::moveTo))
        .function("lineToXY",select_overload<void(SkScalar,SkScalar)>(&SkPath::lineTo))
        .function("lineToPoint",select_overload<void(const SkPoint& p)>(&SkPath::lineTo))
        .function("close",&SkPath::close)
    ;
    // SkPath.h ^

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
        .function("clear",&SkCanvas::clear)
        .function("translate",&SkCanvas::translate)
        .function("drawPath",&SkCanvas::drawPath)
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
        .function("getCanvas",&SkSurface::getCanvas, allow_raw_pointers())
        .function("makeImageSnapshot",&SkSurface::makeImageSnapshot)
    ;
    // SkSurface.h ^
}
