#include <windows.h>


// -------------------- Common --------------------

#define ASSERT_TO(x, label) do{\
        if (x){             \
            goto label;     \
        }                   \
    }while(0)

// ------------------------------------------------


#define PRIME_IOCTL_MEMCPY ((ULONG)CTL_CODE(0x22, 0x808, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS))

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

    unsigned char buf[10] = {0};
    if (!DeviceIoControl(   driver, 
                            PRIME_IOCTL_MEMCPY,
                            buf, 
                            sizeof(buf), 
                            buf, 
                            sizeof(buf),
                            0, 
                            0)){
        printf("DeviceIoControl\n");
        printf("GetLastError: %d\n", GetLastError());
        return -1;
    }

    int i;
    for (i = 0; i < sizeof(buf); i++){
        printf("%d\n", buf[i]);
    }

    return 0;
}
