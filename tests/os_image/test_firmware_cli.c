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
#include <string.h>
#define CHECK(e) do { if (!(e)) { fprintf(stderr, "check %d: %s\n", __LINE__, #e); return 1; } } while (0)
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
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
