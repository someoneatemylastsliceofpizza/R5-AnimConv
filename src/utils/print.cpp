#include <cstdarg>
#include <stdio.h>

bool is_no_verbose = false;
bool is_no_print = false;

void print(const char* format, ...) {
    if (!is_no_verbose) {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }
}