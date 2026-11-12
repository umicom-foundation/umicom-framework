/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/os_image/test_firmware_cli.c
 *
 * PURPOSE:
 *   Refuse partial and malformed firmware pins before any filesystem or process action.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/os_image/boot_firmware.h"
#include <stdio.h>
#include <limits.h>
#include <string.h>
#define CHECK(e) do { if (!(e)) { fprintf(stderr, "check %d: %s\n", __LINE__, #e); return 1; } } while (0)
/* Run both public entry points against the same borrowed vector. Expected
 * refusal occurs before file/process work; the valid help case exercises a
 * counted vector with no trailing sentinel. These are not image build tests. */
static int Arguments(const char *name)
{
    char *help[] = {"os-image", "--help"};
    char *command[] = {"os-image", "profile", "--file", "/unused/profile"};
    char *counted_tail[] = {"os-image", "--help", NULL};
    char **arguments = help;
    int count = 2, expected = 2;
    if (!strcmp(name, "argument-negative")) count = INT_MIN;
    else if (!strcmp(name, "argument-zero")) count = 0;
    else if (!strcmp(name, "argument-null-vector")) arguments = NULL;
    else if (!strcmp(name, "argument-null-program")) help[0] = NULL;
    else if (!strcmp(name, "argument-null-command")) help[1] = NULL;
    else if (!strcmp(name, "argument-null-option")) {
        arguments = command; count = 4; command[2] = NULL;
    } else if (!strcmp(name, "argument-null-value")) {
        arguments = command; count = 4; command[3] = NULL;
    } else if (!strcmp(name, "argument-counted-tail")) {
        arguments = counted_tail; count = 3;
    } else if (!strcmp(name, "argument-no-sentinel")) expected = 0;
    else if (!strcmp(name, "argument-missing-command")) count = 1;
    else return 2;
    CHECK(UmiOsImageMain(count, arguments) == expected);
    CHECK(UmiOsImageMainWithCancellation(count, arguments, NULL) == expected);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (!strncmp(argv[1], "argument-", 9U)) return Arguments(argv[1]);
    char hash[] = "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
    char *boot[] = {"umicom-os-image", "boot", "--bundle", "/unused/bundle",
        "--qemu", "/unused/qemu", "--mode", "normal", "--result", "/unused/result",
        "--firmware", "/unused/firmware", "--sha256", hash, "--bytes", "12"};
    if (!strcmp(argv[1], "partial")) {
        CHECK(UmiOsImageMain(12, boot) == 2);
        CHECK(UmiOsImageMain(14, boot) == 2);
    } else if (!strcmp(argv[1], "zero")) {
        boot[15] = "0";
        CHECK(UmiOsImageMain(16, boot) == 2);
    } else if (!strcmp(argv[1], "overflow")) {
        boot[15] = "18446744073709551616";
        CHECK(UmiOsImageMain(16, boot) == 2);
    } else if (!strcmp(argv[1], "uppercase")) {
        hash[0] = 'A';
        CHECK(UmiOsImageMain(16, boot) == 2);
    } else if (!strcmp(argv[1], "duplicate")) {
        boot[14] = "--sha256";
        CHECK(UmiOsImageMain(16, boot) == 2);
    } else if (!strcmp(argv[1], "null-pin")) {
        UmiOsImageReport report;
        memset(&report, 0x55, sizeof report);
        CHECK(UmiOsImageCheckFirmware(NULL, &report) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!report.completed && !report.outputCreated && !report.processLaunched);
        CHECK(UmiOsImageBootWithFirmware(NULL, NULL, NULL, UMI_OS_IMAGE_NORMAL,
            5U, NULL, NULL, &report) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!report.completed && !report.outputCreated && !report.processLaunched);

    } else if (!strcmp(argv[1], "report-status")) {
        UmiOsImageReport report;
        CHECK(UmiOsImageBoot(NULL, NULL, UMI_OS_IMAGE_NORMAL, 5U, NULL,
            NULL, &report) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(report.status == UMI_STATUS_INVALID_ARGUMENT && !report.completed);
        UmiOsImageFirmware pin = {NULL, NULL, 0};
        CHECK(UmiOsImageBootWithFirmware(NULL, NULL, &pin, UMI_OS_IMAGE_NORMAL,
            5U, NULL, NULL, &report) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(report.status == UMI_STATUS_INVALID_ARGUMENT && !report.processLaunched);
    } else return 2;
    return 0;
}
