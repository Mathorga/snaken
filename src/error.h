#ifndef __SNAKEN_ERROR__
#define __SNAKEN_ERROR__

typedef enum {
    SNAKEN_ERROR_NONE = 0x00,
    SNAKEN_ERROR_FAILED_ALLOC = 0x01,
    SNAKEN_ERROR_INDEX_OUT_OF_RANGE = 0x02,
    SNAKEN_ERROR_INVALID_DIRECTION = 0x03
} snaken_error_code_t;

#endif
