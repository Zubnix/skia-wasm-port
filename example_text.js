const fs = require('fs')
const skia = require('./skia')

function draw (skiaModule, canvas) {
  canvas.clear(0xFFFFFFFF) // white

  const paint = new skiaModule.SkPaint()

  const typeface = skiaModule.SkTypeface.MakeDefault()
  paint.setTypeface(typeface)
  paint.setTextSize(64.0)
  paint.setAntiAlias(true)
  paint.setColor(0xFFFFFFFF)
  paint.setStyle(skiaModule.SkPaint.Style.kFill_Style)


  const text = 'Skia!'

  canvas.drawStringCharArray(text, 100, 100, paint)
}

function main (skiaModule) {
  const width = 512
  const height = 512
  const path = 'out_text.png'
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


