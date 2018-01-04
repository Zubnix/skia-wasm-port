const fs = require('fs')
const skia = require('./skia')

function draw (skiaModule, canvas) {
  canvas.clear(0xFFFFFFFF) // white

  const paint = new skiaModule.SkPaint()
  paint.setTextSize(64.0)
  paint.setAntiAlias(true)
  paint.setColor(0xff4281A4)

  canvas.drawText('Skia', 20, 64, paint)
}

function main (skiaModule) {
  skiaModule.FS.mkdir('/usr')
  skiaModule.FS.mkdir('/usr/share')
  skiaModule.FS.mkdir('/usr/share/fonts')
  // by putting our fonts at /usr/share/fonts, skia will automagically find them.
  skiaModule.FS.mount(skiaModule.FS.filesystems.NODEFS, {root: './fonts'}, '/usr/share/fonts')

  const width = 256
  const height = 256
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

