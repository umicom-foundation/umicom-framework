/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_kernel_disk.c
 * PURPOSE: Native C23 command for Kernel FAT16 durability qualification.
 *
 * No PowerShell script, separate process runner, shell interpretation or
 * writable user-supplied image is involved. The VM Manager creates a new
 * checked scratch copy and owns both QEMU sessions. Framework's existing
 * process service runs the Kernel's independent read-only C23 verifier.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vm_manager/kernel_disk_qualification.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int umi_cli_command_kernel_disk(int argc, char **argv);

static void UmicomKernelDiskHelp(void)
{
    puts("Usage: umicom qemu kernel-disk --qemu ABSOLUTE_EXE\n"
         "       --firmware ABSOLUTE_ELF --directory ABSOLUTE_DIR\n"
         "       --fixture ABSOLUTE_RAW --scratch-directory NEW_ABSOLUTE_DIR\n"
         "       --verifier ABSOLUTE_C23_EXE --mode stage|finish --slot 0..7\n"
         "       [--timeout-ms 100..180000]\n"
         "Runs real Kernel Stage/Finish on a fresh copy of the known synthetic\n"
         "8-MiB FAT16 fixture. The copy remains on disk for review. A fresh\n"
         "read-only QEMU boot verifies mount refusal or success. The Kernel's\n"
         "existing C23 verifier checks every image sector and all changed bytes.\n"
         "Ordinary 'umicom qemu run' remains snapshot-only; no host drive,\n"
         "previous scratch directory or arbitrary FAT16 source is writable.\n"
         "Stage stops AFTER a verified FLUSH, not during an in-flight write.\n"
         "No repair, network access or guest/host shell command is performed.\n");
}

static int UmicomKernelDiskNumber(const char *text, unsigned long *value,
    unsigned long minimum, unsigned long maximum)
{
    if (!text || !*text || *text < '0' || *text > '9') return 0;
    char *end = NULL;
    const unsigned long parsed = strtoul(text, &end, 10);
    if (!end || *end || parsed < minimum || parsed > maximum) return 0;
    *value = parsed;
    return 1;
}

int umi_cli_command_kernel_disk(int argc, char **argv)
{
    if (argc == 1 && argv && argv[0] &&
        (!strcmp(argv[0], "help") || !strcmp(argv[0], "--help"))) {
        UmicomKernelDiskHelp();
        return 0;
    }
    if (!argv || argc < 16 || (argc & 1)) {
        fputs("Missing Kernel disk qualification option.\n", stderr);
        UmicomKernelDiskHelp();
        return 2;
    }
    UmiVmKernelDiskRequest request = {0};
    request.commandTimeoutMilliseconds = 45000U;
    unsigned seen = 0U;
    for (int i = 0; i < argc; i += 2) {
        const char *key = argv[i], *value = argv[i + 1];
        if (!key || !value || !*value) return 2;
        unsigned bit = 0U;
        if (!strcmp(key, "--qemu")) {
            bit = 1U; request.qemuExecutable = value;
        } else if (!strcmp(key, "--firmware")) {
            bit = 2U; request.firmware = value;
        } else if (!strcmp(key, "--directory")) {
            bit = 4U; request.workingDirectory = value;
        } else if (!strcmp(key, "--fixture")) {
            bit = 8U; request.fixture = value;
        } else if (!strcmp(key, "--scratch-directory")) {
            bit = 16U; request.outputDirectory = value;
        } else if (!strcmp(key, "--verifier")) {
            bit = 32U; request.verifier = value;
        } else if (!strcmp(key, "--mode")) {
            bit = 64U;
            if (!strcmp(value, "stage")) request.exercise = UMI_VM_KERNEL_DISK_STAGED;
            else if (!strcmp(value, "finish")) request.exercise = UMI_VM_KERNEL_DISK_FINISHED;
            else return 2;
        } else if (!strcmp(key, "--slot")) {
            unsigned long slot = 0UL;
            bit = 128U;
            if (!UmicomKernelDiskNumber(value, &slot, 0UL, 7UL)) return 2;
            request.blockSlot = (unsigned)slot;
        } else if (!strcmp(key, "--timeout-ms")) {
            unsigned long timeout = 0UL;
            bit = 256U;
            if (!UmicomKernelDiskNumber(value, &timeout, 100UL, 180000UL)) return 2;
            request.commandTimeoutMilliseconds = (uint64_t)timeout;
        } else {
            fprintf(stderr, "Unknown Kernel disk qualification option: %s\n", key);
            return 2;
        }
        if (seen & bit) {
            fprintf(stderr, "Duplicate Kernel disk qualification option: %s\n", key);
            return 2;
        }
        seen |= bit;
    }
    if ((seen & 255U) != 255U ||
        UmiVmKernelDiskRequestValidate(&request) != UMI_STATUS_OK) {
        fputs("Invalid Kernel disk qualification options.\n", stderr);
        UmicomKernelDiskHelp();
        return 2;
    }
    UmiVmKernelDiskReport result = {0};
    const UmiStatus status = UmiVmKernelDiskQualificationRun(&request, &result);
    fprintf(status == UMI_STATUS_OK ? stdout : stderr,
        "Kernel FAT16 qualification: %s\nphase=%d boots=%u verified=%d "
        "reader=%d original-unchanged=%d\n",
        result.details, (int)result.phase, result.bootsCompleted,
        result.stagedOrFinishedVerified, result.readerObserved,
        result.originalUnchanged);
    if (result.disk[0])
        printf("Disposable disk retained for examination: %s\n", result.disk);
    return status == UMI_STATUS_OK ? 0 : 1;
}
