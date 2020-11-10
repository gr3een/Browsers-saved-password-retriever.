#include"helpFile.h"

char* dupcat(const char* s1, ...) {
	int len;
	char* p, * q, * sn;
	va_list ap;

	len = strlen(s1);
	va_start(ap, s1);
	while (1) 
	{
		sn = va_arg(ap, char*);
		if (!sn)
			break;
		len += strlen(sn);
	}
	va_end(ap);

	p = new char[len + 1];
	strcpy(p, s1);
	q = p + strlen(p);

	va_start(ap, s1);
	while (1)
	{
		sn = va_arg(ap, char*);
		if (!sn)
			break;
		strcpy(q, sn);
		q += strlen(q);
	}
	va_end(ap);

	return p;
}


DWORD* FindProcessIDs(char* procName, int* count) {
	PROCESSENTRY32 info;
	int e = 1;
	*count = 0;
	DWORD* ret = (DWORD*)malloc(sizeof(DWORD) * e);
	info.dwSize = sizeof(info);
	HANDLE prc = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (!prc) {
		CloseHandle(prc);
		return 0;
	}
	if (Process32First(prc, &info) != FALSE) {
		while (Process32Next(prc, &info) != 0) {
			if (!strcmp((const char*)info.szExeFile, procName) != 0) {
				ret = (DWORD*)realloc(ret, sizeof(DWORD) * e);
				ret[e - 1] = info.th32ProcessID;
				*count = e;
				e++;
			}
		}
	}
	CloseHandle(prc);
	return ret;
	//Free(ret);
}

