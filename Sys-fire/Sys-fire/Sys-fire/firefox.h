#pragma once

#include"helpFile.h"
#include <Windows.h>

char* installPath();
BOOL loadFunctions(char* path);
void showDecryptedPasswords();
char *  handle_credential_password(const char* pass);

