/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_kernel_console.c
 * PURPOSE: Run a fixed, read-only Kernel console acceptance journey via the
 *          existing reviewed QEMU boot plan and owned QMP session.
 *
 * The command is deliberately separate from disk durability qualification.
 * The existing boot service uses a transient QEMU disk overlay (snapshot=on);
 * this acceptance checks real serial communication and the guest shell, not
 * persistent media or a graceful guest shutdown. No arbitrary guest command
 * string and no host-device path is accepted by this command.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vm_manager/boot.h"
#include "umicom/vm_manager/console_journey.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Native CLI adapter, called by command_qemu.c after Framework UTF-8 capture. */
int umi_cli_command_kernel_console(int argc, char **argv);

static void UmicomKernelConsoleHelp(void)
{
    puts("Usage: umicom qemu kernel-console --qemu ABSOLUTE_EXE\n"
         "       --directory ABSOLUTE_DIR --firmware ABSOLUTE_ELF\n"
         "       [--disk ABSOLUTE_RAW] [--timeout-ms 100..180000]\n"
         "The Framework boots one reviewed Umicom Kernel VM (initially paused),\n"
         "then verifies real guest serial echoes, the prompt, and read-only\n"
         "RAMFS/console commands. The owned QEMU process is force-stopped after\n"
         "the journey. An optional disk is opened using a temporary overlay;\n"
         "this is NOT a storage-persistence or clean-poweroff qualification.\n"
         "The host disk is never opened writable or replaced by this command.\n");
}

static int UmicomKernelConsoleCopy(char *out, size_t capacity, const char *value)
{
    if (!value || !value[0]) return 0;
    const size_t length = strlen(value);
    if (length >= capacity) return 0;
    memcpy(out, value, length + 1U);
    return 1;
}

static int UmicomKernelConsoleOptions(int argc, char **argv,
    UmiVmBootRequest *request, uint64_t *timeout)
{
    unsigned seen = 0U;
    if (UmiVmBootRequestInit(request, UMI_VM_BOOT_UMICOM_KERNEL) != UMI_STATUS_OK)
        return 0;
    *timeout = 45000U;
    if (argc < 6 || (argc & 1)) return 0;
    for (int i = 0; i < argc; i += 2) {
        const char *key = argv[i], *value = argv[i + 1];
        char *output = NULL;
        size_t capacity = 0U;
        unsigned bit = 0U;
        if (!strcmp(key, "--qemu")) {
            output = request->executable; capacity = sizeof request->executable; bit = 1U;
        } else if (!strcmp(key, "--directory")) {
            output = request->workingDirectory; capacity = sizeof request->workingDirectory; bit = 2U;
        } else if (!strcmp(key, "--firmware")) {
            output = request->firmware; capacity = sizeof request->firmware; bit = 4U;
        } else if (!strcmp(key, "--disk")) {
            output = request->disk; capacity = sizeof request->disk; bit = 8U;
        } else if (!strcmp(key, "--timeout-ms")) {
            char *end = NULL;
            bit = 16U;
            if (!value || !value[0] || value[0] < '0' || value[0] > '9') return 0;
            const unsigned long milliseconds = strtoul(value, &end, 10);
            if (!end || *end || milliseconds < 100UL || milliseconds > 180000UL) return 0;
            *timeout = (uint64_t)milliseconds;
        } else return 0;
        if (seen & bit) return 0;
        seen |= bit;
        if (output && !UmicomKernelConsoleCopy(output, capacity, value)) return 0;
    }
    if ((seen & 7U) != 7U) return 0;
    return UmiVmBootRequestValidate(request) == UMI_STATUS_OK;
}

int umi_cli_command_kernel_console(int argc, char **argv)
{
    if (argc == 1 && argv &&
        (!strcmp(argv[0], "help") || !strcmp(argv[0], "--help"))) {
        UmicomKernelConsoleHelp();
        return 0;
    }
    UmiVmBootRequest request;
    uint64_t timeout = 0U;
    if (!UmicomKernelConsoleOptions(argc, argv, &request, &timeout)) {
        fputs("Kernel console: invalid/missing/duplicate option.\n", stderr);
        UmicomKernelConsoleHelp();
        return 2;
    }
    UmiVmBootPlan *plan = NULL;
    UmiVmSession *session = NULL;
    UmiVmReport report = {0};
    UmiStatus status = UmiVmBootPlanCreate(&request, &plan);
    if (status == UMI_STATUS_OK) status = UmiVmBootReview(plan, &report);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Kernel console review failed: %s\n", report.detail);
        UmiVmBootPlanDestroy(plan);
        return 1;
    }
    /* A boot fingerprint binds the exact QEMU executable, firmware and input
     * bytes. Start revalidates it before launching the owned child process. */
    status = UmiVmBootStart(plan, report.fingerprint, &session, &report);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Kernel console launch failed: %s\n", report.detail);
        UmiVmSessionDestroy(session);
        UmiVmBootPlanDestroy(plan);
        return 1;
    }
    status = UmiVmControl(session, UMI_VM_RESUME, NULL, 0U,
        NULL, 0U, NULL, &report);
    UmiVmConsoleJourneyStatus result = UMI_VM_CONSOLE_JOURNEY_TRANSPORT_FAILURE;
    UmiVmConsoleJourneyReport evidence = {0};
    if (status == UMI_STATUS_OK) {
        /* Read-only operations already present in normal Umicom Kernel.
         * Positive evidence must be found within each command's response. */
        static const UmiVmConsoleJourneyStep steps[] = {
            {"cat /README", "Umicom Kernel development console"},
            {"ls /", "README"},
            {"mountinfo", "disk.mount-state=unused"}
        };
        result = UmiVmConsoleJourneyRunNative(session, steps,
            sizeof steps / sizeof steps[0], timeout, &evidence);
    }
    if (status == UMI_STATUS_OK)
        printf("%s\n", evidence.detail);
    else
        fprintf(stderr, "Kernel console resume failed: %s\n", report.detail);
    /* This is a supervised diagnostic stop, NOT a clean Kernel poweroff.
     * Only the owned process is touched, never all QEMU processes by name. */
    UmiVmReport stopReport = {0};
    const UmiStatus stopped = UmiVmForceStop(session, &stopReport);
    if (stopped != UMI_STATUS_OK)
        fprintf(stderr, "Kernel console cleanup failed: %s\n", stopReport.detail);
    UmiVmSessionDestroy(session);
    UmiVmBootPlanDestroy(plan);
    if (status != UMI_STATUS_OK || result != UMI_VM_CONSOLE_JOURNEY_OK ||
        stopped != UMI_STATUS_OK) return 1;
    puts("Kernel console acceptance passed. QEMU was stopped; disk persistence was not tested.");
    return 0;
}
