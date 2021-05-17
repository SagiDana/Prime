#include <ntifs.h>
#include <ntstrsafe.h>

#include "prime.h"

DRIVER_INITIALIZE DriverEntry;

// Handles a IRP request.
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

    if (irpStack->MajorFunction != IRP_MJ_DEVICE_CONTROL) goto end;

    // PVOID ioBuffer = Irp->AssociatedIrp.SystemBuffer;
	// ULONG inputLength = irpStack->Parameters.DeviceIoControl.InputBufferLength;
    ULONG ioControlCode = irpStack->Parameters.DeviceIoControl.IoControlCode;

    // COMMANDS:
    switch (ioControlCode){
        case (PRIME_IOCTL_MEMCPY):
            break;
        default:
            break;
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

	PDEVICE_OBJECT deviceObject = NULL;
	status = IoCreateDevice(DriverObject, 
                            0, 
                            &deviceName, 
                            FILE_DEVICE_UNKNOWN, 
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

