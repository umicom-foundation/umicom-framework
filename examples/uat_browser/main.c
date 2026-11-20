/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/uat_browser/main.c
 *
 * PURPOSE:
 *   Verify a local Chromium page through a named UAT observation probe.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/path.h"
#include "umicom/testing/uat_process_fixture.h"
#include <stdio.h>
#include <string.h>

static int ReadExpectation(const char *path, unsigned char *bytes, size_t *length)
{
    FILE *file = fopen(path, "rb");
    if (file == NULL) return 0;
    size_t count = 0U;
    int complete = 1;
    /* Treat the reviewed HTML as a text snapshot. Chromium's Windows CLI
     * writes CRLF. Prepare that expected span before the raw fixture check. */
    for (int ch = fgetc(file); ch != EOF; ch = fgetc(file)) {
        if (ch == '\r') {
            ch = fgetc(file);
            if (ch != '\n') { complete = 0; break; }
        }
#ifdef _WIN32
        if (ch == '\n' && count < UMI_UAT_PROCESS_EXPECTED_LIMIT) bytes[count++] = '\r';
#endif
        if (count == UMI_UAT_PROCESS_EXPECTED_LIMIT) { complete = 0; break; }
        bytes[count++] = (unsigned char)ch;
    }
    complete = complete && !ferror(file) && feof(file);
    const int closed = fclose(file);
    if (!complete || closed != 0 || count == 0U) return 0;
    *length = count;
    return 1;
}

static int IsLocalPageUrl(const char *url)
{
    if (strncmp(url, "file:///", 8U) != 0 || url[8] == '\0') return 0;
#ifdef _WIN32
    const char drive = url[8];
    return ((drive >= 'A' && drive <= 'Z') || (drive >= 'a' && drive <= 'z')) &&
        url[9] == ':' && url[10] == '/';
#else
    return url[8] != '/' && url[8] != '\\';
#endif
}

int main(int argc, char **argv)
{
    if (argc != 5 || !umi_path_is_absolute(argv[1]) || !umi_path_is_absolute(argv[2]) ||
        !IsLocalPageUrl(argv[3]) || !umi_path_is_absolute(argv[4])) {
        fputs("Usage: umicom-uat-browser-observation <absolute-browser> <absolute-scratch-folder> "
              "<local-file-url> <absolute-expected-dom>\n", stderr);
        return 2;
    }

    unsigned char expected[UMI_UAT_PROCESS_EXPECTED_LIMIT];
    size_t length = 0U;
    if (!ReadExpectation(argv[4], expected, &length)) {
        fputs("Expected browser output must be a readable, nonempty file of at most 8192 bytes.\n", stderr);
        return 2;
    }
    char profile[UMI_TEST_COMMAND_CAPACITY];
    const int profileLength = snprintf(profile, sizeof profile, "--user-data-dir=%s/browser-profile", argv[2]);
    if (profileLength < 0 || (size_t)profileLength >= sizeof profile) return 2;
    /* The example supplies a disposable local profile, one file URL and an
     * exact expected DOM. The workflow can only ask for the named check. */
    const char *arguments[] = {"--headless=new", "--disable-gpu", "--no-first-run",
        "--no-default-browser-check", "--disable-background-networking", "--disable-component-update",
        "--disable-sync", "--disable-extensions", "--host-resolver-rules=MAP * ~NOTFOUND",
        profile, "--dump-dom", argv[3]};
    const UmiUatTarget target = {UMI_UAT_TARGET_PROTOCOL, UMI_UAT_WEB, "chromium-observation", "local-page"};
    const UmiUatProcessProbe probe = {"read-total", UMI_UAT_UI_OBSERVE, argv[1], argv[2],
        arguments, sizeof arguments / sizeof arguments[0], 20000U, expected, length};
    const UmiUatProcessFixture fixture = {target, &probe, 1U};
    UmiUatStep step = {.phase = UMI_UAT_ASSERT};
    umi_test_case_init(&step.test, "read-total", "Observe the computed total in the local page");
    step.test.kind = UMI_TEST_KIND_EXTERNAL;
    step.test.timeout_ms = 20000U;
    UmiUatReport *report = NULL;
    UmiStatus status = UmiUatRunProcessFixture(&target, UMI_UAT_UI_OBSERVE, &fixture, &step, 1U,
        NULL, NULL, &report);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "The browser fixture could not be prepared (%d).\n", (int)status);
        return 2;
    }
    UmiUatSummary summary;
    char json[4096];
    status = UmiUatReportSummary(report, &summary);
    if (status == UMI_STATUS_OK) status = UmiUatReportJson(report, 1, json, sizeof json);
    UmiUatReportDestroy(report);
    if (status != UMI_STATUS_OK || puts(json) == EOF) return 2;
    return summary.accepted ? 0 : 1;
}
