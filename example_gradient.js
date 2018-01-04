const fs = require('fs')
const skia = require('./skia')

function draw (skiaModule, canvas) {
  canvas.clear(0xFFFFFFFF)//white

  const paint = new skiaModule.SkPaint()

  const points = new skiaModule.VectorSkPoint()
  points.push_back(skiaModule.SkPoint.Make(0, 0))
  points.push_back(skiaModule.SkPoint.Make(256, 256))

  const colors = new skiaModule.VectorSkColor()
  colors.push_back(0xFF00FF00)
  colors.push_back(0xFFFF0000)

  const pos = new skiaModule.VectorSkScalar()

  paint.setShader(skiaModule.SkGradientShader.MakeLinear(points, colors, pos, 2, skiaModule.SkShader.TileMode.kClamp_TileMode))
  canvas.drawPaint(paint)
}

function main (skiaModule) {
  const width = 256
  const height = 256
  const path = 'out_gradient.png'
  const rasterSurface = skiaModule.SkSurface.MakeRasterN32Premul(width, height, null)
  const rasterCanvas = rasterSurface.getCanvas()
  draw(skiaModule, rasterCanvas)

  const img = rasterSurface.makeImageSnapshot()
  if (!img) { return }
  const png = img.encodeToData()
  if (!png) { return }
  const pngBytes = skiaModule.getSkDataBytes(png)
  fs.appendFileSync(path, new Buffer(pngBytes))
}

function loadNativeModule (module) {
  return new Promise((resolve) => {
    if (module.calledRun) {
      resolve()
    } else {
      module.onRuntimeInitialized = () => {
        resolve()
      }
    }
  })
}

// make sure the native modules is ready for use before we start our main flow
const skiaModule = skia()
loadNativeModule(skiaModule).then(() => {
  main(skiaModule)
}).catch((error) => {
  console.log(error)
})


