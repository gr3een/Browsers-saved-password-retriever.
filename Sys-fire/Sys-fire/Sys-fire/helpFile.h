#pragma once

#include <Shlwapi.h>
#include <Shlobj.h>
#include <string>
#include <stdio.h>
#include <tlhelp32.h>

#pragma comment (lib, "shlwapi.lib")
#pragma comment (lib, "crypt32.lib")


char* dupcat(const char* s, ...);
DWORD* FindProcessIDs(char* proc, int* cou);

