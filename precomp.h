/*++

Copyright (c) Alex Ionescu.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED UNDER THE LESSER GNU PUBLIC LICENSE.
    PLEASE READ THE FILE "COPYING" IN THE TOP LEVEL DIRECTORY.

Module Name:

    precomp.h

Abstract:

    The Native Command Line Interface (NCLI) is the command shell for the
    TinyKRNL OS.

Environment:

    Native mode

Revision History:

    Alex Ionescu - Started Implementation - 23-Mar-06

--*/
#pragma once

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#define WIN32_NO_STATUS
#define NTOS_MODE_USER

#include <stdarg.h>
#include <excpt.h>
#include <windef.h>
#include <winnt.h>
#include <ntddkbd.h>
#include "ntfile.h"
#include "ntreg.h"

#include "ndk/umtypes.h"
#include "ndk/pstypes.h"
#include "ndk/extypes.h"
#include "ndk/iofuncs.h"
#include "ndk/cmfuncs.h"
#include "ndk/psfuncs.h"
#include "ndk/obfuncs.h"
#include "ndk/kefuncs.h"
#include "ndk/exfuncs.h"
#include "ndk/rtlfuncs.h"

#if defined(__RESHARPER__)
#define PRINTF_ATTR(FormatIndex, FirstToCheck) \
	[[gnu::format(printf, FormatIndex, FirstToCheck)]]
#elif defined(__GNUC__)
#define PRINTF_ATTR(FormatIndex, FirstToCheck) \
	__attribute__((format(printf, FormatIndex, FirstToCheck)))
#else
#define PRINTF_ATTR(FormatIndex, FirstToCheck)
#endif

//
// Device type for input/output
//
typedef enum _CON_DEVICE_TYPE
{
    KeyboardType,
    MouseType
} CON_DEVICE_TYPE;

//
// Display functions
//
PRINTF_ATTR(1, 2)
NTSTATUS
__cdecl
RtlCliDisplayString(
    IN PCH Message,
    ...
);

NTSTATUS
RtlCliPrintString(
    IN PUNICODE_STRING Message
);

NTSTATUS
RtlCliPutChar(
    IN WCHAR Char
);

//
// Input functions
//
NTSTATUS
RtlCliOpenInputDevice(
    OUT PHANDLE Handle,
    IN CON_DEVICE_TYPE Type
);

CHAR
RtlCliGetChar(
    IN HANDLE hDriver
);

PCHAR
RtlCliGetLine(
    IN HANDLE hDriver
);

//
// System information functions
//
NTSTATUS
RtlCliListDrivers(
    VOID
);

NTSTATUS
RtlCliListProcesses(
    VOID
);

NTSTATUS
RtlCliDumpSysInfo(
    VOID
);

NTSTATUS
RtlCliShutdown(
    VOID
);

NTSTATUS
RtlCliReboot(
    VOID
);

NTSTATUS
RtlCliPowerOff(
    VOID
);

//
// Hardware functions
//
NTSTATUS RtlCliListHardwareTree(VOID);

//
// File functions
//
NTSTATUS RtlCliListDirectory(VOID);

NTSTATUS RtlCliSetCurrentDirectory(PCHAR Directory);

ULONG RtlCliGetCurrentDirectory(IN OUT PWSTR CurrentDirectory);

// Keyboard:

HANDLE hKeyboard;

typedef struct _KBD_RECORD {
    WORD  wVirtualScanCode;
    DWORD dwControlKeyState;
    UCHAR AsciiChar;
    BOOL  bKeyDown;
} KBD_RECORD, * PKBD_RECORD;

void IntTranslateKey(PKEYBOARD_INPUT_DATA InputData, KBD_RECORD* kbd_rec);

#define RIGHT_ALT_PRESSED     0x0001 // the right alt key is pressed.
#define LEFT_ALT_PRESSED      0x0002 // the left alt key is pressed.
#define RIGHT_CTRL_PRESSED    0x0004 // the right ctrl key is pressed.
#define LEFT_CTRL_PRESSED     0x0008 // the left ctrl key is pressed.
#define SHIFT_PRESSED         0x0010 // the shift key is pressed.
#define NUMLOCK_ON            0x0020 // the numlock light is on.
#define SCROLLLOCK_ON         0x0040 // the scrolllock light is on.
#define CAPSLOCK_ON           0x0080 // the capslock light is on.
#define ENHANCED_KEY          0x0100 // the key is enhanced.

// Process:

NTSTATUS CreateNativeProcess(IN PWSTR file_name, IN PWSTR cmd_line, OUT PHANDLE hProcess);

#define BUFFER_SIZE 1024

// Command processing:

PRINTF_ATTR(1, 2)
VOID
__cdecl
Printf(
    _In_ PCCH Format,
    _In_ ...
);

UINT StringToArguments(CHAR *str);

char *xargv[BUFFER_SIZE];
unsigned int xargc;

BOOL GetFullPath(IN PSTR filename, OUT PWSTR out, IN BOOL add_slash);
BOOL FileExists(PWSTR fname);

// Registry

NTSTATUS OpenKey(OUT PHANDLE pHandle, IN PWCHAR key);
NTSTATUS RegWrite(HANDLE hKey, INT type, PWCHAR key_name, PVOID data, DWORD size);

NTSTATUS RegReadValue(HANDLE hKey, PWCHAR key_name, OUT PULONG type, OUT PVOID data, IN ULONG buf_size, OUT PULONG out_size);

// Misc

void FillUnicodeStringWithAnsi(OUT PUNICODE_STRING us, IN PCHAR as);

//===========================================================
//
// Helper Functions for ntreg.c
//
//===========================================================

BOOLEAN SetUnicodeString(
    UNICODE_STRING* pustrRet,
    WCHAR* pwszData
);

BOOLEAN DisplayString(WCHAR* pwszData
);

HANDLE InitHeapMemory(VOID);

BOOLEAN DeinitHeapMemory(
    HANDLE hHeap
);

PVOID kmalloc(
    HANDLE hHeap,
    int nSize
);

BOOLEAN kfree(
    HANDLE hHeap,
    PVOID pMemory
);

BOOLEAN AppendString(
    WCHAR* pszInput,
    WCHAR* pszAppend
);

ULONG GetStringLength(
    WCHAR* pszInput
);

//
// TEMP: missing CRT and misc. other stuff
//

NTSYSAPI int __CRTDECL sprintf(
    _Pre_notnull_ _Always_(_Post_z_) char* const _Buffer,
    _In_z_ _Printf_format_string_    char const* const _Format,
    ...);

NTSYSAPI int __CRTDECL swprintf(
    _Out_writes_opt_(_BufferCount) _Always_(_Post_z_) wchar_t* const _Buffer,
    _In_ size_t const _BufferCount,
    _In_z_ _Printf_format_string_ wchar_t const* const _Format,
    ...);

NTSYSAPI int __CRTDECL _vsnprintf(
    char* buffer,
    size_t count,
    const char* format,
    va_list argptr
);

#define SE_MIN_WELL_KNOWN_PRIVILEGE (2L)
#define SE_CREATE_TOKEN_PRIVILEGE (2L)
#define SE_ASSIGNPRIMARYTOKEN_PRIVILEGE (3L)
#define SE_LOCK_MEMORY_PRIVILEGE (4L)
#define SE_INCREASE_QUOTA_PRIVILEGE (5L)
#define SE_MACHINE_ACCOUNT_PRIVILEGE (6L)
#define SE_TCB_PRIVILEGE (7L)
#define SE_SECURITY_PRIVILEGE (8L)
#define SE_TAKE_OWNERSHIP_PRIVILEGE (9L)
#define SE_LOAD_DRIVER_PRIVILEGE (10L)
#define SE_SYSTEM_PROFILE_PRIVILEGE (11L)
#define SE_SYSTEMTIME_PRIVILEGE (12L)
#define SE_PROF_SINGLE_PROCESS_PRIVILEGE (13L)
#define SE_INC_BASE_PRIORITY_PRIVILEGE (14L)
#define SE_CREATE_PAGEFILE_PRIVILEGE (15L)
#define SE_CREATE_PERMANENT_PRIVILEGE (16L)
#define SE_BACKUP_PRIVILEGE (17L)
#define SE_RESTORE_PRIVILEGE (18L)
#define SE_SHUTDOWN_PRIVILEGE (19L)
#define SE_DEBUG_PRIVILEGE (20L)
#define SE_AUDIT_PRIVILEGE (21L)
#define SE_SYSTEM_ENVIRONMENT_PRIVILEGE (22L)
#define SE_CHANGE_NOTIFY_PRIVILEGE (23L)
#define SE_REMOTE_SHUTDOWN_PRIVILEGE (24L)
#define SE_UNDOCK_PRIVILEGE (25L)
#define SE_SYNC_AGENT_PRIVILEGE (26L)
#define SE_ENABLE_DELEGATION_PRIVILEGE (27L)
#define SE_MANAGE_VOLUME_PRIVILEGE (28L)
#define SE_IMPERSONATE_PRIVILEGE (29L)
#define SE_CREATE_GLOBAL_PRIVILEGE (30L)
#define SE_TRUSTED_CREDMAN_ACCESS_PRIVILEGE (31L)
#define SE_RELABEL_PRIVILEGE (32L)
#define SE_INC_WORKING_SET_PRIVILEGE (33L)
#define SE_TIME_ZONE_PRIVILEGE (34L)
#define SE_CREATE_SYMBOLIC_LINK_PRIVILEGE (35L)
#define SE_MAX_WELL_KNOWN_PRIVILEGE SE_CREATE_SYMBOLIC_LINK_PRIVILEGE
