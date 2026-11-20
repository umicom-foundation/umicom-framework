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
static const char REORDERED[] = "UMICOM_BOOT_REPORT 1\n"
    "source=0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef\n"
    "reason=none\ncompleted=2\nplanned=2\nstate=ready\nmode=normal\n";

static int Capture(const char *name, const char *path)
{
    /* Both checksums were calculated independently from the literal bytes. */
    static const char goodDigest[] = "0427518fdb35fc19e2e3abbec7038b7e2111c388c3e9c109dae3e8a108a9bbc7";
    static const char reorderedDigest[] = "eca13a0fe184f6d4408b24a0d1e987f83320518f7ad5c3842a6e2ad0886615a1";
    UmiBootReportCapture output, saved;
    memset(&output, 0xA5, sizeof output);
    memcpy(&saved, &output, sizeof saved);
    if (!strcmp(name, "arguments")) {
        /* Validate explicit digest spelling even when the path does not exist. */
        CHECK(UmiBootReportInspectFile(path, NULL, goodDigest, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBootReportInspectFile(path, NULL, "", &output) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBootReportInspectFile(path, NULL, "123", &output) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBootReportInspectFile(path, NULL,
            "0427518FDB35FC19E2E3ABBEC7038B7E2111C388C3E9C109DAE3E8A108A9BBC7", &output) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBootReportInspectFile("relative.report", SOURCE, goodDigest, &output) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!memcmp(&output, &saved, sizeof output));
        return 0;
    }
    if (!strcmp(name, "missing")) {
        CHECK(UmiBootReportInspectFile(path, SOURCE, goodDigest, &output) == UMI_STATUS_NOT_FOUND);
        CHECK(!memcmp(&output, &saved, sizeof output));
        return 0;
    }

    /* One fixture input distinguishes a changed capture from a changed source.
     * Refusals must preserve every byte of the previously accepted result. */
    const char *expectedSource = SOURCE;
    const char *expectedDigest = goodDigest;
    UmiStatus expected = UMI_STATUS_OK;
    char bytes[UMI_BOOT_REPORT_MAX_BYTES + 1U];
    size_t size = sizeof GOOD - 1U;
    memcpy(bytes, GOOD, size);
    if (!strcmp(name, "valid")) {
        expectedSource = NULL;
        expectedDigest = NULL;
    } else if (!strcmp(name, "wrong-content")) {
        expectedDigest = reorderedDigest;
        expected = UMI_STATUS_INVALID_STATE;
    } else if (!strcmp(name, "wrong-source")) {
        expectedSource = "1123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
        expected = UMI_STATUS_INVALID_STATE;
    } else if (!strcmp(name, "reordered")) {
        size = sizeof REORDERED - 1U;
        memcpy(bytes, REORDERED, size);
        expectedDigest = reorderedDigest;
    } else if (!strcmp(name, "malformed")) {
        bytes[0] = 'X';
        expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (!strcmp(name, "large")) {
        size = sizeof bytes;
        memset(bytes, 'x', size);
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(name, "matching")) return 2;

    UmiOutputFile *file = NULL;
    CHECK(UmiOutputFileCreate(path, &file) == UMI_STATUS_OK);
    CHECK(UmiOutputFileWrite(file, bytes, size) == UMI_STATUS_OK);
    CHECK(UmiOutputFileClose(file) == UMI_STATUS_OK);
    UmiOutputFileDestroy(file);
    CHECK(UmiBootReportInspectFile(path, expectedSource, expectedDigest, &output) == expected);
    if (expected == UMI_STATUS_OK) {
        CHECK(output.byteCount == size);
        CHECK(!strcmp(output.captureSha256, expectedDigest ? expectedDigest : goodDigest));
        UmiBootReport baseline;
        uint32_t changes = UINT32_MAX;
        CHECK(UmiBootReportParse(GOOD, sizeof GOOD - 1U, &baseline) == UMI_STATUS_OK);
        CHECK(UmiBootReportCompare(&baseline, &output.report, &changes) == UMI_STATUS_OK && changes == 0U);
    } else CHECK(!memcmp(&output, &saved, sizeof output));
    size_t afterSize = 0U;
    unsigned char *after = FixtureRead(path, &afterSize);
    CHECK(afterSize == size && !memcmp(after, bytes, size));
    free(after);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *name = argv[1];
    char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    FixturePath(path, root, "boot.report");
    /* Exercise exact capture and legacy parsing in the same physical fixture. */
    if (!strncmp(name, "capture-", 8U)) return Capture(name + 8U, path);
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
