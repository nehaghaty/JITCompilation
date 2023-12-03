#include <sys/types.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

int debugf(const char *format, ...) {
    int ret = 0;
#ifdef DBG_TO_STDOUT
    va_list args;
    va_start(args, format);

    ret = vprintf(format, args);

    va_end(args);
#endif
    return ret;
}

size_t roundUp(size_t available, size_t blockSize) {
    size_t remainder = available % blockSize;
    size_t paddingSize = (remainder == 0) ? 0 : (blockSize - remainder);
    return available + paddingSize;
}

void print_buf(unsigned char *bf, size_t n){
    return;
    for (size_t i = 0; i < n; i++) {
        if (i < 32 || i >= (n-32)) {
            debugf("[%02X]", *(bf + i));
            if ((i+1) % 32 == 0) {
                debugf("\n");
            } 
        }
    }
    debugf("\n");
}