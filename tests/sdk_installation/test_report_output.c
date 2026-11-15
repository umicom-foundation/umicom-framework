/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_installation/test_report_output.c
 *
 * PURPOSE:
 *   Verify SDK CLI report delivery and unchanged inspection results using isolated SDK bytes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../build_log/fixture.h"
#include "../support/report_stream.h"
#include "umicom/platform/filesystem.h"
#define printf ReportTestPrintf
#define puts ReportTestPuts
#define fflush ReportTestFlush
#define ferror ReportTestError
#include "../../tools/umicom/src/command_sdk.c"
#undef printf
#undef puts
#undef fflush
#undef ferror

static const char ReportFingerprint[] =
    "beab3af1b5a6e91196243c1bf9a86a6a0117d74a7874ce7fe581493e6f463c9b";
static const char ReportMismatch[] =
    "0000000000000000000000000000000000000000000000000000000000000000";

static void WriteFixture(const char *prefix, const char *leaf, const char *text)
{
    char path[UMI_PATH_CAPACITY];
    FixturePath(path, prefix, leaf);
    CHECK(umi_fs_write_text(path, text) == UMI_STATUS_OK);
}
static void MakeReportSdk(const char *prefix, const char *route)
{
    static const char *directories[] = {
        "share/umicom/framework", "lib/cmake/UmicomFramework", "include/umicom", "bin"
    };
    char path[UMI_PATH_CAPACITY];
    for (size_t i = 0U; i < sizeof directories / sizeof directories[0]; ++i) {
        FixturePath(path, prefix, directories[i]);
        CHECK(umi_fs_make_directories(path) == UMI_STATUS_OK);
    }
    WriteFixture(prefix, "share/umicom/framework/sdk.properties",
        "schema=umicom.sdk\nframework_version=7.12.3\nframework_abi=17\n"
        "include_directory=include\nlibrary_directory=lib\nbase_library=lib/a.a\n");
    WriteFixture(prefix, "lib/cmake/UmicomFramework/UmicomFrameworkConfig.cmake",
        "message(FATAL_ERROR \"Inspection must never execute package code\")\n");
    WriteFixture(prefix, "lib/cmake/UmicomFramework/UmicomFrameworkTargets.cmake", "# fixture\n");
    if (strcmp(route, "inspect-incomplete") != 0)
        WriteFixture(prefix, "lib/cmake/UmicomFramework/UmicomFrameworkConfigVersion.cmake", "# fixture\n");
    WriteFixture(prefix, "include/umicom/umicom.h", "/* presence only */\n");
    WriteFixture(prefix, "lib/a.a", "abc");
    if (strcmp(route, "artifacts-missing") != 0)
        WriteFixture(prefix, "bin/sdk-tool.exe", "tool bytes\n");
    WriteFixture(prefix, "share/umicom/framework/sdk-artifacts.txt",
        strcmp(route, "artifacts-empty") == 0 ?
        "schema=umicom.sdk.artifacts/1\nconfiguration=Debug\nentries=1\napi|interface|-\n" :
        "schema=umicom.sdk.artifacts/1\nconfiguration=Debug\nentries=3\n"
        "a|archive|lib/a.a\ntool|tool|bin/sdk-tool.exe\napi|interface|-\n");
}
int main(int argc, char **argv)
{
    CHECK(argc == 3);
    const char *route = argv[1], *mode = argv[2];
    char root[UMI_PATH_CAPACITY], prefix[UMI_PATH_CAPACITY], receipt[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    FixturePath(prefix, root, "SDK");
    MakeReportSdk(prefix, route);
    FixturePath(receipt, prefix, "share/umicom/framework/sdk-artifacts.txt");
    size_t before_size = 0U, after_size = 0U;
    unsigned char *before = FixtureRead(receipt, &before_size);
    char *arguments[4] = {"inspect", prefix, NULL, NULL};
    int argument_count = 2, content_result = 0;
    const char *expected_text = "File-presence evidence only.";
    if (strcmp(route, "inspect-incomplete") == 0) content_result = 1;
    else if (strncmp(route, "artifacts-", 10U) == 0) {
        CHECK(strcmp(route, "artifacts-complete") == 0 || strcmp(route, "artifacts-missing") == 0 ||
              strcmp(route, "artifacts-empty") == 0);
        arguments[0] = "artifacts"; arguments[2] = "--all"; argument_count = 3;
        content_result = strcmp(route, "artifacts-complete") == 0 ? 0 : 1;
        expected_text = "Declared file-presence evidence only.";
    } else if (strncmp(route, "fingerprint-", 12U) == 0) {
        CHECK(strcmp(route, "fingerprint-match") == 0 || strcmp(route, "fingerprint-mismatch") == 0);
        arguments[0] = "fingerprint"; arguments[2] = "--expect";
        arguments[3] = (char *)(strcmp(route, "fingerprint-match") == 0 ?
            ReportFingerprint : ReportMismatch);
        argument_count = 4; content_result = strcmp(route, "fingerprint-match") == 0 ? 0 : 1;
        expected_text = content_result == 0 ? "Expected fingerprint: match" : "Expected fingerprint: MISMATCH";
    } else if (strcmp(route, "help") == 0 || strcmp(route, "help-alias") == 0) {
        arguments[0] = strcmp(route, "help") == 0 ? "--help" : "help";
        argument_count = 1; expected_text = "umicom sdk fingerprint SDK_PREFIX";
    } else CHECK(strcmp(route, "inspect-complete") == 0 || strcmp(route, "inspect-incomplete") == 0);

    ReportTestReset("success", 0U);
    CHECK(umi_cli_command_sdk(argument_count, arguments) == content_result);
    CHECK(strstr(ReportTestText, expected_text) != NULL);
    CHECK(ReportTestFlushes == 1U && ReportTestErrors == 1U && ReportTestWrites != 0U);
    size_t writes = ReportTestWrites, report_size = ReportTestLength;
    char *complete = malloc(report_size + 1U); CHECK(complete != NULL);
    memcpy(complete, ReportTestText, report_size + 1U);

    ReportTestReset(mode, writes);
    CHECK(umi_cli_command_sdk(argument_count, arguments) ==
        (strcmp(mode, "success") == 0 ? content_result : 2));
    CHECK(ReportTestFlushes == 1U && ReportTestErrors == 1U && ReportTestWrites == writes);

    /* A refused stream is not a failed SDK observation. A fresh output
     * destination must produce the same content-specific result and bytes. */
    ReportTestReset("success", 0U);
    CHECK(umi_cli_command_sdk(argument_count, arguments) == content_result);
    CHECK(ReportTestLength == report_size && strcmp(ReportTestText, complete) == 0);
    unsigned char *after = FixtureRead(receipt, &after_size);
    CHECK(before_size == after_size && memcmp(before, after, before_size) == 0);
    free(after); free(before); free(complete);
    return 0;
}
