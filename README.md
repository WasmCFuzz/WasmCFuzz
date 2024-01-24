## Compile AFL++
```bash
cd /home/WasmCFuzz/
export LD_LIBRARY_PATH=/home/WasmCFuzz/WasmCFuzz/build:$LD_LIBRARY_PATH
make 
```

## Fuzz
### JSC
```bash
# Build
export CC=/home/WasmCFuzz/afl-clang-fast
export CXX=/home/WasmCFuzz/afl-clang-fast++
./Tools/Scripts/build-jsc --jsc-only --build-dir=patch/
# Run
AFL_NO_STARTUP_CALIBRATION=1 ./afl-fuzz -G 1024 -t 500 -i /home/seeds/copy_3/ -o /data/fuzzout/WAfuzz/JSC_1017 /home/WebKit/0927fuzz/Release/bin/jsc --useWebAssemblyGC=true  --useWebAssemblyTypedFunctionReferences=true --useWebAssemblyTailCalls=true  --useWebAssemblyRelaxedSIMD=true @@
```
### SpiderMonkey
```bash
# Build
export CC=/home/WasmCFuzz/afl-clang-fast
export CXX=/home/WasmCFuzz/afl-clang-fast++
cd js/src
mkdir build
cd build
../configure --disable-jemalloc --enable-debug --enable-optimize --disable-shared-js
make -j12
# Run
AFL_NO_STARTUP_CALIBRATION=1 ./afl-fuzz -G 1024 -t 500 -i /home/seeds/copy_3 -o /data/fuzzout/WAfuzz/SM_1018 /home/gecko-dev/js/src/afl/dist/bin/js  --wasm-moz-intgemm --wasm-memory-control --wasm-multi-memory --wasm-function-references --wasm-verbose --wasm-test-serialization --wasm-gc @@
```
### V8
```bash
# Build
follow https://migraine-sudo.github.io/2020/10/06/v8-Instrumentation/
# Run
AFL_NO_STARTUP_CALIBRATION=1 ./afl-fuzz -G 1024 -t 500 -i /home/seeds/copy_3/ -o /data/fuzzout/WAfuzz/V8_1018 /home/v8/out/afl/d8 --experimental-wasm-compilation-hints --experimental-wasm-instruction-tracing --experimental-wasm-gc  --experimental-wasm-js-inlining --experimental-wasm-typed-funcref --experimental-wasm-branch-hinting --experimental-wasm-type-reflection  --experimental-wasm-memory64  --experimental-wasm-inlining --experimental-wasm-stringref --experimental-wasm-stack-switching @@
```
