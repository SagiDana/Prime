#include <windows.h>


// -------------------- Common --------------------

#define ASSERT_TO(x, label) do{\
        if (x){             \
            goto label;     \
        }                   \
    }while(0)

// ------------------------------------------------


#define PRIME_IOCTL_MEMCPY ((ULONG)CTL_CODE(0x22, 0x808, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS))

typedef struct {
    int action;
    int process_id;
    void* source;
    void* target;
    unsigned int size;
} memcpy_t;

int main()
{
    HANDLE driver;
    driver = CreateFileW(   L"\\\\.\\PrimeDriver", 
                            GENERIC_READ | GENERIC_WRITE, 
                            FILE_SHARE_READ | FILE_SHARE_WRITE, 
                            0, 
                            OPEN_EXISTING, 
                            0, 
                            0);
    if (driver == INVALID_HANDLE_VALUE) {
        printf("driver == NULL\n");
        printf("GetLastError: %d\n", GetLastError());
        return -1;
    }

    int source_process_id = 5308;
    void* source = 0x9000;

    int target = 0xdead;
    printf("before: %d\n", target);

    memcpy_t buf = {
        .process_id = source_process_id,
        .source = source,
        .target = &target,
    };

    if (!DeviceIoControl(   driver, 
                            PRIME_IOCTL_MEMCPY,
                            &buf, 
                            sizeof(buf), 
                            &buf, 
                            sizeof(buf),
                            0, 
                            0)){
        printf("DeviceIoControl\n");
        printf("GetLastError: %d\n", GetLastError());
        return -1;
    }
    printf("after: %d\n", target);
    printf("after: %d\n", *((int*)source));

    return 0;
}
