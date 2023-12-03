#include <stdint.h>

int debugf(const char *format, ...);

int roundUp(int available, uint64_t blockSize);

void print_buf(unsigned char *bf, size_t n);