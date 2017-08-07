
#include "def.h"
#include <ntstatus.h>
#include <Winternl.h>
#include <WinBase.h>

#pragma comment(lib, "Kernel32.lib")

typedef NTSTATUS(NTAPI * TFNNtCreateFile)(
	_Out_ PHANDLE,
	_In_ ACCESS_MASK,
	_In_ POBJECT_ATTRIBUTES,
	_Out_ PIO_STATUS_BLOCK,
	_In_opt_ PLARGE_INTEGER,
	_In_ ULONG,
	_In_ ULONG,
	_In_ ULONG,
	_In_ ULONG,
	_In_ PVOID,
	_In_ ULONG
	);

typedef NTSTATUS(__stdcall *pNtCreateThreadEx)(
	_Out_ PHANDLE,
	_In_ ACCESS_MASK,
	_In_ PVOID,
	_In_ HANDLE,
	_In_ PVOID,
	_In_ PVOID,
	_In_ ULONG,
	_In_ SIZE_T,
	_In_ SIZE_T,
	_In_ SIZE_T,
	_Out_ PVOID
	);

FORCEINLINE
VOID
RtlInitUnicodeString(
	_In_ PUNICODE_STRING Dst,
	_In_ PWSTR Src
);

typedef PVOID(__stdcall *pRtlSecureZeroMemory)(
	_Out_ PVOID  ptr,
	_In_  SIZE_T cnt
	);

typedef ULONG(__stdcall *pNtCreateFile)(
	PHANDLE FileHandle,
	ULONG DesiredAccess,
	PVOID ObjectAttributes,
	PVOID IoStatusBlock,
	PLARGE_INTEGER AllocationSize,
	ULONG FileAttributes,
	ULONG ShareAccess,
	ULONG CreateDisposition,
	ULONG CreateOptions,
	PVOID EaBuffer,
	ULONG EaLength
	);

typedef ULONG(__stdcall *pNtReadFile)(
	IN HANDLE  FileHandle,
	IN HANDLE  Event  OPTIONAL,
	IN PVOID  ApcRoutine  OPTIONAL,
	IN PVOID  ApcContext  OPTIONAL,
	OUT PVOID  IoStatusBlock,
	OUT PVOID  Buffer,
	IN ULONG  Length,
	IN PLARGE_INTEGER  ByteOffset  OPTIONAL,
	IN PULONG  Key  OPTIONAL
	);

typedef NTSTATUS(__stdcall *pNtCreateKey)(
	_Out_ PHANDLE,
	_In_ ACCESS_MASK,
	_In_ POBJECT_ATTRIBUTES,
	_Reserved_ ULONG,
	_In_opt_ PUNICODE_STRING,
	_In_ ULONG,
	_Out_opt_ PULONG
	);

typedef NTSTATUS(__stdcall *pNtSetValueKey)(
	_In_ HANDLE,
	_In_ PUNICODE_STRING,
	_In_opt_ ULONG,
	_In_ ULONG,
	_In_opt_ PVOID,
	_In_ ULONG
	);

typedef NTSTATUS(__stdcall *pNtClose)(
	_In_ HANDLE
	);

#define InitializeObjectAttributes( p, n, a, r, s ) { \
(p)->Length = sizeof( OBJECT_ATTRIBUTES );          \
(p)->RootDirectory = r;                             \
(p)->Attributes = a;                                \
(p)->ObjectName = n;                                \
(p)->SecurityDescriptor = s;                        \
(p)->SecurityQualityOfService = NULL;               \
}

#define OBJ_CASE_INSENSITIVE  0x00000040L
#define FILE_NON_DIRECTORY_FILE  0×00000040
#define FILE_OPEN_BY_FILE_ID  0×00002000
#define FILE_OPEN   0×00000001

VOID
SeCreateFile(
	_In_ PCWSTR loc
);

VOID
SeWriteFile(
	_In_ PCWSTR loc,
	_In_ LPCVOID Content,
	_In_ DWORD nByte
);

BOOL
SeWriteReg(
	_In_ HKEY Root,
	_In_ LPCTSTR SubK,
	_In_ LPCTSTR lValu,
	_In_ LPCTSTR lDat
);

BOOL
SeCopyFile(
	_In_ LPCWSTR Src,
	_In_ LPCWSTR Dst
);

BOOL
SeAdjustPrivilege(
	_In_ HANDLE hToken,
	_In_ LPCTSTR lpszPrivilege,
	_In_ BOOL bEnablePrivilege
);

BOOL
SeTakeOwnership(
	_In_ LPTSTR lpszOwnFile,
	_In_ LPCTSTR oName,
	_In_ LPCTSTR nName
);

VOID 
FixBin(
	_In_ LPCTSTR Patch,
	_In_ LPCTSTR Fix,
	_In_ LPCTSTR unlock
);

VOID main(
);
