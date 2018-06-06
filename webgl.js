"use strict"

function main(skiaModule) {
    const canvas = document.createElement("canvas")
    const id = 'somecanvas'
    canvas.id = id
    canvas.width = 800
    canvas.height = 600
    document.body.appendChild(canvas)

    const gpuSurface = skiaModule.makeWebGLSurface(canvas.id, canvas.width, canvas.height)

    const gpuCanvas = gpuSurface.getCanvas()
    gpuCanvas.clear(0xFFFFFFFF)//white

    const paint = new skiaModule.SkPaint()

    const points = new skiaModule.VectorSkPoint()
    points.push_back(skiaModule.SkPoint.Make(0, 0))
    points.push_back(skiaModule.SkPoint.Make(256, 256))

    const colors = new skiaModule.VectorSkColor()
    colors.push_back(0xFF00FF00)
    colors.push_back(0xFFFF0000)

    const pos = new skiaModule.VectorSkScalar()

    paint.setShader(skiaModule.SkGradientShader.MakeLinear(points, colors, pos, 2, skiaModule.SkShader.TileMode.kClamp_TileMode))
    gpuCanvas.drawPaint(paint)
    gpuCanvas.flush()
}


function loadNativeModule(skiaModule) {
    return new Promise((resolve) => {
        if (skiaModule.calledRun) {
            resolve()
        } else {
            skiaModule.onRuntimeInitialized = () => {
                resolve()
            }
        }
    })
}

window.onload = async () => {
    // make sure the native modules is ready for use before we start our main flow
    const skiaModule = Module()
    await loadNativeModule(skiaModule)
    try {
        main(skiaModule)
    } catch (error) {
        console.error(error)
    }
}