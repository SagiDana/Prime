#include <ntifs.h>
#include <ntstrsafe.h>

#include "prime.h"

extern void inline disable_interrupts(void);
extern void inline enable_interrupts(void);
extern void inline get_cr3(void*);
extern void inline invalidate_tlb(void*);

#define LOG(...) do {                           \
    char buff[1024];                            \
    sprintf(buff, __VA_ARGS__);                 \
    DbgPrintEx( DPFLTR_IHVDRIVER_ID,            \
                0,                              \
                buff);                          \
}while(0)

#define COL_SIZE (16)
void hexdump(   unsigned char* buf,
                unsigned int size)
{
    unsigned int i,j;
    for (i = 0; i < size / COL_SIZE; i++){
        for (j = 0; j < COL_SIZE; j++){
            LOG("%02x ", buf[(i*COL_SIZE) + j]);
        }
        LOG("\n");
    }

    // print rest if need be.
    if (size % COL_SIZE != 0){
        for (j = 0; j < size % COL_SIZE; j++){
            LOG("%02x ", buf[(size / COL_SIZE) + j]);
        }
        LOG("\n");
    }
}

DRIVER_INITIALIZE DriverEntry;

typedef struct{
    unsigned char pad_1[0x20];
    void* LdtDescriptor;
}kprocess_t;

#define OFFSET_EPROCESS_CR3 (40)
void prime_memcpy(memcpy_t* args)
{
    NTSTATUS status = STATUS_SUCCESS;
    unsigned long long original_cr3;
    unsigned long long target_cr3;
    // void* remote_ldt_descriptor;
    // void* current_ldt_descriptor;
    PEPROCESS remote_process;
    PEPROCESS current_process;

    current_process = PsGetCurrentProcess(); // Do i need to derference it?

	status = PsLookupProcessByProcessId((HANDLE)args->process_id, &remote_process);
    if (!NT_SUCCESS(status)) return;

    hexdump((unsigned char*)current_process, 0x100);
    LOG("\n----------------------------------------\n");
    hexdump((unsigned char*)remote_process, 0x100);

    // change cr3
    // change gdt and ldt
    // flush tlb
    //
    // access memory
    //
    // restore cr3
    // restore gdt and ldt
    // flush tlb

    original_cr3 = __readcr3();
    target_cr3 = *((unsigned long long*)(((unsigned char*)remote_process) + OFFSET_EPROCESS_CR3));

    unsigned int value = 0;

    // disable_interrupts();

    __writecr3(target_cr3);
    invalidate_tlb(args->source);

    value = *((int *)args->source);

    __writecr3(original_cr3);
    invalidate_tlb(args->source);

    // enable_interrupts();

    LOG("value: %d\n", value);

    // write to target.
    *((int *)args->target) = value;

    ObDereferenceObject(remote_process);
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

    LOG("Prime: DriverDispatch()\n");

    if (irpStack->MajorFunction != IRP_MJ_DEVICE_CONTROL) goto end;

    ULONG ioControlCode = irpStack->Parameters.DeviceIoControl.IoControlCode;

    // COMMANDS:
    if (ioControlCode == PRIME_IOCTL_MEMCPY){
        LOG("Prime: PRIME_IOCTL_MEMCPY\n");

        unsigned char* buffer = Irp->AssociatedIrp.SystemBuffer;
        ULONG buffer_length = irpStack->Parameters.DeviceIoControl.InputBufferLength;
        if (buffer_length != sizeof(memcpy_t)){
            LOG("buffer_length is not fitting.\n");
            goto end;
        }

        prime_memcpy((memcpy_t*)buffer);
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

