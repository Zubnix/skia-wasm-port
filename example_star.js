const fs = require('fs')
const skia = require('./skia')

function draw (canvas) {
  const scale = 256.0
  const R = 0.45 * scale
  const TAU = 6.2831853
  const path = new skiaModule.SkPath()
  for (let i = 0; i < 5; ++i) {
    const theta = 2 * i * TAU / 5
    if (i === 0) {
      path.moveToXY(R * Math.cos(theta), R * Math.sin(theta))
    } else {
      path.lineToXY(R * Math.cos(theta), R * Math.sin(theta))
    }
  }
  path.close()
  const p = new skiaModule.SkPaint()
  p.setAntiAlias(true)
  canvas.clear(0xFFFFFFFF)//white
  canvas.translate(0.5 * scale, 0.5 * scale)
  canvas.drawPath(path, p)
}

function main (skiaModule) {
  const width = 256
  const height = 256
  const path = 'out_star.png'
  const rasterSurface = skiaModule.SkSurface.MakeRasterN32Premul(width, height, null)
  const rasterCanvas = rasterSurface.getCanvas()
  draw(rasterCanvas)

  const img = rasterSurface.makeImageSnapshot()
  if (!img) { return }
  const png = img.encodeToData()
  if (!png) { return }
  const pngBytes = skiaModule.getSkDataBytes(png)
  fs.appendFileSync(path, new Buffer(pngBytes));
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


