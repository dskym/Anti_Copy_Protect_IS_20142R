#include <ntddk.h>

typedef unsigned int DWORD, *PDWORD;
typedef unsigned char BYTE, *PBYTE;

#define PROCNAMELEN 32
#define NT_PROCNAMELEN 16

#define FILELISTSIZE 10

#define LINK_NAME L"\\DosDevices\\dskym"
#define DEVICE_NAME L"\\Device\\InfoSec2"
#define IOCTL_TEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x4000, METHOD_BUFFERED, FILE_ANY_ACCESS)

//make same struct form with kernel.
#pragma pack(1)
typedef struct ServiceDescriptorEntry
{
	PDWORD ServiceTable;//We will overwrite here.
	PDWORD CounterTableBase;
	DWORD ServiceLimit;
	PBYTE ArgumentTable;
} SSDT;
//make struct form back again.
#pragma pack()

//make same struct form with kernel.
#pragma pack(1)
typedef struct _Node {
	WCHAR sOriginalFile[256];
	WCHAR sCopyFile[256];
}	Node;
#pragma pack()
//make struct form back again.

Node** fileList;

//set functions original form.
typedef NTSTATUS (*ZWWRITEFILE)(
	HANDLE FileHandle,
	HANDLE Event,
	PIO_APC_ROUTINE ApcRoutine,
	PVOID ApcContext,
	PIO_STATUS_BLOCK IoStatusBlock,
	PVOID Buffer,
	ULONG Length,
	PLARGE_INTEGER ByteOffset,
	PULONG Key
);

typedef NTSTATUS (*ZWCREATEFILE)(
	PHANDLE FileHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	PIO_STATUS_BLOCK IoStatusBlock,
	PLARGE_INTEGER AllocationSize,
	ULONG FileAttributes,
	ULONG ShareAccess,
	ULONG CreateDisposition,
	ULONG CreateOptions,
	PVOID EaBuffer,
	ULONG EaLength
);

typedef NTSTATUS (*ZWCLOSE)(
  IN  HANDLE Handle
);

typedef NTSTATUS(*ZWTERMINATEPROCESS)(
	HANDLE ProcessHandle,
	NTSTATUS ExitStatus
);
//set functions original forms end.

//[*(PULONG)((PUCHAR)_function+1)] means the function's offset.
//macro function that brings the function's ssdt address and return.
#define SYSTEMSERVICE(_function) KeServiceDescriptorTable.ServiceTable[*(PULONG)((PUCHAR)_function+1)]

//import KeServiceDescriptorTable.(in ntdll.dll)
__declspec(dllimport) SSDT KeServiceDescriptorTable;

DRIVER_INITIALIZE DriverEntry;
DRIVER_UNLOAD MyUnload;

ZWCREATEFILE OldZwCreateFile;
ZWWRITEFILE OldZwWriteFile;

ZWTERMINATEPROCESS OldZwTerminateProcess;

WCHAR fileName[1024];
int i = 0;

PDEVICE_OBJECT MyDevice;
UNICODE_STRING DeviceLink;
UNICODE_STRING DeviceName;

//declar function's original form.
NTSTATUS NewZwWriteFile(
	IN      HANDLE FileHandle,
	_In_opt_  HANDLE Event,
	_In_opt_  PIO_APC_ROUTINE ApcRoutine,
	_In_opt_  PVOID ApcContext,
	OUT     PIO_STATUS_BLOCK IoStatusBlock,
	IN      PVOID Buffer,
	IN      ULONG Length,
	_In_opt_  PLARGE_INTEGER ByteOffset,
	_In_opt_  PULONG Key
);

NTSTATUS NewZwCreateFile(
	OUT     PHANDLE FileHandle,
	IN      ACCESS_MASK DesiredAccess,
	IN      POBJECT_ATTRIBUTES ObjectAttributes,
	OUT     PIO_STATUS_BLOCK IoStatusBlock,
	_In_opt_  PLARGE_INTEGER AllocationSize,
	IN      ULONG FileAttributes,
	IN      ULONG ShareAccess,
	IN      ULONG CreateDisposition,
	IN      ULONG CreateOptions,
	_In_opt_  PVOID EaBuffer,
	IN      ULONG EaLength
);

NTSTATUS NewZwTerminateProcess(
	HANDLE ProcessHandle,
	NTSTATUS ExitStatus
);

NTSTATUS MyIOControl(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp
);

NTSTATUS CreateHandler(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp
);

//we can't write to memory's ssdt area. this function make we can(and can't) write to ssdt area.
void EnableWriteSSDT()
{
   __asm	
   {
      push eax
      mov eax, cr0
      and eax, 0xFFFEFFFF
      mov cr0, eax
      pop eax
   }
}

void DisableWriteSSDT()
{
   __asm
   {
      push eax
      mov eax, cr0
      or eax, 0x00010000
      mov cr0, eax
      pop eax
   }
}

NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT  DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
	int i;

	DriverObject->DriverUnload = MyUnload;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = MyIOControl;
	DriverObject->MajorFunction[IRP_MJ_CREATE] = CreateHandler;

	KdPrintEx(( DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "Driver Loaded.\n"));

	//File List limitted
	fileList = (Node**)ExAllocatePool(NonPagedPool, sizeof(Node*) * FILELISTSIZE);

	for(i=0;i<FILELISTSIZE;++i)
		fileList[i] = (Node*)ExAllocatePool(NonPagedPool, sizeof(Node));

	//driver-application communication setting.
	RtlInitUnicodeString(&DeviceLink, LINK_NAME);
	RtlInitUnicodeString(&DeviceName, DEVICE_NAME);

	//for driver-application.
	IoCreateDevice(DriverObject, 0, &DeviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &MyDevice);
	IoCreateSymbolicLink(&DeviceLink, &DeviceName);

	EnableWriteSSDT();

	//overwrite ssdt. five functions are overwritten.
	OldZwCreateFile = (ZWCREATEFILE)InterlockedExchange((PLONG)&SYSTEMSERVICE(ZwCreateFile), (LONG)NewZwCreateFile);
	OldZwWriteFile = (ZWWRITEFILE)InterlockedExchange((PLONG)&SYSTEMSERVICE(ZwWriteFile), (LONG)NewZwWriteFile);
	OldZwTerminateProcess = (ZWTERMINATEPROCESS)InterlockedExchange((PLONG)&SYSTEMSERVICE(ZwTerminateProcess), (LONG)NewZwTerminateProcess);
	
	DisableWriteSSDT();

	return STATUS_SUCCESS;
}

VOID MyUnload(_In_ PDRIVER_OBJECT DriverObject)
{
	int i;

	KdPrintEx(( DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "Driver Unload\n" ));

	//driver-application communication unset.
	IoDeleteDevice(MyDevice);
	IoDeleteSymbolicLink(&DeviceLink);
	
	//if listed file is not 0, free the files.
	if(fileList != NULL)
	{
		for(i=0;i<FILELISTSIZE;++i)
			ExFreePool(fileList[i]);

		ExFreePool(fileList);
	}

	EnableWriteSSDT();

	//restore overwritten functions to original functions.
	InterlockedExchange((PLONG)&SYSTEMSERVICE(ZwCreateFile), (LONG)OldZwCreateFile);
	InterlockedExchange((PLONG)&SYSTEMSERVICE(ZwWriteFile), (LONG)OldZwWriteFile);
	InterlockedExchange((PLONG)&SYSTEMSERVICE(ZwTerminateProcess), (LONG)OldZwTerminateProcess);

	DisableWriteSSDT();
}

//Copy 파일에 작성하고 저장하면 원본 파일에도 변경이 적용됨...
//읽기 전용 등으로 열 필요가 있을 듯?
NTSTATUS NewZwCreateFile(
	OUT     PHANDLE FileHandle,
	IN      ACCESS_MASK DesiredAccess,
	IN      POBJECT_ATTRIBUTES ObjectAttributes,
	OUT     PIO_STATUS_BLOCK IoStatusBlock,
	_In_opt_  PLARGE_INTEGER AllocationSize,
	IN      ULONG FileAttributes,
	IN      ULONG ShareAccess,
	IN      ULONG CreateDisposition,
	IN      ULONG CreateOptions,
	_In_opt_  PVOID EaBuffer,
	IN      ULONG EaLength
)
{
	NTSTATUS status;
	WCHAR str[1024];
	int i;

	status = ((ZWCREATEFILE)(OldZwCreateFile))(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock,AllocationSize, FileAttributes, ShareAccess, CreateDisposition, CreateOptions, EaBuffer, EaLength);

	//사용자가 직접 만드는 경우와 복사했을 때 만들어지는 경우 구별하기
	//복사에 의해 파일이 만들어지는 경우
	//when copy file.
	wcscpy(str, ObjectAttributes->ObjectName->Buffer);

	if(IoStatusBlock->Information & FILE_CREATED)
	{
		for(i=0;i<FILELISTSIZE;++i)
		{
			if(wcscmp(fileList[i]->sCopyFile, L"") && wcscmp(fileList[i]->sOriginalFile, L""))
			{
				if((fileList[i] != NULL) && (wcsstr(str, fileList[i]->sCopyFile) != NULL))
				{
					wcscpy(fileName, str);

					KdPrintEx(( DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "Created File name : %S.\n", fileName));

					break;
				}
			}
		}
	}
	//when copied file is opened, read original file.
	else if((IoStatusBlock->Information & FILE_OPENED) && (DesiredAccess & GENERIC_READ))
	{
		for(i=0;i<FILELISTSIZE;++i)
		{
			if(wcscmp(fileList[i]->sCopyFile, L"") && wcscmp(fileList[i]->sOriginalFile, L""))
			{
				if((fileList[i] != NULL) && (wcsstr(str, fileList[i]->sCopyFile) != NULL))
				{
					WCHAR temp[300] = L"\\??\\";

					wcscat(temp, fileList[i]->sOriginalFile);
					wcscpy(ObjectAttributes->ObjectName->Buffer, temp);

					ObjectAttributes->ObjectName->Length = wcslen(temp) * 2;
					ObjectAttributes->ObjectName->MaximumLength = wcslen(temp) * 2 + 2;

					status = ((ZWCREATEFILE)(OldZwCreateFile))(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock,AllocationSize, FileAttributes, ShareAccess, CreateDisposition, CreateOptions, EaBuffer, EaLength);

					wcscpy(fileName, ObjectAttributes->ObjectName->Buffer);

					KdPrintEx(( DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "Created File name!!! : %S.\n", fileName));

					wcscpy(ObjectAttributes->ObjectName->Buffer, str);
					ObjectAttributes->ObjectName->Length = wcslen(ObjectAttributes->ObjectName->Buffer) * 2;
					ObjectAttributes->ObjectName->MaximumLength = wcslen(ObjectAttributes->ObjectName->Buffer) * 2 + 2;

					KdPrintEx(( DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "Created File name!!! : %S.\n", ObjectAttributes->ObjectName->Buffer));
					KdPrintEx(( DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "Created File name!!! : %d.\n", ObjectAttributes->ObjectName->Length));
					KdPrintEx(( DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "Created File name!!! : %d.\n", ObjectAttributes->ObjectName->MaximumLength));

					break;
				}
			}
		}
	}

	memset(str, 0, sizeof(str));

	return status;
}

NTSTATUS NewZwWriteFile(
	IN      HANDLE FileHandle,
	_In_opt_  HANDLE Event,
	_In_opt_  PIO_APC_ROUTINE ApcRoutine,
	_In_opt_  PVOID ApcContext,
	OUT     PIO_STATUS_BLOCK IoStatusBlock,
	IN      PVOID Buffer,
	IN      ULONG Length,
	_In_opt_  PLARGE_INTEGER ByteOffset,
	_In_opt_  PULONG Key
)
{
	NTSTATUS status;
	int i;
	char dummy[] = "DUMMYTEXT";

	status = ((ZWWRITEFILE)(OldZwWriteFile))(FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, Buffer, Length, ByteOffset, Key);

	if(IoStatusBlock->Information & FILE_CREATED)
	{
		for(i=0;i<FILELISTSIZE;++i)
		{
			if(wcscmp(fileList[i]->sCopyFile, L"") && wcscmp(fileList[i]->sOriginalFile, L""))
			{
				//When read copied file, overwrite original file. 
				if((fileList[i] != NULL) && (wcsstr(fileName, fileList[i]->sOriginalFile) != NULL))
				{
					KdPrintEx(( DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "FileWrite : %S.\n", fileName));

					memset(fileName, 0, sizeof(fileName));

					break;
				}
				//When copied file is created, dummy value will be written.
				else if((fileList[i] != NULL) && (wcsstr(fileName, fileList[i]->sCopyFile) != NULL))
				{
					char* str;
					char* temp;

					str = (char*)ExAllocatePool(NonPagedPool, Length * sizeof(char));
					temp = (char*)ExAllocatePool(NonPagedPool, Length * sizeof(char));

					for(i=0;i<Length;++i)
						strncat(str, &dummy[i % strlen(dummy)], 1);

					KdPrintEx(( DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "FileWrite : %S.\n", fileName));

					KdPrintEx(( DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "FileWriteOriginalBuffer : %s.\n", (char*)Buffer));
					KdPrintEx(( DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "FileWriteOriginalBuffer : %x.\n", Buffer));
					KdPrintEx(( DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "FileWriteOriginalLength : %d.\n", Length));
					KdPrintEx(( DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "FileWriteDummyBuffer : %s.\n", str));
					KdPrintEx(( DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "FileWriteDummyBuffer : %d.\n", strlen(str)));

					memcpy(temp, Buffer, Length);
					memcpy(Buffer, str, Length);

					status = ((ZWWRITEFILE)(OldZwWriteFile))(FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, Buffer, Length, ByteOffset, Key);

					memcpy(Buffer, temp, Length);

					ExFreePool(str);
					ExFreePool(temp);

					memset(fileName, 0, sizeof(fileName));

					break;
				}
			}
		}
	}

	return status;
}

NTSTATUS MyIOControl(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp
)
{
	Node* file;
	PIO_STACK_LOCATION pStack;
	ULONG ControlCode;

	pStack = IoGetCurrentIrpStackLocation(Irp);
	ControlCode = pStack->Parameters.DeviceIoControl.IoControlCode;

	switch(ControlCode)
	{
		case IOCTL_TEST :
			KdPrintEx(( DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "IOCTL_TEST Call\n"));

			file = (Node*)Irp->AssociatedIrp.SystemBuffer;

			KdPrintEx(( DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "OriginalFile : %S\n", file->sOriginalFile));
			KdPrintEx(( DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "CopyFile : %S\n", file->sCopyFile));

			memcpy(fileList[i++], file, sizeof(Node));

			break;
	}

	Irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS CreateHandler(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp
)
{
	Irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS NewZwTerminateProcess(
	HANDLE ProcessHandle,
	NTSTATUS ExitStatus
)
{
	PEPROCESS Process;
	NTSTATUS ObjectStatus;

	char processName[16];
	char CallerProcessName[16];

	//get process EPROCESS struct that will be turminated.
	ObjectStatus = ObReferenceObjectByHandle(ProcessHandle, PROCESS_ALL_ACCESS, NULL, KernelMode, &Process, NULL);

	if (NT_SUCCESS(ObjectStatus)){
		//if success save the process' name to processName. 
		strncpy(processName, (PCHAR)Process + 0x16c, 16);

		KdPrintEx((DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "NewZwTerminateProcess!! ProcessName : %s, compare : %d.\n", processName, strcmp(processName, "Ahroobe.exe")));
	}

	//get EPROCESS struct that that terminate this process and get the process' name.
	Process = PsGetCurrentProcess();
	strncpy(CallerProcessName, (PCHAR)Process + 0x16c, 16);

	//if terminating process is our project(if we close program normally), it closes well.
	if (strcmp(CallerProcessName, "Ahroobe.exe") == 0){
		KdPrintEx((DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "Terminate Ahroobe Normal! \n"));
		return ((ZWTERMINATEPROCESS)(OldZwTerminateProcess))(ProcessHandle, ExitStatus);
	}
	//else if terminating process is another process, terminating will rejected.
	else if (NT_SUCCESS(ObjectStatus) && strcmp(processName, "Ahroobe.exe") == 0)
	{
		KdPrintEx((DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "Terminate Ahroobe! \n"));
		return STATUS_ACCESS_DENIED;
	}
	//terminated process is not our process, terminating will go well.
	else{
		KdPrintEx((DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "Terminate not Ahroobe \n"));
		return ((ZWTERMINATEPROCESS)(OldZwTerminateProcess))(ProcessHandle, ExitStatus);
	}
}