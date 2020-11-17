#ifndef PLATFORM_SPECIFIC_H
#define PLATFORM_SPECIFIC_H

#include <cstdio>

inline void open_binary_file(const char * path, FILE* & f);

#ifndef EMSCRIPTEN
#define FORMAT_PRINT(buf, format, buf_size, arg) sprintf_s(buf, buf_size, format, arg);
#else
#define FORMAT_PRINT(buf, format, buf_size, arg) sprintf(buf, format, arg);
#endif

#endif