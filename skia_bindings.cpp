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
// Hack to avoid embind creating a binding for SkTypeface destructor
namespace emscripten {
    namespace internal {
        template<typename ClassType>
        void raw_destructor(ClassType*);

        template<>
        void raw_destructor<SkTypeface>(SkTypeface* ptr) {
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
    class_<SkSurfaceProps>("SkSurfaceProps")
        .constructor<uint32_t, SkPixelGeometry>()
        .function("flags", &SkSurfaceProps::flags)
    ;

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

    // SkString.h ->
    class_<SkString>("SkString")
        .constructor<>()
        .function("c_str",
            optional_override([](SkString& this_)->std::string{
                            return std::string(this_.SkString::c_str());
                        })
        )
    ;
    // SkString.h ^

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

    // SkTypeFace ->
    class_<SkTypeface>("SkTypeface")
        .smart_ptr<sk_sp<SkTypeface>>("sk_sp<SkTypeface>")
        .class_function("MakeDefault",&SkTypeface::MakeDefault)
        .function("countGlyphs",&SkTypeface::countGlyphs)
    ;
    // SkTypeFace ^

    // SkFontMgr.h ->
    class_<SkFontMgr>("SkFontMgr")
        .smart_ptr<sk_sp<SkFontMgr>>("sk_sp<SkFontMgr>")
        .class_function("RefDefault",&SkFontMgr::RefDefault)
        .function("countFamilies",&SkFontMgr::countFamilies)
        .function("getFamilyName",&SkFontMgr::getFamilyName, allow_raw_pointers())
    ;
    // SkFontMgr.h ^

    // SkPaint.h ->
    class_<SkPaint>("SkPaint")
        .constructor<>()
        .function("setAntiAlias",&SkPaint::setAntiAlias)
        .function("setTextSize",&SkPaint::setTextSize)
        .function("setColor",&SkPaint::setColor)
        .function("setStyle",&SkPaint::setStyle)
        .function("setStrokeWidth",&SkPaint::setStrokeWidth)
        .function("setTextScaleX",&SkPaint::setTextScaleX)
        .function("getTypeface",&SkPaint::getTypeface, allow_raw_pointers())
        .function("setTypeface",&SkPaint::setTypeface)
    ;

    enum_<SkPaint::Style>("SkPaint.Style")
        .value("kFill_Style", SkPaint::Style::kFill_Style)
        .value("kStroke_Style", SkPaint::Style::kStroke_Style)
        .value("kStrokeAndFill_Style", SkPaint::Style::kStrokeAndFill_Style)
    ;
    EM_ASM(
        Module['SkPaint']['Style'] = Module['SkPaint.Style'];
        delete Module['SkPaint.Flags'];
    );
    // SkPaint.h ^

    // SkCanvas.h ->
    class_<SkCanvas>("SkCanvas")
        .constructor<>()
        .function("flush",&SkCanvas::flush)
        .function("clear",&SkCanvas::clear)
        .function("translate",&SkCanvas::translate)
        .function("drawPath",&SkCanvas::drawPath)
        .function("drawStringCharArray",
            // we wrap the real function in a lambda to do the conversion of the js string (represented as an std:string) to a c string
            optional_override([](SkCanvas& this_, const std::string s,SkScalar x,SkScalar y,const SkPaint& p){
                return this_.SkCanvas::drawString(s.c_str(), x, y, p);
            }),
            allow_raw_pointers())
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
