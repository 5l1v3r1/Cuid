
#include "Nas.h"
#include "def.h"

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
