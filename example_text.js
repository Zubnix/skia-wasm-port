const fs = require('fs')
const skia = require('./skia')

function draw (skiaModule, canvas) {
  canvas.clear(0xFFFFFFFF) // white

  const paint = new skiaModule.SkPaint()
  paint.setTextSize(48.0)
  paint.setAntiAlias(true)
  paint.setColor(0xff4281A4)
  const liberationMonoRegular = skiaModule.SkTypeface.MakeFromFile('/usr/share/fonts/liberation-fonts/LiberationMono-Regular.ttf', 0)
  paint.setTypeface(liberationMonoRegular)

  const paint1 = new skiaModule.SkPaint()
  paint1.setTextSize(48.0)
  paint1.setAntiAlias(true)
  paint1.setColor(0xff6291B4)
  const liberationSerifBoldItalic = skiaModule.SkTypeface.MakeFromFile('/usr/share/fonts/liberation-fonts/LiberationSerif-BoldItalic.ttf', 0)
  paint1.setTypeface(liberationSerifBoldItalic)

  const paint2 = new skiaModule.SkPaint()
  paint2.setTextSize(48.0)
  paint2.setAntiAlias(true)
  paint2.setColor(0xff6291B4)
  const liberationSansRegular = skiaModule.SkTypeface.MakeFromFile('/usr/share/fonts/liberation-fonts/LiberationSans-Regular.ttf', 0)
  paint2.setTypeface(liberationSansRegular)

  canvas.drawText('Liberation Mono Regular', 20, 64, paint)
  canvas.drawText('Liberation Serif Bold Italic', 20, 128, paint1)
}

function main (skiaModule) {
  skiaModule.FS.mkdir('/usr')
  skiaModule.FS.mkdir('/usr/share')
  skiaModule.FS.mkdir('/usr/share/fonts')
  // by putting our fonts at /usr/share/fonts, skia will automagically find them.
  skiaModule.FS.mount(skiaModule.FS.filesystems.NODEFS, {root: './fonts'}, '/usr/share/fonts')

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


