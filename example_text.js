const fs = require('fs')
const path = require('path')
const skia = require('./skia')

function draw (skiaModule, canvas) {
  canvas.clear(0xFFFFFFFF) // white

  // read data from file & copy it to wasm memory
  const data = fs.readFileSync(path.join(__dirname, '.', 'fonts', 'liberation-fonts', 'LiberationMono-Regular.ttf'))
  const nativeData = skiaModule.SkData.MakeUninitialized(data.byteLength)
  const nativeBuffer = skiaModule.getSkDataBytes(nativeData)
  nativeBuffer.set(data)
  const liberationMonoRegular = skiaModule.SkFontMgr.RefDefault().makeFromData(nativeData, 0)
  const paint = new skiaModule.SkPaint()
  paint.setTextSize(48.0)
  paint.setAntiAlias(true)
  paint.setColor(0xff4281A4)
  paint.setTypeface(liberationMonoRegular)

  const data1 = fs.readFileSync(path.join(__dirname, '.', 'fonts', 'liberation-fonts', 'LiberationSerif-BoldItalic.ttf'))
  const nativeData1 = skiaModule.SkData.MakeUninitialized(data1.byteLength)
  const nativeBuffer1 = skiaModule.getSkDataBytes(nativeData1)
  nativeBuffer1.set(data1)
  const liberationSerifBoldItalic = skiaModule.SkFontMgr.RefDefault().makeFromData(nativeData1, 0)
  const paint1 = new skiaModule.SkPaint()
  paint1.setTextSize(48.0)
  paint1.setAntiAlias(true)
  paint1.setColor(0xff6291B4)
  paint1.setTypeface(liberationSerifBoldItalic)

  canvas.drawText('Liberation Mono Regular', 20, 64, paint)
  canvas.drawText('Liberation Serif Bold Italic', 20, 128, paint1)
}

function main (skiaModule) {
  const width = 768
  const height = 192
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


