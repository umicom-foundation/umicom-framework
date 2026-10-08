/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vm_manager/boot_cli.c
 * PURPOSE: Expose shared boot review and session controls through a small native command.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "boot_internal.h"
#include <stddef.h>

static void BootHelp(void)
{
    puts("Named profiles: umicom qemu profiles --help\n"
         "Usage: umicom qemu targets\n"
         "       umicom qemu plan|review|run --target NAME --qemu EXE --directory DIR\n"
         "         [--firmware FILE] [--kernel FILE] [--initrd FILE] [--disk RAW]\n"
         "         [--iso FILE] [--append TEXT] [--memory MIB] [--cpus COUNT]\n"
         "         [--expect SHA256]\n"
         "All paths must be absolute. plan opens nothing; review reads inputs.\n"
         "run requires the exact review fingerprint and starts PAUSED.\n"
         "Type resume, console, send TEXT, status, pause or powerdown afterward.\n"
         "quit-qemu and force-stop end the emulator, not a clean guest shutdown.\n"
         "Disk changes use temporary overlays and are discarded when QEMU exits.\n"
         "This is a serial console. ISO guests must support serial interaction.");
}

/* Offsets keep path parsing in one bounded routine. New options must be added
 * here and to the shared request validator, never passed through as raw QEMU
 * arguments that could bypass the device policy. */
typedef struct BootOption {
    const char *name;
    size_t offset;
    size_t capacity;
} BootOption;

static const BootOption BootOptions[] = {
    {"--qemu", offsetof(UmiVmBootRequest, executable), UMI_VM_PATH},
    {"--directory", offsetof(UmiVmBootRequest, workingDirectory), UMI_VM_PATH},
    {"--firmware", offsetof(UmiVmBootRequest, firmware), UMI_VM_PATH},
    {"--kernel", offsetof(UmiVmBootRequest, kernel), UMI_VM_PATH},
    {"--initrd", offsetof(UmiVmBootRequest, initrd), UMI_VM_PATH},
    {"--disk", offsetof(UmiVmBootRequest, disk), UMI_VM_PATH},
    {"--iso", offsetof(UmiVmBootRequest, iso), UMI_VM_PATH},
    {"--append", offsetof(UmiVmBootRequest, commandLine), UMI_VM_BOOT_COMMAND_LINE}
};

static int BootParse(int argc, char **argv, UmiVmBootRequest *request, const char **expected)
{
    const UmiVmBootTargetInfo *target = NULL;
    unsigned seen = 0U;
    /* Select defaults before copying options, independent of option order. */
    for (int index = 1; index < argc; index += 2) {
        if (strcmp(argv[index], "--target") == 0) {
            if (target) return 0;
            target = UmiVmBootTargetFind(argv[index + 1]);
            if (!target) return 0;
        }
    }
    if (!target || UmiVmBootRequestInit(request, target->target) != UMI_STATUS_OK) return 0;
    *expected = NULL;
    for (int index = 1; index < argc; index += 2) {
        const char *key = argv[index], *value = argv[index + 1];
        size_t option;
        for (option = 0; option < sizeof BootOptions / sizeof BootOptions[0]; ++option) {
            if (strcmp(key, BootOptions[option].name) == 0) break;
        }
        if (option < sizeof BootOptions / sizeof BootOptions[0]) {
            unsigned bit = 1U << option;
            if ((seen & bit) || strlen(value) >= BootOptions[option].capacity) return 0;
            seen |= bit;
            strcpy((char *)request + BootOptions[option].offset, value);
        } else if (strcmp(key, "--target") == 0) {
            continue;
        } else if (strcmp(key, "--expect") == 0) {
            if (*expected || !VmHash(value)) return 0;
            *expected = value;
        } else if (strcmp(key, "--memory") == 0 || strcmp(key, "--cpus") == 0) {
            unsigned bit = strcmp(key, "--memory") == 0 ? 256U : 512U;
            uint64_t number;
            if ((seen & bit) || !VmNumber(value, &number) || number > 32768U) return 0;
            seen |= bit;
            if (bit == 256U) request->memoryMiB = (unsigned)number;
            else request->processors = (unsigned)number;
        } else return 0;
    }
    return UmiVmBootRequestValidate(request) == UMI_STATUS_OK;
}

/* A numbered vector is a review aid, not a command to paste into a shell.
 * It shows exact argument boundaries even when a filename contains spaces. */
static void BootPrintPlan(const UmiVmBootPlan *plan)
{
    printf("Executable: %s\nWorking directory: %s\n", UmiVmBootPlanProgram(plan),
        UmiVmBootPlanDirectory(plan));
    for (size_t index = 0; index < UmiVmBootPlanArgumentCount(plan); ++index)
        printf("  argv[%zu] = %s\n", index + 1U, UmiVmBootPlanArgument(plan, index));
    puts("Paused start; software emulation; serial console; no guest network or host shares.");
}

/* Profile commands reuse the exact same request grammar and preview. */
int VmBootParseRequest(int argc, char **argv, UmiVmBootRequest *request, const char **expected)
{
    return BootParse(argc, argv, request, expected);
}

void VmBootPrintPlan(const UmiVmBootPlan *plan)
{
    BootPrintPlan(plan);
}

int UmiVmBootMain(int argc, char **argv)
{
    if (argc > 0 && strcmp(argv[0], "profiles") == 0)
        return VmBootProfilesMain(argc - 1, argv + 1);
    if (argc == 0 || (argc == 1 && (!strcmp(argv[0], "--help") || !strcmp(argv[0], "help")))) {
        BootHelp();
        return 0;
    }
    if (argc == 1 && strcmp(argv[0], "targets") == 0) {
        for (size_t index = 0; index < UmiVmBootTargetCount(); ++index) {
            const UmiVmBootTargetInfo *target = UmiVmBootTargetAt(index);
            printf("%s | %s | %s\n  %s\n", target->name, target->architecture,
                target->emulator, target->description);
        }
        return 0;
    }
    if (argc < 1 || (argc - 1) % 2 != 0 ||
        (strcmp(argv[0], "plan") != 0 && strcmp(argv[0], "review") != 0 && strcmp(argv[0], "run") != 0)) {
        BootHelp();
        return 2;
    }
    UmiVmBootRequest request;
    const char *expected;
    if (!BootParse(argc, argv, &request, &expected) ||
        (strcmp(argv[0], "run") == 0 ? expected == NULL : expected != NULL)) {
        fputs("Invalid, missing, duplicate or incompatible option. Use qemu --help and qemu targets.\n", stderr);
        return 2;
    }
    UmiVmBootPlan *plan = NULL;
    UmiStatus status = UmiVmBootPlanCreate(&request, &plan);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Boot plan: %s\n", UmiSetupStatusText(status));
        return 1;
    }
    BootPrintPlan(plan);
    if (strcmp(argv[0], "plan") == 0) {
        UmiVmBootPlanDestroy(plan);
        return 0;
    }
    UmiVmReport report = {0};
    UmiVmSession *session = NULL;
    status = strcmp(argv[0], "run") == 0
        ? UmiVmBootStart(plan, expected, &session, &report) : UmiVmBootReview(plan, &report);
    puts(report.detail);
    int result = status == UMI_STATUS_OK ? 0 : 1;
    if (status == UMI_STATUS_OK && session) {
        result = VmSessionConsole(session);
    } else if (status == UMI_STATUS_OK) {
        printf("Fingerprint: %s\n", report.fingerprint);
    } else {
        fprintf(stderr, "Stopped: %s\n", UmiSetupStatusText(status));
    }
    /* Plans and sessions have separate ownership. Destroying the former never
     * stops a child; the session is always closed first at this CLI boundary. */
    UmiVmSessionDestroy(session);
    UmiVmBootPlanDestroy(plan);
    return result;
}
