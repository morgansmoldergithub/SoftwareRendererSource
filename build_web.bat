call C:\Users\Morgan\Desktop\Desktop\emsdk\emsdk activate
em++ -O2 ./src/main.cpp  -s WASM=1 -o ./build_web/index.js --preload-file ./obj/@/obj -fno-rtti -fno-exceptions -s EXTRA_EXPORTED_RUNTIME_METHODS=['UTF8ToString'] -s INITIAL_MEMORY=50mb -s USE_SDL=2 
rem --closure 1 