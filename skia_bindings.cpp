#include <iostream>
#include "skia.h"

#include <emscripten.h>
#include <emscripten/html5.h>

// Hack to avoid embind creating a binding for SkData destructor
namespace emscripten {
    namespace internal {
        template<typename ClassType>
        void raw_destructor(ClassType *);

        template<>
        void raw_destructor<SkData>(SkData *ptr) {
        }
    }
}
// Hack to avoid embind creating a binding for SkTypeface destructor
namespace emscripten {
    namespace internal {
        template<typename ClassType>
        void raw_destructor(ClassType *);

        template<>
        void raw_destructor<SkTypeface>(SkTypeface *ptr) {
        }
    }
}

#include <emscripten/bind.h>

using namespace emscripten;
using emscripten::val;
using emscripten::vecFromJSArray;

// to map from raw memory to a uint8array
val getSkDataBytes(const SkData *data) {
    return val(typed_memory_view(data->size(), data->bytes()));
}

sk_sp <SkSurface> makeWebGLSurface(std::string id, int width, int height) {
    // Context configurations
    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);
    attrs.majorVersion = 2;
    attrs.enableExtensionsByDefault = true;

    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context = emscripten_webgl_create_context(id.c_str(), &attrs);
    printf("create webgl context %d\n", context);
    if (context < 0) {
        printf("failed to create webgl context %d\n", context);
    }
    EMSCRIPTEN_RESULT r = emscripten_webgl_make_context_current(context);
    printf("make webgl current %d\n", r);
    if (r < 0) {
        printf("failed to make webgl current %d\n", r);
    }

    // setup GrContext
    sk_sp<const GrGLInterface> grGLInterface = nullptr;
    // setup contexts
    sk_sp <GrContext> grContext = GrContext::MakeGL(grGLInterface);
    printf("grContext %p\n", grContext.get());

    const SkImageInfo info = SkImageInfo::MakeN32(800, 600, kPremul_SkAlphaType);
    sk_sp <SkSurface> gpuSurface = SkSurface::MakeRenderTarget(grContext.get(), SkBudgeted::kNo, info, 0,
                                                               kTopLeft_GrSurfaceOrigin,
                                                               nullptr, false);
    printf("gpuSurface %p\n", gpuSurface.get());
    if (!gpuSurface.get()) {
        printf("failed to create gpu surface.");
    }

    return gpuSurface;
}

EMSCRIPTEN_BINDINGS(skia_module) {
        // register array types, used by various functions to provide arrays as arguments functionality
        register_vector<SkPoint>("VectorSkPoint");
        register_vector<SkColor>("VectorSkColor");
        register_vector<SkScalar>("VectorSkScalar");

        function("getSkDataBytes", &getSkDataBytes, allow_raw_pointers());
        function("GrGLMakeAssembledInterface", &GrGLMakeAssembledInterface, allow_raw_pointers());
        function("makeWebGLSurface", &makeWebGLSurface, allow_raw_pointers());

        // GrContext.h ->
        class_<GrGLInterface>("GrGLInterface");
        // GrContext.h ^

        // SkSurfaceProps.h ->
        class_<SkSurfaceProps>("SkSurfaceProps")
        .constructor<uint32_t, SkPixelGeometry>()
        .function("flags", &SkSurfaceProps::flags);

        enum_<SkPixelGeometry>("SkPixelGeometry")
        .value("kUnknown_SkPixelGeometry", SkPixelGeometry::kUnknown_SkPixelGeometry)
        .value("kRGB_H_SkPixelGeometry", SkPixelGeometry::kRGB_H_SkPixelGeometry)
        .value("kBGR_H_SkPixelGeometry", SkPixelGeometry::kBGR_H_SkPixelGeometry)
        .value("kRGB_V_SkPixelGeometry", SkPixelGeometry::kRGB_V_SkPixelGeometry)
        .value("kBGR_V_SkPixelGeometry", SkPixelGeometry::kBGR_V_SkPixelGeometry);

        enum_<SkSurfaceProps::Flags>("SkSurfaceProps.Flags")
        .value("kUseDeviceIndependentFonts_Flag", SkSurfaceProps::Flags::kUseDeviceIndependentFonts_Flag);
        EM_ASM(
        Module['SkSurfaceProps']['Flags'] = Module['SkSurfaceProps.Flags'];
        delete Module['SkSurfaceProps.Flags'];
        );
        // SkSurfaceProps.h ^

        // SkBitmap.h ->
        class_<SkBitmap>("SkBitmap");
        // SkBitmap.h ^

        // SkImageInfo.h ->
        class_<SkImageInfo>("SkImageInfo")
        .constructor<>();
        // SkImageInfo.h ^

        // SkData.h ->
        class_<SkData>("SkData")
        .smart_ptr<sk_sp<SkData>>("sk_sp<SkData>>")
        .class_function("MakeFromMalloc",
        optional_override([](std::string data, size_t length)->sk_sp<SkData>{
            return SkData::MakeFromMalloc(&data[0], length);
        }),
        allow_raw_pointers())
        .class_function("MakeUninitialized", &SkData::MakeUninitialized)
        .function("size", &SkData::size);
        // SkData.h ^

        // SkPoint.h ->
        class_<SkPoint>("SkPoint")
        .class_function("Make", &SkPoint::Make)
        .property("fX", &SkPoint::fX)
        .property("fY", &SkPoint::fY);
        // SkPoint.h ^

        // SkString.h ->
        class_<SkString>("SkString")
        .constructor<>()
        .function("c_str",
        optional_override([](SkString& this_)->std::string{
            return std::string(this_.SkString::c_str());
        })
        );
        // SkString.h ^

        //SkImage.h ->
        class_<SkImage>("SkImage")
        .smart_ptr<sk_sp<SkImage>>("sk_sp<SkImage>")
        .function("encodeToData", select_overload<sk_sp<SkData>()const>(&SkImage::encodeToData));
        //SkImage.h ^

        // SkPath.h ->
        class_<SkPath>("SkPath")
        .constructor<>()
        .constructor<const SkPath&>()
        .function("moveToXY",
        select_overload<void(SkScalar, SkScalar)>(&SkPath::moveTo))
        .function("moveToPoint",
        select_overload<void(const SkPoint& p)>(&SkPath::moveTo))
        .function("lineToXY",
        select_overload<void(SkScalar, SkScalar)>(&SkPath::lineTo))
        .function("lineToPoint",
        select_overload<void(const SkPoint& p)>(&SkPath::lineTo))
        .function("close", &SkPath::close);
        // SkPath.h ^

        // SkFontStyle.h ->
        class_<SkFontStyle>("SkFontStyle")
        .constructor<>()
        .class_function("Normal", &SkFontStyle::Normal)
        .class_function("Bold", &SkFontStyle::Bold)
        .class_function("Italic", &SkFontStyle::Italic)
        .class_function("BoldItalic", &SkFontStyle::BoldItalic);
        // SkFontStyle ^

        // SkTypeFace ->
        class_<SkTypeface>("SkTypeface")
        .smart_ptr<sk_sp<SkTypeface>>("sk_sp<SkTypeface>")
        .class_function("MakeDefault", &SkTypeface::MakeDefault)
        .class_function("MakeFromFile",
        optional_override([](std::string path, int index)->sk_sp<SkTypeface>{
            return SkTypeface::MakeFromFile(path.c_str(), index);
        }))
        .class_function("MakeFromName",
        optional_override([](std::string familyName, SkFontStyle fontStyle)->sk_sp<SkTypeface>{
            return SkTypeface::MakeFromName(familyName.c_str(), fontStyle);
        }))
        .function("countGlyphs", &SkTypeface::countGlyphs);
        // SkTypeFace ^

        // SkFontMgr.h ->
        class_<SkFontMgr>("SkFontMgr")
        .smart_ptr<sk_sp<SkFontMgr>>("sk_sp<SkFontMgr>")
        .class_function("RefDefault", &SkFontMgr::RefDefault)
        .function("countFamilies", &SkFontMgr::countFamilies)
        .function("getFamilyName", &SkFontMgr::getFamilyName, allow_raw_pointers())
        .function("makeFromData", &SkFontMgr::makeFromData)
        .function("matchFaceStyle", &SkFontMgr::matchFaceStyle, allow_raw_pointers())
        .function("makeFromFile",
        optional_override([](SkFontMgr& this_, std::string path, int ttcIndex)->sk_sp<SkTypeface>{
            return this_.SkFontMgr::makeFromFile(path.c_str(), ttcIndex);
        }));
        // SkFontMgr.h ^

        // SkPaint.h ->
        class_<SkPaint>("SkPaint")
        .constructor<>()
        .function("setAntiAlias", &SkPaint::setAntiAlias)
        .function("setTextSize", &SkPaint::setTextSize)
        .function("setColor", &SkPaint::setColor)
        .function("setStyle", &SkPaint::setStyle)
        .function("setStrokeWidth", &SkPaint::setStrokeWidth)
        .function("setTextScaleX", &SkPaint::setTextScaleX)
        .function("getTypeface", &SkPaint::getTypeface, allow_raw_pointers())
        .function("setTypeface", &SkPaint::setTypeface)
        .function("setShader", &SkPaint::setShader);

        enum_<SkPaint::Style>("SkPaint.Style")
        .value("kFill_Style", SkPaint::Style::kFill_Style)
        .value("kStroke_Style", SkPaint::Style::kStroke_Style)
        .value("kStrokeAndFill_Style", SkPaint::Style::kStrokeAndFill_Style);
        EM_ASM(
        Module['SkPaint']['Style'] = Module['SkPaint.Style'];
        delete Module['SkPaint.Flags'];
        );
        // SkPaint.h ^

        // SkCanvas.h ->
        class_<SkCanvas>("SkCanvas")
        .constructor<>()
        .function("flush", &SkCanvas::flush)
        .function("clear", &SkCanvas::clear)
        .function("translate", &SkCanvas::translate)
        .function("drawPath", &SkCanvas::drawPath)
        .function("drawText",
        // we wrap the real function in a lambda to do the conversion of the js string (represented as an std:string) to a c string
        optional_override([](SkCanvas& this_, const std::string text, SkScalar x, SkScalar y, const SkPaint& p){
            return this_.SkCanvas::drawText(text.c_str(), text.length(), x, y, p);
        }),
        allow_raw_pointers())
        .function("drawPaint", &SkCanvas::drawPaint);
        // SkCanvas ^

        // SkSurface.h ->
        class_<SkSurface>("SkSurface")
        .smart_ptr<sk_sp<SkSurface>>("sk_sp<SkSurface>")
        .class_function("MakeRasterDirect", &SkSurface::MakeRasterDirect, allow_raw_pointers())
        .class_function("MakeRasterN32Premul", &SkSurface::MakeRasterN32Premul, allow_raw_pointers())
        .class_function("MakeRasterAutoRow",
        select_overload<sk_sp<SkSurface>(const SkImageInfo&, const SkSurfaceProps*)>(&SkSurface::MakeRaster),
        allow_raw_pointers())
        .function("width", &SkSurface::width)
        .function("height", &SkSurface::height)
        .function("getCanvas", &SkSurface::getCanvas, allow_raw_pointers())
        .function("makeImageSnapshot", &SkSurface::makeImageSnapshot);
        // SkSurface.h ^

        // SkShader.h ->
        class_<SkShader>("SkShader")
        .smart_ptr<sk_sp<SkShader>>("sk_sp<SkShader>");

        enum_<SkShader::TileMode>("SkShader.TileMode")
        .value("kClamp_TileMode", SkShader::TileMode::kClamp_TileMode)
        .value("kRepeat_TileMode", SkShader::TileMode::kRepeat_TileMode)
        .value("kMirror_TileMode", SkShader::TileMode::kMirror_TileMode);
        EM_ASM(
        Module['SkShader']['TileMode'] = Module['SkShader.TileMode'];
        delete Module['SkShader.TileMode'];
        );
        // SkShader.h ^

        // SkGradientShader.h ->
        class_<SkGradientShader>("SkGradientShader")
        .class_function("MakeLinear",
        optional_override([](const std::vector<SkPoint> pts, const std::vector<SkColor> colors, const std::vector<SkScalar> pos, int count, SkShader::TileMode mode)->sk_sp<SkShader>{
            const SkPoint *ptsPtr = pts.data();
            const SkColor *colorsPtr = colors.data();
            const SkScalar *posPtr = pos.data();

            return SkGradientShader::MakeLinear(ptsPtr, colorsPtr, posPtr, count, mode);
        }),
        allow_raw_pointers());
        // SkGradientShader.h ^
}
