#pragma once

//#define _WIN32_WINNT _WIN32_WINNT_WIN7

//#include <sstream>
#pragma comment (lib, "Shlwapi.lib")
#include <Windows.h>
#include <string>
//#include <Shlwapi.h>
#include <ShlObj.h>
#include <fstream>
#include<iostream>
#include <Shlwapi.h>
#include <tlhelp32.h>
#include <string>
#include <dpapi.h>
#include <stdio.h>
//#include <stdbool.h>



#pragma comment(lib, "Crypt32")


#include <iostream>
#include <algorithm>
//#include <fstream>
//#include <SDKDDKVer.h>

using namespace std;





#define NOMINMAX
#define PRBool   int
#define PRUint32 unsigned int
#define PR_TRUE  1
#define PR_FALSE 0
#define SQLITE_OK 0
#define SQLITE_ROW 100
#define SQLITE_API

typedef enum SECItemType {
	siBuffer = 0,
	siClearDataBuffer = 1,
	siCipherDataBuffer,
	siDERCertBuffer,
	siEncodedCertBuffer,
	siDERNameBuffer,
	siEncodedNameBuffer,
	siAsciiNameString,
	siAsciiString,
	siDEROID,
	siUnsignedInteger,
	siUTCTime,
	siGeneralizedTime
};

struct SECItem {
	SECItemType type;
	unsigned char* data;
	size_t len;
};

typedef enum {
	SECWouldBlock = -2,
	SECFailure = -1,
	SECSuccess = 0
} SECStatus;

typedef struct PK11SlotInfoStr PK11SlotInfo;
typedef SECStatus(*NSS_Init) (const char*);
typedef SECStatus(*NSS_Shutdown) (void);
typedef PK11SlotInfo* (*PK11_GetInternalKeySlot) (void);
typedef void(*PK11_FreeSlot) (PK11SlotInfo*);
typedef SECStatus(*PK11_Authenticate) (PK11SlotInfo*, PRBool, void*);
typedef SECStatus(*PK11SDR_Decrypt) (SECItem*, SECItem*, void*);
typedef struct sqlite3 sqlite3;
typedef struct sqlite3_stmt sqlite3_stmt;
typedef int(SQLITE_API* fpSqliteOpen)(const char*, sqlite3**);
typedef int(SQLITE_API* fpSqlitePrepare_v2)(sqlite3*, const char*, int, sqlite3_stmt**, const char**);
typedef int(SQLITE_API* fpSqliteStep)(sqlite3_stmt*);
typedef const unsigned char* (SQLITE_API* fpSqliteColumnText)(sqlite3_stmt*, int);
typedef int (SQLITE_API* fpSqliteColumnTextint)(sqlite3_stmt*, int);
typedef int(SQLITE_API* fpSqliteFinalize)(sqlite3_stmt*);
typedef int(SQLITE_API* fpSqliteColumnByte)(sqlite3_stmt*);
typedef const void* (SQLITE_API* fpSqliteColumnBlob)(sqlite3_stmt*, int);
typedef int(SQLITE_API* fpSqliteClose)(sqlite3*);
typedef char* (*PL_Base64DecodeFunc)(const char*, PRUint32, char*);
typedef void(*SECITEM_ZfreeItemFunc)(SECItem*, PRBool);


PL_Base64DecodeFunc     PLBase64Decode = 0;
PK11_GetInternalKeySlot PK11GetInternalKeySlot = 0;
PK11_FreeSlot           PK11FreeSlot = 0;
PK11_Authenticate       PK11Authenticate = 0;
PK11SDR_Decrypt         PK11SDRDecrypt = 0;
NSS_Init                fpNSS_INIT = 0;
NSS_Shutdown            fpNSS_Shutdown = 0;
SECITEM_ZfreeItemFunc SECITEMZfreeItem;

fpSqliteOpen isqlite3_open = 0;
fpSqliteColumnByte isqlite3_column_bytes = 0;
fpSqliteColumnBlob isqlite3_column_blob = 0;
fpSqlitePrepare_v2 isqlite3_prepare_v2 = 0;
fpSqliteStep isqlite3_step = 0;
fpSqliteColumnText isqlite3_column_text = 0;
fpSqliteColumnTextint isqlite3_column_text_int = 0;
fpSqliteFinalize isqlite3_finalize = 0;
fpSqliteClose isqlite3_close = 0;
//
//#pragma warning(disable:4996)
//#define MY_ENCODING_TYPE  (PKCS_7_ASN_ENCODING | X509_ASN_ENCODING)

