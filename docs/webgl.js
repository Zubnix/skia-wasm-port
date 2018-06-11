'use strict'

//TODO port sdl example instead
// TODO port types: SkTArray, SkRect, SkMatrix

const helpMessage = 'Click and drag to create rects.'
const state = {
  fRects: [],
  skiaModule: null,
  canvas: null,
  paint: null,
  image: null,
  width: window.innerWidth,
  height: window.innerHeight,
  rotation: 0
}

async function loadFont () {
  const {skiaModule, paint} = state

  const request = new XMLHttpRequest()
  request.open('GET', `${window.location}/LiberationMono-Regular.ttf`, true)
  request.responseType = 'arraybuffer'

  const fontData = await new Promise((resolve) => {
    request.onload = () => {
      resolve(new Uint8Array(request.response))
    }
    request.send()
  })

  const nativeData = skiaModule.SkData.MakeUninitialized(fontData.byteLength)
  const nativeBuffer = skiaModule.getSkDataBytes(nativeData)
  nativeBuffer.set(fontData)
  const liberationMonoRegular = skiaModule.SkFontMgr.RefDefault().makeFromData(nativeData, 0)
  paint.setTextSize(48.0)
  paint.setColor(0xff4281A4)
  paint.setTypeface(liberationMonoRegular)
}

function create_star () {
  const {skiaModule} = state
  const concavePath = new skiaModule.SkPath()
  const points = [
    skiaModule.SkPoint.Make(0, -50),
    skiaModule.SkPoint.Make(0, 0),
    skiaModule.SkPoint.Make(0, 0),
    skiaModule.SkPoint.Make(0, 0),
    skiaModule.SkPoint.Make(0, 0)
  ]
  const kNumPoints = points.length
  const rot = new skiaModule.SkMatrix()
  rot.setRotate(360 / kNumPoints)
  for (let i = 1; i < kNumPoints; ++i) {
    rot.mapPoints(points[i], points[i - 1], 1)
  }
  concavePath.moveToPoint(points[0])
  for (let i = 0; i < kNumPoints; ++i) {
    concavePath.lineToPoint(points[(2 * i) % kNumPoints])
  }
  concavePath.setFillType(skiaModule.SkPath.FillType.kEvenOdd_FillType)
  concavePath.close()
  return concavePath
}

async function main (skiaModule) {
  state.skiaModule = skiaModule

  const canvasElement = document.createElement('canvas')
  const {width, height} = state
  canvasElement.id = 'somecanvas'
  canvasElement.width = width
  canvasElement.height = height
  document.body.appendChild(canvasElement)

  const surface = skiaModule.makeWebGLSurface(canvasElement.id, width, height)

  // skia canvas
  const canvas = surface.getCanvas()
  state.canvas = canvas

  const paint = new skiaModule.SkPaint()
  state.paint = paint
  paint.setAntiAlias(true)

  await loadFont()

  // create a surface for CPU rasterization
  const cpuSurface = skiaModule.SkSurface.MakeRasterN32Premul(width, height, null)
  const offscreen = cpuSurface.getCanvas()
  offscreen.save()
  offscreen.translate(501, 50)
  offscreen.drawPath(create_star(), paint)
  offscreen.restore()

  state.image = cpuSurface.makeImageSnapshot()

  canvasElement.addEventListener('mousemove', (event) => {
    if (event.buttons !== 0) {
      const rect = state.fRects[state.fRects.length - 1]
      rect.fRight = event.clientX
      rect.fBottom = event.clientY
    }
  })
  canvasElement.addEventListener('mousedown', (event) => {
    if (event.buttons !== 0) {
      state.fRects.push(skiaModule.SkRect.MakeLTRB(event.clientX,
        event.clientY,
        event.clientX,
        event.clientY))
    }
  })

  scheduleAnimation()
}

function scheduleAnimation () {
  const {skiaModule, canvas, paint, image, width, height} = state
  window.requestAnimationFrame(() => {
    const rand = new skiaModule.SkRandom()
    canvas.clear(0xFFFFFFFF)//white

    paint.setColor(0xff4281A4)
    canvas.drawText(helpMessage, 50, 50, paint)

    for (let i = 0; i < state.fRects.length; i++) {
      paint.setColor((rand.nextU() & 0x7fffffff) | 0x44808080)
      canvas.drawRect(state.fRects[i], paint)
    }

    // draw offscreen canvas
    canvas.save()
    canvas.translate(width / 2.0, height / 2.0)
    canvas.rotate(state.rotation++)
    canvas.drawImage(image, -50, -50, null)
    canvas.restore()

    canvas.flush()
    scheduleAnimation()
  })
}

function loadNativeModule (skiaModule) {
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