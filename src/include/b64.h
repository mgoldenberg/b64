#ifndef B64_H
#define B64_H

#include <stdint.h>
#include <stddef.h>

int b64_encode(const uint8_t *, const size_t, uint8_t *, const size_t);
int b64_decode(const uint8_t *, const size_t, uint8_t *, const size_t);

#endif
