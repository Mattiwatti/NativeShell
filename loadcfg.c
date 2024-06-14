// Force the linker to add a minimal load config directory to satisfy checked kernels >= NT 6.3,
// or both loaders (ntdll and ntoskrnl, both CHK and FRE) if our own image subsystem version is >= 6.03
// Note that this file makes the entire /GS switch effectively useless. (Even more so than it already was, I mean)

#include <stdarg.h>
#include <stdint.h>
#include "precomp.h"

#ifdef _M_IX86
#if !defined(_DEBUG) || (!defined(DBG) || !DBG)
	uintptr_t __security_cookie = 0xBB40E64E;
#endif

	// Declare these (but still as extern, and unimplemented by us) becase we just nuked vcruntime.h above, which normally declares these
	extern const uintptr_t __safe_se_handler_table;
	extern const uintptr_t __safe_se_handler_count;
#else // _M_IX86
	uintptr_t __security_cookie = 0x2B992DDFA232;

#if defined(_DEBUG) || (defined(DBG) && DBG)
	void __fastcall __security_check_cookie(uintptr_t x) { UNREFERENCED_PARAMETER(x); }
	__declspec(noreturn) void __cdecl __report_rangecheckfailure(void) { __int2c(); }
#endif
#endif // _M_IX86

//
//
//

const IMAGE_LOAD_CONFIG_DIRECTORY _load_config_used =
{
	// NB: the minimum size to be allowed to load (as of Win 10 RS3) is actually slightly less, namely:
	//FIELD_OFFSET(IMAGE_LOAD_CONFIG_DIRECTORY, SEHandlerCount) + sizeof(IMAGE_LOAD_CONFIG_DIRECTORY::SEHandlerCount),

	// But a slightly bigger version, at least up to and including GuardFlags, is needed to tell the loader not to bother initializing the dumb /GS cookie. (This value is used here.)
	FIELD_OFFSET(IMAGE_LOAD_CONFIG_DIRECTORY, GuardFlags) + sizeof(ULONG), // sizeof(IMAGE_LOAD_CONFIG_DIRECTORY::GuardFlags)
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Fields TimeDateStamp through EditList
	(ULONG)(uintptr_t)&__security_cookie,	// SecurityCookie
#ifdef _M_IX86
	(ULONG)&__safe_se_handler_table,		// SEHandlerTable
	(ULONG)&__safe_se_handler_count,		// SEHandlerCount
#else
	0,		// SEHandlerTable, N/A on x64
	0,		// SEHandlerCount, N/A on x64
#endif
	0,		// GuardCFCheckFunctionPointer. CFG only, but dummy function pointers are inserted for you by the CRT otherwise, even when CFG support is disabled at compile time
	0,		// GuardCFDispatchFunctionPointer. Idem dito
	0,		// GuardCFFunctionTable
	0,		// GuardCFFunctionCount
	IMAGE_GUARD_SECURITY_COOKIE_UNUSED // GuardFlags

	/* Remaining Windows 10/11 stuff omitted */
};
