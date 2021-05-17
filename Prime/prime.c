#include <ntifs.h>
#include <ntstrsafe.h>

#include "prime.h"

#define LOG(...) do {                           \
    char buff[1024*8];                          \
    sprintf(buff, __VA_ARGS__);                 \
    DbgPrintEx( DPFLTR_IHVDRIVER_ID,            \
                0,                              \
                buff);                          \
}while(0)

DRIVER_INITIALIZE DriverEntry;

typedef struct{
    unsigned char pad_1[0x20];
    void* LdtDescriptor;
}kprocess_t;

void read_memory(int process_id)
{
    NTSTATUS status = STATUS_SUCCESS;
    void* ldt_descriptor;
    PEPROCESS process;

	status = PsLookupProcessByProcessId((HANDLE)process_id, &process);
    if (!NT_SUCCESS(status)) return;

    ldt_descriptor = ((kprocess_t*)process)->LdtDescriptor;
}

NTSTATUS 
DriverDispatch(
    _In_ PDEVICE_OBJECT DeviceObject, 
    _Inout_ PIRP Irp
) 
{
	UNREFERENCED_PARAMETER(DeviceObject);
    NTSTATUS status;

	Irp->IoStatus.Status = STATUS_SUCCESS;

    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);

    // LOG("Prime: DriverDispatch()\n");

    if (irpStack->MajorFunction != IRP_MJ_DEVICE_CONTROL) goto end;

    ULONG ioControlCode = irpStack->Parameters.DeviceIoControl.IoControlCode;
    // COMMANDS:
    if (ioControlCode == PRIME_IOCTL_MEMCPY){
        // LOG("Prime: PRIME_IOCTL_MEMCPY\n");

        // unsigned char* buffer = Irp->AssociatedIrp.SystemBuffer;
        // ULONG buffer_length = irpStack->Parameters.DeviceIoControl.InputBufferLength;
        // unsigned int i;
        // for (i = 0; i < buffer_length; i++){
            // buffer[i] = 0x11;
        // }
    }

end:
    status = Irp->IoStatus.Status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return status;
}

VOID
DriverUnload(
    IN PDRIVER_OBJECT DriverObject
)
{	
    UNICODE_STRING dosDeviceName;
	RtlUnicodeStringInit(&dosDeviceName, L"\\DosDevices\\PrimeDriver");

    LOG("Prime: DriverUnload()\n");

	IoDeleteSymbolicLink(&dosDeviceName);
	IoDeleteDevice(DriverObject->DeviceObject);
}

NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT     DriverObject,
    _In_ PUNICODE_STRING    RegistryPath
)
{
    NTSTATUS status = STATUS_SUCCESS;
	UNREFERENCED_PARAMETER(RegistryPath);

	UNICODE_STRING deviceName;
	RtlUnicodeStringInit(&deviceName, L"\\Device\\PrimeDriver");

    LOG("Prime: DriverEntry()\n");

	PDEVICE_OBJECT deviceObject = NULL;
	status = IoCreateDevice(DriverObject, 
                            0, 
                            &deviceName, 
                            0x22, 
                            0, 
                            FALSE, 
                            &deviceObject);

	if (!NT_SUCCESS(status)) {
		return status;
	}

	DriverObject->MajorFunction[IRP_MJ_CREATE] = DriverDispatch;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = DriverDispatch;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DriverDispatch;
	DriverObject->DriverUnload = DriverUnload;

	UNICODE_STRING dosDeviceName;
	RtlUnicodeStringInit(&dosDeviceName, L"\\DosDevices\\PrimeDriver");

	status = IoCreateSymbolicLink(&dosDeviceName, &deviceName);

	if (!NT_SUCCESS(status)) {
		IoDeleteDevice(deviceObject);
	}

    return status;
}

