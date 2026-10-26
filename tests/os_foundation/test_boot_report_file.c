/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/os_foundation/test_boot_report_file.c
 *
 * PURPOSE:
 *   Check bounded report-file admission, source matching and unchanged rejected output.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/boot_report_file.h"
#include "../build_log/fixture.h"

static const char SOURCE[] = "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
static const char GOOD[] = "UMICOM_BOOT_REPORT 1\nmode=normal\nstate=ready\nplanned=2\n"
    "completed=2\nreason=none\nsource=0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef\n";

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *name = argv[1];
    char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    FixturePath(path, root, "boot.report");
    UmiBootReport report, saved;
    memset(&report, 0xA5, sizeof report);
    memcpy(&saved, &report, sizeof saved);
    if (!strcmp(name, "arguments")) {
        CHECK(UmiBootReportLoad(path, NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBootReportLoad(path, "short", &report) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBootReportLoad(path, "ABCDEF0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcd", &report) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBootReportLoad(NULL, NULL, &report) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBootReportLoad("relative", NULL, &report) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!memcmp(&report, &saved, sizeof report));
        return 0;
    }
    if (!strcmp(name, "missing")) {
        CHECK(UmiBootReportLoad(path, NULL, &report) == UMI_STATUS_NOT_FOUND);
        CHECK(!memcmp(&report, &saved, sizeof report));
        return 0;
    }
    if (!strcmp(name, "directory")) {
        CHECK(UmiBootReportLoad(root, NULL, &report) != UMI_STATUS_OK);
        CHECK(!memcmp(&report, &saved, sizeof report));
        return 0;
    }
    char bytes[UMI_BOOT_REPORT_MAX_BYTES + 1U];
    size_t size = sizeof GOOD - 1U;
    memcpy(bytes, GOOD, size);
    UmiStatus expected = UMI_STATUS_OK;
    const char *identity = NULL;
    if (!strcmp(name, "matching")) identity = SOURCE;
    else if (!strcmp(name, "mismatch")) {
        identity = "1123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
        expected = UMI_STATUS_INVALID_STATE;
    } else if (!strcmp(name, "large")) {
        memset(bytes, 'x', sizeof bytes); size = sizeof bytes;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (!strcmp(name, "malformed")) {
        bytes[0] = 'X'; expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (!strcmp(name, "embedded-nul")) {
        bytes[30] = 0; expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (!strcmp(name, "empty")) {
        size = 0; expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(name, "valid")) return 2;
    UmiOutputFile *file = NULL;
    CHECK(UmiOutputFileCreate(path, &file) == UMI_STATUS_OK);
    CHECK(UmiOutputFileWrite(file, bytes, size) == UMI_STATUS_OK);
    CHECK(UmiOutputFileClose(file) == UMI_STATUS_OK);
    UmiOutputFileDestroy(file);
    CHECK(UmiBootReportLoad(path, identity, &report) == expected);
    if (expected != UMI_STATUS_OK) CHECK(!memcmp(&report, &saved, sizeof report));
    else {
        CHECK(report.state == UMI_BOOT_REPORT_READY && report.completed == 2);
        CHECK(!strcmp(report.sourceId, SOURCE));
    }
    size_t afterSize = 0;
    unsigned char *after = FixtureRead(path, &afterSize);
    CHECK(afterSize == size && !memcmp(after, bytes, size));
    free(after);
    return 0;
}
