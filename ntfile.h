
#ifndef NATIVEFILE_FUNCTIONS_H
#define NATIVEFILE_FUNCTIONS_H 1

//#define DEBUGMODE

#include <ntndk.h> // include this for its native functions and defn's
//#include "stdio.h"

BOOLEAN NtFileOpenFile(HANDLE* phRetFile, WCHAR* pwszFileName, BOOLEAN bWrite, BOOLEAN bOverwrite);
BOOLEAN NtFileOpenDirectory(HANDLE* phRetFile, WCHAR* pwszFileName, BOOLEAN bWrite, BOOLEAN bOverwrite);

BOOLEAN NtFileReadFile(HANDLE hFile, LPVOID pOutBuffer, DWORD dwOutBufferSize, DWORD* pRetReadedSize);
BOOLEAN NtFileWriteFile(HANDLE hFile, LPVOID lpData, DWORD dwBufferSize, DWORD* pRetWrittenSize);

BOOLEAN NtFileSeekFile(HANDLE hFile, LONGLONG lAmount);
BOOLEAN NtFileGetFilePosition(HANDLE hFile, LONGLONG* pRetCurrentPosition);
BOOLEAN NtFileGetFileSize(HANDLE hFile, LONGLONG* pRetFileSize);

BOOLEAN NtFileCopyFile(WCHAR* pszSrc, WCHAR* pszDst);

BOOLEAN NtFileDeleteFile(PWSTR filename);
BOOLEAN NtFileCreateDirectory(PWSTR dirname);

BOOLEAN NtFileMoveFile(IN PWSTR lpExistingFileName, IN PWSTR lpNewFileName, BOOLEAN ReplaceIfExists);

#endif