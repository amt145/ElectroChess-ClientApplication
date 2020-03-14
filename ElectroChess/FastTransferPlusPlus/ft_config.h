#pragma once
#include <stdint.h>
#define ARRAY_SZ 12
#define MAX_PCKT_SZ 60
#if ARRAY_SZ > 256
#error FT config: Array Size exceeds 256
#endif
