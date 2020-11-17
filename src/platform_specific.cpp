#include "platform_specific.h"

inline void open_binary_file(const char * path, FILE* & f)
{
#ifndef EMSCRIPTEN
    fopen_s(&f, path, "rb");
#else
    f = fopen(path, "rb");
#endif
}