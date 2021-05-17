#include <windows.h>
#include <stdio.h>

#pragma comment (lib, "Advapi32.lib")

// -------------------- Common --------------------

#define ASSERT_TO(x, label) do{\
        if (x){             \
            goto label;     \
        }                   \
    }while(0)

// ------------------------------------------------

int install_driver( SC_HANDLE h_service_manager,
                    unsigned char* driver_name,
                    unsigned char* driver_path)
{
    int status = 0;
    int err;

    SC_HANDLE h_service = NULL;

    h_service = CreateService(  h_service_manager,
                                driver_name,
                                driver_name,
                                SERVICE_ALL_ACCESS,
                                SERVICE_KERNEL_DRIVER,
                                SERVICE_DEMAND_START,
                                SERVICE_ERROR_NORMAL,
                                driver_path,
                                NULL,
                                NULL,
                                NULL,
                                NULL,
                                NULL);
    if (h_service != NULL){
        CloseServiceHandle(h_service);
        goto end;
    }

    err = GetLastError();
    if (err == ERROR_SERVICE_EXISTS){
        goto end;
    }

    // failed
    status = -1;

end:
    return status;
}

int uninstall_driver(   SC_HANDLE h_service_manager,
                        unsigned char* driver_name)
{
    int status = 0;
    SC_HANDLE h_service;

    h_service = OpenService(h_service_manager,
                            driver_name,
                            SERVICE_ALL_ACCESS);
    ASSERT_TO(h_service == NULL, failed);

    ASSERT_TO(  !DeleteService(h_service),
                failed_delete);

    CloseServiceHandle(h_service);
end:
    return status;
failed_delete:
    CloseServiceHandle(h_service);
failed:
    status = -1;
}

int start_driver(SC_HANDLE h_service_manager,
                unsigned char* driver_name)
{
    int err;
    int status = 0;
    SC_HANDLE h_service = NULL;

    h_service = OpenService(h_service_manager,
                            driver_name,
                            SERVICE_ALL_ACCESS);
    ASSERT_TO(h_service == NULL, failed);

    if (!StartService(h_service, 0, NULL)){
        err = GetLastError();
        ASSERT_TO(err != ERROR_SERVICE_ALREADY_RUNNING, failed);
    }

    if (h_service){
        CloseServiceHandle(h_service);
    }

end:
    return status;

failed:
    printf("GetLastError: %d\n", err);
    status = -1;
    goto end;
}

int stop_driver(SC_HANDLE h_service_manager,
                unsigned char* driver_name)
{
    int err;
    int status = 0;
    SC_HANDLE h_service = NULL;
    SERVICE_STATUS service_status;

    h_service = OpenService(h_service_manager,
                            driver_name,
                            SERVICE_ALL_ACCESS);
    ASSERT_TO(h_service == NULL, failed);

    ASSERT_TO(  !ControlService(h_service, SERVICE_CONTROL_STOP, &service_status), 
                failed_stoping);

    CloseServiceHandle(h_service);

end:
    return status;

failed_stoping:
    CloseServiceHandle(h_service);
failed:
    status = -1;
    goto end;
}

int main(int argc, char* argv[])
{
    if (argc < 3){
        printf("manager.exe install <driver_name> <driver_path>\n");
        printf("manager.exe uninstall <driver_name>\n");
        printf("manager.exe start <driver_name>\n");
        printf("manager.exe stop <driver_name>\n");
        return 0;
    }

    SC_HANDLE h_service_manager = NULL;
    h_service_manager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    ASSERT_TO(h_service_manager == NULL, end);

    printf("argv[0]: %s\n", argv[0]);
    printf("argv[1]: %s\n", argv[1]);
    printf("argv[2]: %s\n", argv[2]);
    if (argc == 4) printf("argv[3]: %s\n", argv[3]);

    if (strcmp("install", argv[1]) == 0){
        ASSERT_TO(  install_driver( h_service_manager,
                                    argv[2],
                                    argv[3]) != 0,
                    failed);
    }
    if (strcmp("uninstall", argv[1]) == 0){
        ASSERT_TO(  uninstall_driver(   h_service_manager,
                                        argv[2]) != 0,
                    failed);
    }
    if (strcmp("start", argv[1]) == 0){
        ASSERT_TO(  start_driver( h_service_manager,
                                  argv[2]) != 0,
                    failed);
    }
    if (strcmp("stop", argv[1]) == 0){
        ASSERT_TO(  stop_driver(  h_service_manager,
                                  argv[2]) != 0,
                    failed);
    }

end_close_service_manager:
    CloseServiceHandle(h_service_manager);
end:
    return 0;

failed:
    printf("failed\n");
    goto end_close_service_manager;
}
