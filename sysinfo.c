/*++

Copyright (c) Alex Ionescu.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED UNDER THE LESSER GNU PUBLIC LICENSE.
    PLEASE READ THE FILE "COPYING" IN THE TOP LEVEL DIRECTORY.

Module Name:

    sysinfo.c

Abstract:

    The Native Command Line Interface (NCLI) is the command shell for the
    TinyKRNL OS.
    This module implements commands for displaying system information.

Environment:

    Native mode

Revision History:

    Alex Ionescu - Started Implementation - 23-Mar-06

--*/
#include "precomp.h"

NTSTATUS RtlCliShutdown(VOID)
{
    BOOLEAN Old;
    // Get the shutdown privilege and shutdown the system
    RtlAdjustPrivilege(SE_SHUTDOWN_PRIVILEGE, TRUE, FALSE, &Old);

    return NtShutdownSystem(ShutdownNoReboot);
}

NTSTATUS RtlCliReboot(VOID)
{
    BOOLEAN Old;
    // Get the shutdown privilege and shutdown the system
    RtlAdjustPrivilege(SE_SHUTDOWN_PRIVILEGE, TRUE, FALSE, &Old);

    return NtShutdownSystem(ShutdownReboot);
}

/*++
 * @name RtlCliPowerOff
 *
 * The RtlCliPowerOff routine FILLMEIN
 *
 * @param None.
 *
 * @return NTSTATUS
 *
 * @remarks Documentation for this routine needs to be completed.
 *
 *--*/
NTSTATUS
RtlCliPowerOff(VOID)
{
    BOOLEAN Old;
    // Get the shutdown privilege and shutdown the system
    RtlAdjustPrivilege(SE_SHUTDOWN_PRIVILEGE, TRUE, FALSE, &Old);

    return NtShutdownSystem(ShutdownPowerOff);
}

/*++
 * @name RtlCliListDrivers
 *
 * The RtlCliListDrivers routine FILLMEIN
 *
 * @param None.
 *
 * @return NTSTATUS
 *
 * @remarks Documentation for this routine needs to be completed.
 *
 *--*/
NTSTATUS
RtlCliListDrivers(VOID)
{
    PRTL_PROCESS_MODULES ModuleInfo;
    PRTL_PROCESS_MODULE_INFORMATION ModuleEntry;
    NTSTATUS Status;
    ULONG Size = 1024*1024;
    ULONG i;

    //
    // Allocate it
    ModuleInfo = RtlAllocateHeap(RtlGetProcessHeap(), HEAP_ZERO_MEMORY, Size);

    //
    // Query the buffer
    //
    Status = NtQuerySystemInformation(
        SystemModuleInformation,
        ModuleInfo,
        Size,
        NULL);
    if (!NT_SUCCESS(Status))
        return Status;
	
    //
    // Display Header
    //
    RtlCliDisplayString("*** ACTIVE MODULE LIST - DUMPING %ld MODULES\n", ModuleInfo->NumberOfModules);

    //
    // Now walk every module in it
    //
    for (i = 0; i < ModuleInfo->NumberOfModules; i++)
    {
        //
        // Check if we've displayed 20
        // BUGBUG: Should be natively handled by our display routines
        //

        if (i && !(i % 20))
        {
            //
            // Hold for more input
            //
            RtlCliDisplayString("--- PRESS SPACE TO CONTINUE ---\n");

        	while (RtlCliGetChar(hKeyboard) != ' ')
            {
            }
        }


        //
        // Get this entry
        //
        ModuleEntry = &ModuleInfo->Modules[i];

        //
        // Display basic data
        //
        RtlCliDisplayString(
            "%hs - Base: 0x%p Size: 0x%lx\n",
            ModuleEntry->FullPathName,
            ModuleEntry->ImageBase,
            ModuleEntry->ImageSize);
    }

    RtlFreeHeap(RtlGetProcessHeap(), 0, ModuleInfo);

    // Return error code
    //
    return Status;
}

/*++
 * @name RtlCliListProcesses
 *
 * The RtlCliListProcesses routine provides a way to list the current
 * processes.
 *
 * @param None.
 *
 * @return NTSTATUS
 *
 * @remarks Documentation for this routine needs to be completed.
 *
 *--*/
NTSTATUS RtlCliListProcesses(VOID)
{
    PSYSTEM_PROCESS_INFORMATION ModuleInfo;
    NTSTATUS Status;
    ULONG Size = 0x10000;

    //
    // Allocate a static buffer that should be large enough
    //
    ModuleInfo = RtlAllocateHeap(RtlGetProcessHeap(), HEAP_ZERO_MEMORY, Size);
    if (!ModuleInfo) return STATUS_INSUFFICIENT_RESOURCES;

    //
    // Query the buffer
    //
    Status = NtQuerySystemInformation(SystemProcessInformation,
                                      ModuleInfo,
                                      Size,
                                      NULL);
    if (!NT_SUCCESS(Status)) return Status;

    //
    // Display Header
    //
    RtlCliDisplayString("*** ACTIVE PROCESS LIST\n");

    //
    // Now walk every module in it
    //
    while (TRUE)
    {
        //
        // Display basic data
        //
        RtlCliDisplayString("[%lu] %ls - WS/PF/V:[%llu MB/%llu MB/%llu MB] Threads: %lu\n",
                         (ULONG)(ULONG_PTR)ModuleInfo->UniqueProcessId,
                         ModuleInfo->ImageName.Buffer,
                         (SIZE_T)ModuleInfo->WorkingSetSize / 1024ull / 1024ull,
                         (SIZE_T)ModuleInfo->PagefileUsage / 1024ull / 1024ull,
                         (SIZE_T)ModuleInfo->VirtualSize / 1024ull / 1024ull,
                         ModuleInfo->NumberOfThreads);


        //
        // Break out if we're done
        //
        if (!ModuleInfo->NextEntryOffset) break;

        //
        // Get next entry
        //
        ModuleInfo = (PSYSTEM_PROCESS_INFORMATION)((ULONG_PTR)ModuleInfo +
                                                   ModuleInfo->NextEntryOffset);
    }

    //
    // Return error code
    //
    return Status;
}

/*++
 * @name RtlCliDumpSysInfo
 *
 * The RtlCliDumpSysInfo routine queries a large amount of system information
 * and displays it on screen.
 *
 * @param None.
 *
 * @return NTSTATUS
 *
 * @remarks Documentation for this routine needs to be completed.
 *
 *--*/
NTSTATUS
RtlCliDumpSysInfo(VOID)
{
    NTSTATUS Status;
    SYSTEM_BASIC_INFORMATION BasicInfo;
    SYSTEM_PROCESSOR_INFORMATION ProcInfo;
    SYSTEM_PERFORMANCE_INFORMATION PerfInfo;
    SYSTEM_TIMEOFDAY_INFORMATION TimeInfo;
    SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION ProcPerfInfo[2];
    SYSTEM_FILECACHE_INFORMATION CacheInfo;
    PKUSER_SHARED_DATA SharedData = (PKUSER_SHARED_DATA)USER_SHARED_DATA;
    TIME_FIELDS BootTime, IdleTime, KernelTime, UserTime, DpcTime;

    //
    // Query basic system information
    //
    Status = NtQuerySystemInformation(
        SystemBasicInformation,
        &BasicInfo,
        sizeof(BasicInfo),
        NULL);
    if (!NT_SUCCESS(Status))
        return Status;

    //
    // Query basic processor information
    //
    Status = NtQuerySystemInformation(
        SystemProcessorInformation,
        &ProcInfo,
        sizeof(ProcInfo),
        NULL);

    if (!NT_SUCCESS(Status))
        return Status;

    //
    // Query basic system information
    //
    Status = NtQuerySystemInformation(
        SystemPerformanceInformation,
        &PerfInfo,
        sizeof(PerfInfo),
        NULL);

    if (!NT_SUCCESS(Status))
        return Status;

    //
    // Query basic system information
    //
    Status = NtQuerySystemInformation(
        SystemTimeOfDayInformation,
        &TimeInfo,
        sizeof(TimeInfo),
        NULL);
    if (!NT_SUCCESS(Status)) return Status;

    //
    // Query basic system information
    //
    Status = NtQuerySystemInformation(
        SystemProcessorPerformanceInformation,
        &ProcPerfInfo,
        sizeof(ProcPerfInfo),
        NULL);
    if (!NT_SUCCESS(Status)) return Status;

    //
    // Query basic system information
    //
    Status = NtQuerySystemInformation(SystemFileCacheInformation,
        &CacheInfo,
        sizeof(CacheInfo),
        NULL);
    if (!NT_SUCCESS(Status)) return Status;

    //
    // Display Header
    // FIXME: Center it
    //
    RtlTimeToTimeFields(&TimeInfo.BootTime, &BootTime);
    RtlCliDisplayString("Native shell running in %S booted on %02d-%02d-%02d "
        "at %02d:%02d. CPUs: %d\n",
        SharedData->NtSystemRoot,
        BootTime.Day,
        BootTime.Month,
        BootTime.Year,
        BootTime.Hour,
        BootTime.Minute,
        BasicInfo.NumberOfProcessors);

    //
    // Display System Flags
    //
    RtlCliDisplayString("Version: %x.%x. Debug Mode: %x. Safe Mode: %x "
        "Product Type: %x. Suite Mask: %x\n",
        SharedData->NtMajorVersion,
        SharedData->NtMinorVersion,
        SharedData->KdDebuggerEnabled,
        SharedData->SafeBootMode,
        SharedData->NtProductType,
        SharedData->SuiteMask);
    RtlCliDisplayString("--------------------------------------------------------------------------\n");

    //
    // Display CPU Information
    //
    RtlCliDisplayString("[CPU] %s Family %d Model %x Stepping %x. "
        "Feature Bits: 0x%X NX: 0x%x\n",
        (ProcInfo.ProcessorArchitecture ==
            PROCESSOR_ARCHITECTURE_INTEL) ? "x86" :
			(ProcInfo.ProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ? "AMD64" :
                (ProcInfo.ProcessorArchitecture == PROCESSOR_ARCHITECTURE_ARM ? "ARM" :
                    ProcInfo.ProcessorArchitecture == PROCESSOR_ARCHITECTURE_ARM64 ? "ARM64" : "Unknown")),
        ProcInfo.ProcessorLevel,
        ProcInfo.ProcessorRevision >> 8,
        ProcInfo.ProcessorRevision & 0xFF,
        ProcInfo.ProcessorFeatureBits,
        SharedData->NXSupportPolicy);

    //
    // Display RAM Information
    //
    RtlCliDisplayString("[RAM] Page Size: %lu KB. Physical Pages: 0x%lX. "
                        "Total Physical RAM: %llu MB\n",
                        BasicInfo.PageSize / 1024,
                        BasicInfo.NumberOfPhysicalPages,
                        ((SIZE_T)BasicInfo.NumberOfPhysicalPages * (SIZE_T)BasicInfo.PageSize) / 1024ull / 1024ull);

    //
    // Display User-Mode Virtual Memory Information
    //
    RtlCliDisplayString(
        "[USR] User-Mode Range: 0x%p-0x%p. Allocation Granularity: %d KB\n",
        (PVOID)BasicInfo.MinimumUserModeAddress,
        (PVOID)BasicInfo.MaximumUserModeAddress,
        BasicInfo.AllocationGranularity / 1024);

    //
    // Display System Virtual Memory Information
    //
    RtlCliDisplayString("[VRAM] Free: %llu MB. Committed: %llu MB. "
                        "Total: %llu MB. Peak: %llu MB\n",
                        (SIZE_T)PerfInfo.AvailablePages * (SIZE_T)PAGE_SIZE / 1024ull / 1024ull,
                        (SIZE_T)PerfInfo.CommittedPages * (SIZE_T)PAGE_SIZE / 1024ull / 1024ull,
                        (SIZE_T)PerfInfo.CommitLimit * (SIZE_T)PAGE_SIZE / 1024ull / 1024ull,
                        (SIZE_T)PerfInfo.PeakCommitment * (SIZE_T)PAGE_SIZE / 1024ull / 1024ull);

    //
    // Display Kernel Memory/Pool Information
    //
    RtlCliDisplayString("[KRNL] Paged: %llu MB. Non-Paged: %llu MB. "
                        "Drivers: %llu MB Code: %llu MB\n",
                        (SIZE_T)PerfInfo.PagedPoolPages * (SIZE_T)PAGE_SIZE / 1024ull / 1024ull,
                        (SIZE_T)PerfInfo.NonPagedPoolPages * (SIZE_T)PAGE_SIZE / 1024ull / 1024ull,
                        (SIZE_T)PerfInfo.TotalSystemDriverPages * (SIZE_T)PAGE_SIZE / 1024ull / 1024ull,
                        (SIZE_T)PerfInfo.TotalSystemCodePages * (SIZE_T)PAGE_SIZE/ 1024ull / 1024ull);

    //
    // Check if we have two CPUs
    //
    if (BasicInfo.NumberOfProcessors > 1)
    {
        //
        // Handle two CPU case by adding all of CPU 2's times into CPU 1's
        // FIXME: This should be improved to support 2+ CPUs later
        //
        ProcPerfInfo[0].IdleTime.QuadPart += ProcPerfInfo[1].IdleTime.QuadPart;
        ProcPerfInfo[0].KernelTime.QuadPart += ProcPerfInfo[1].KernelTime.QuadPart;
        ProcPerfInfo[0].UserTime.QuadPart += ProcPerfInfo[1].UserTime.QuadPart;
        ProcPerfInfo[0].DpcTime.QuadPart += ProcPerfInfo[1].DpcTime.QuadPart;
        ProcPerfInfo[0].InterruptCount += ProcPerfInfo[1].InterruptCount;
    }

    // Convert all 64-bit times into a readable format
    RtlTimeToTimeFields(&ProcPerfInfo[0].IdleTime, &IdleTime);
    RtlTimeToTimeFields(&ProcPerfInfo[0].KernelTime, &KernelTime);
    RtlTimeToTimeFields(&ProcPerfInfo[0].UserTime, &UserTime);
    RtlTimeToTimeFields(&ProcPerfInfo[0].DpcTime, &DpcTime);

    // Display System Times
    RtlCliDisplayString("[TIME] Kernel: %02d:%02d:%02d. User: %02d:%02d:%02d. "
                        "DPC: %02d:%02d:%02d. Idle: %02d:%02d:%02d.\n",
                        KernelTime.Hour, KernelTime.Minute, KernelTime.Second,
                        UserTime.Hour, UserTime.Minute, UserTime.Second,
                        DpcTime.Hour, DpcTime.Minute, DpcTime.Second,
                        IdleTime.Hour, IdleTime.Minute, IdleTime.Second);

    //
    // Display Core Performance Information
    //
    RtlCliDisplayString("[PERF] INTs: %d. SysCalls: %d. PFs: %d. "
                        "Ctx Switches: %d\n",
                        ProcPerfInfo[0].InterruptCount,
                        PerfInfo.SystemCalls,
                        PerfInfo.PageFaultCount,
                        PerfInfo.ContextSwitches);

    //
    // Display I/O Information
    //
    RtlCliDisplayString("[I/O] Reads: %d/%I64dKB. Writes: %d/%I64dKB. "
                        "Others: %d/%I64dKB\n",
                        PerfInfo.IoReadOperationCount,
                        PerfInfo.IoReadTransferCount.QuadPart / 1024,
                        PerfInfo.IoWriteOperationCount,
                        PerfInfo.IoWriteTransferCount.QuadPart / 1024,
                        PerfInfo.IoOtherOperationCount,
                        PerfInfo.IoOtherTransferCount.QuadPart / 1024);

    //
    // Display FileSystem Cache Information
    //
    Status = NtQuerySystemInformation(SystemFileCacheInformation,
                                      &CacheInfo,
                                      sizeof(CacheInfo),
                                      NULL);
    if (NT_SUCCESS(Status)) {
        RtlCliDisplayString("[CACHE] Size: %llu MB. Peak: %llu MB. "
                            "Min WS: %llu MB. Max WS: %llu MB\n",
                            CacheInfo.CurrentSize / 1024ull / 1024ull,
                            CacheInfo.PeakSize / 1024ull / 1024ull,
                            (CacheInfo.MinimumWorkingSet << 12ull) / 1024ull / 1024ull,
                            (CacheInfo.MaximumWorkingSet << 12ull) / 1024ull / 1024ull);
    }
    
    //
    // Return success
    //
    return STATUS_SUCCESS;
}
