const skia = require('./skia')

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

function main (skiaModule) {
  const skSurface = skiaModule.SkSurface.MakeRasterN32Premul(100, 100, null)
  const width = skSurface.width()
  console.log(width)
}

// make sure all native modules are ready for use before we start our main flow
const skiaModule = skia()
loadNativeModule(skiaModule).then(() => {
  main(skiaModule)
}).catch((error) => {
  console.log(error)
})


