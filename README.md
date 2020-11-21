C++ Software Renderer
=======
Simple software renderer implemented in c++, cross-compiled for the web using [Emscripten](https://emscripten.org/).

External Libraries
-----------
* [SDL 2.0](https://www.libsdl.org/download-2.0.php) - Platform Abstraction, User Input
* [STB Image](https://github.com/nothings/stb/blob/master/stb_image.h) - Texture Loading

**All rendering logic and math written from scratch.**

Building the Code - Windows
-----------
Download and install Emscripten by following the instructions [here](https://emscripten.org/docs/getting_started/downloads.html).

Open a command prompt with the Emscripten SDK environment variables active and run the file "build_web.bat". This will compile the program and generate output wasm in the "build_web" subdirectory. Use a browser to open the file "./build_web/index.html" to view the compiled site.