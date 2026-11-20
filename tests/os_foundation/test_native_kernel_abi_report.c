/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/os_foundation/test_native_kernel_abi_report.c
 *
 * PURPOSE:
 *   Verify complete hosted admission of native Kernel ABI frames and files.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/native_kernel_abi_report.h"
#include "umicom/platform/input_file.h"
#include "../build_log/fixture.h"

#define FRAME(version, word, order, page, copy, calls, features, nl) \
    "abi.version=" version nl "abi.word-bits=" word nl "abi.byte-order=" order nl \
    "abi.page-bytes=" page nl "abi.copy-limit=" copy nl "abi.call-limit=" calls nl \
    "abi.compiled-features=" features nl
#define START "UMICOM_NATIVE_ABI_REPORT 1"
#define FINISH "UMICOM_NATIVE_ABI_END"
#define CURRENT(nl) FRAME("1.0", "64", "1", "4096", "64", "32", "31", nl)

static const char current[] = START "\n" CURRENT("\n") FINISH "\n";

typedef struct ParseCase {
    const char *name;
    const char *bytes;
    size_t size;
    UmiStatus expected;
} ParseCase;

#define CASE(name, text, status) {name, text, sizeof(text) - 1U, status}
/* Spellings follow actual serial producer fields. Original synthetic values
 * explore limits without pretending to observe a guest capability. */
static const ParseCase records[] = {
    CASE("lf", START "\n" CURRENT("\n") FINISH "\n", UMI_STATUS_OK),
    CASE("crlf", START "\r\n" CURRENT("\r\n") FINISH "\r\n", UMI_STATUS_OK),
    CASE("future-abi", START "\n" FRAME("65535.65535", "32", "2", "8192", "0", "8", "18446744073709551615", "\n") FINISH "\n", UMI_STATUS_OK),
    CASE("future-schema", "UMICOM_NATIVE_ABI_REPORT 2\n" CURRENT("\n") FINISH "\n", UMI_STATUS_UNAVAILABLE),
    CASE("schema-zero", "UMICOM_NATIVE_ABI_REPORT 0\n" CURRENT("\n") FINISH "\n", UMI_STATUS_PARSE_ERROR),
    CASE("missing-header", CURRENT("\n") FINISH "\n", UMI_STATUS_PARSE_ERROR),
    CASE("missing-footer", START "\n" CURRENT("\n"), UMI_STATUS_PARSE_ERROR),
    CASE("missing-newline", START "\n" CURRENT("\n") FINISH, UMI_STATUS_PARSE_ERROR),
    CASE("command-echo", "abi --record\n" START "\n" CURRENT("\n") FINISH "\n", UMI_STATUS_PARSE_ERROR),
    CASE("trailing-prompt", START "\n" CURRENT("\n") FINISH "\n> ", UMI_STATUS_PARSE_ERROR),
    CASE("major-zero", START "\n" FRAME("0.0", "64", "1", "4096", "64", "32", "31", "\n") FINISH "\n", UMI_STATUS_PARSE_ERROR),
    CASE("version-overflow", START "\n" FRAME("1.65536", "64", "1", "4096", "64", "32", "31", "\n") FINISH "\n", UMI_STATUS_PARSE_ERROR),
    CASE("version-dots", START "\n" FRAME("1.0.2", "64", "1", "4096", "64", "32", "31", "\n") FINISH "\n", UMI_STATUS_PARSE_ERROR),
    CASE("words", START "\n" FRAME("1.0", "63", "1", "4096", "64", "32", "31", "\n") FINISH "\n", UMI_STATUS_PARSE_ERROR),
    CASE("byte-order", START "\n" FRAME("1.0", "64", "0", "4096", "64", "32", "31", "\n") FINISH "\n", UMI_STATUS_PARSE_ERROR),
    CASE("page-power", START "\n" FRAME("1.0", "64", "1", "4095", "64", "32", "31", "\n") FINISH "\n", UMI_STATUS_PARSE_ERROR),
    CASE("call-budget", START "\n" FRAME("1.0", "64", "1", "4096", "64", "0", "31", "\n") FINISH "\n", UMI_STATUS_PARSE_ERROR),
    CASE("overflow", START "\n" FRAME("1.0", "64", "1", "4096", "64", "32", "18446744073709551616", "\n") FINISH "\n", UMI_STATUS_PARSE_ERROR),
    CASE("leading-zero", START "\n" FRAME("1.0", "64", "1", "04096", "64", "32", "31", "\n") FINISH "\n", UMI_STATUS_PARSE_ERROR),
    CASE("embedded-nul", START "\n" FRAME("1.0", "64", "1", "4096", "64", "32", "3\0" "1", "\n") FINISH "\n", UMI_STATUS_PARSE_ERROR),
    CASE("duplicate-row", START "\n" CURRENT("\n") "abi.call-limit=32\n" FINISH "\n", UMI_STATUS_PARSE_ERROR),
    CASE("unknown-row", START "\n" CURRENT("\n") "abi.unexpected=0\n" FINISH "\n", UMI_STATUS_PARSE_ERROR),
};

static int CurrentValues(const UmiNativeKernelAbiReport *report)
{
    return report->major == 1U && report->minor == 0U && report->wordBits == 64U &&
        report->byteOrder == 1U && report->pageBytes == 4096U && report->copyLimit == 64U &&
        report->callLimit == 32U && report->compiledFeatures == 31U;
}

static int CheckParse(const ParseCase *test)
{
    /* Check the entire caller-owned result on every rejection. */
    UmiNativeKernelAbiReport result, original;
    memset(&result, 0xA5, sizeof result);
    memcpy(&original, &result, sizeof original);
    CHECK(UmiNativeKernelAbiReportParse(test->bytes, test->size, &result) == test->expected);
    if (test->expected != UMI_STATUS_OK) {
        CHECK(memcmp(&original, &result, sizeof result) == 0);
    } else if (strcmp(test->name, "future-abi") == 0) {
        CHECK(result.major == UINT16_MAX && result.minor == UINT16_MAX);
        CHECK(result.wordBits == 32U && result.byteOrder == 2U && result.pageBytes == 8192U);
        CHECK(result.copyLimit == 0U && result.callLimit == 8U && result.compiledFeatures == UINT64_MAX);
    } else CHECK(CurrentValues(&result));
    return 0;
}

static int CheckFile(const char *name)
{
    /* Use an actual absolute Unicode path and compare the untouched source via
     * the shared native reader. A rejected file never replaces the result. */
    char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    FixturePath(path, root, "ABI-\xC3\xA9.report");
    UmiNativeKernelAbiReport result, original;
    memset(&result, 0xA5, sizeof result);
    memcpy(&original, &result, sizeof original);
    if (strcmp(name, "file-missing") == 0) {
        CHECK(UmiNativeKernelAbiReportLoad(path, &result) == UMI_STATUS_NOT_FOUND);
        CHECK(memcmp(&original, &result, sizeof result) == 0);
        return 0;
    }
    UmiOutputFile *file = NULL;
    CHECK(UmiOutputFileCreate(path, &file) == UMI_STATUS_OK);
    size_t length = sizeof current - 1U;
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(name, "file-incomplete") == 0) {
        --length;
        expected = UMI_STATUS_PARSE_ERROR;
    }
    CHECK(UmiOutputFileWrite(file, current, length) == UMI_STATUS_OK);
    CHECK(UmiOutputFileClose(file) == UMI_STATUS_OK);
    UmiOutputFileDestroy(file);
    CHECK(UmiNativeKernelAbiReportLoad(path, &result) == expected);
    if (expected == UMI_STATUS_OK) CHECK(CurrentValues(&result));
    else CHECK(memcmp(&original, &result, sizeof result) == 0);
    unsigned char *readback = NULL;
    size_t actual = 0U;
    CHECK(UmiInputFileRead(path, length, &readback, &actual) == UMI_STATUS_OK);
    CHECK(actual == length && memcmp(readback, current, actual) == 0);
    UmiInputFileFree(readback);
    return 0;
}

int main(int argc, char **argv)
{
    /* The shared target selects a single case for a source-specific result. */
    CHECK(argc == 2);
    for (size_t index = 0U; index < sizeof records / sizeof records[0]; ++index)
        if (strcmp(argv[1], records[index].name) == 0) return CheckParse(&records[index]);
    if (strncmp(argv[1], "file-", 5U) == 0) return CheckFile(argv[1]);
    if (strcmp(argv[1], "bound") == 0) {
        char bytes[UMI_NATIVE_KERNEL_ABI_REPORT_MAX_BYTES + 1U] = {0};
        const ParseCase oversize = {"bound", bytes, sizeof bytes, UMI_STATUS_CAPACITY_EXCEEDED};
        return CheckParse(&oversize);
    }
    return 2;
}
