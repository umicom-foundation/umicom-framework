/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vm_manager/kernel_disk_qualification.h
 *
 * PURPOSE:
 *   Qualify the native Umicom Kernel's ordered FAT16 Stage/Finish protocol
 *   against an exclusively owned DISPOSABLE copy of a known synthetic disk.
 *
 * BOUNDARY:
 *   This is a hosted Framework/VM-Manager service. The freestanding Kernel
 *   does not import Framework or depend on its host process APIs. The source
 *   fixture and normal QEMU boot sessions are never opened writable.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VM_MANAGER_KERNEL_DISK_QUALIFICATION_H
#define UMICOM_VM_MANAGER_KERNEL_DISK_QUALIFICATION_H

#include "umicom/vm_manager/boot.h"
#include "umicom/vm_manager/console_journey.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiVmKernelDiskCase {
    UMI_VM_KERNEL_DISK_STAGED = 1,
    UMI_VM_KERNEL_DISK_FINISHED = 2
} UmiVmKernelDiskCase;

typedef enum UmiVmKernelDiskPhase {
    UMI_VM_KERNEL_DISK_PHASE_VALIDATE = 0,
    UMI_VM_KERNEL_DISK_PHASE_COPY,
    UMI_VM_KERNEL_DISK_PHASE_WRITER,
    UMI_VM_KERNEL_DISK_PHASE_VERIFY,
    UMI_VM_KERNEL_DISK_PHASE_READER,
    UMI_VM_KERNEL_DISK_PHASE_COMPLETE
} UmiVmKernelDiskPhase;

typedef struct UmiVmKernelDiskRequest {
    const char *qemuExecutable;    /* Absolute qemu-system-riscv64 executable. */
    const char *firmware;          /* Built normal umicom-system.elf. */
    const char *workingDirectory;  /* Existing Framework build/work directory. */
    const char *fixture;           /* Immutable, exact synthetic 8-MiB input. */
    const char *outputDirectory;   /* NEW directory; never an existing disk. */
    const char *verifier;          /* Kernel's existing C23 read-only image verifier. */
    UmiVmKernelDiskCase exercise;
    unsigned blockSlot;           /* Explicit QEMU slot, 0..7. */
    uint64_t commandTimeoutMilliseconds; /* 100..180000 per journey. */
} UmiVmKernelDiskRequest;

typedef struct UmiVmKernelDiskReport {
    UmiStatus status;
    UmiVmKernelDiskPhase phase;
    UmiVmConsoleJourneyStatus consoleStatus;
    unsigned bootsCompleted;
    int stagedOrFinishedVerified;
    int readerObserved;
    int originalUnchanged;
    char disk[UMI_VM_PATH];
    char fixtureHash[65];
    char diskAfterWriterHash[65];
    char diskAfterReaderHash[65];
    char details[512];
} UmiVmKernelDiskReport;

/* Preflight makes no disk copy and starts no process. Paths are explicit;
 * validation never silently chooses a host drive or a block-device name. */
UmiStatus UmiVmKernelDiskRequestValidate(const UmiVmKernelDiskRequest *request);

/* The caller must provide a NEW output directory, an exact known disposable
 * FAT16 fixture and a compiled read-only verifier. The implementation copies
 * the fixture with Framework's checked file service, boots only the copy,
 * stops only its owned QEMU sessions, and runs the independent C23 verifier.
 * A staged exercise deliberately force-stops after the guest's successful
 * Stage: this is NOT a simulated mid-sector power cut, and no repair is done.
 * On any failure the resulting disk/evidence are retained for inspection.
 * Execute on a worker when called by a GUI; all callbacks are synchronous. */
UmiStatus UmiVmKernelDiskQualificationRun(
    const UmiVmKernelDiskRequest *request, UmiVmKernelDiskReport *report);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_VM_MANAGER_KERNEL_DISK_QUALIFICATION_H */
