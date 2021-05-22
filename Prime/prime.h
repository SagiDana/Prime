#pragma once

// 0x22 means DEVICE_UNKNOWN_TYPE 
#define PRIME_IOCTL_MEMCPY ((ULONG)CTL_CODE(0x22, 0x808, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)) 

typedef struct {
    int action;
    int process_id;
    void* source;
    void* target;
    unsigned int size;
} memcpy_t;
