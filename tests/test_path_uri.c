/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_path_uri.c
 *
 * PURPOSE:
 *   Check URI syntax, host path context and transactional output without editor dependencies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/path_uri.h"
#include "umicom/platform/path.h"
#include <stdio.h>
#include <string.h>
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); return 1; } } while (0)
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *name = argv[1];
    char output[UMI_PATH_CAPACITY * 3U + 9U] = "sentinel";
    const char *input = "src/sample.c", *expected = "src/sample.c";
    UmiStatus expected_status = UMI_STATUS_OK, status;
    int absolute = 0;
    if (strcmp(name, "relative") == 0) { }
    else if (strcmp(name, "reserved") == 0) {
        input = "src/a b#c%d?.c"; expected = "src/a%20b%23c%25d%3F.c";
    } else if (strcmp(name, "unicode") == 0) {
        input = "src/caf\xc3\xa9.c"; expected = "src/caf%C3%A9.c";
    } else if (strcmp(name, "colon") == 0) {
        input = "src/a:b.c"; expected = "src/a%3Ab.c";
    } else if (strcmp(name, "absolute") == 0) {
        absolute = 1;
#ifdef _WIN32
        input = "C:\\Practice\\a b.c"; expected = "file:///C:/Practice/a%20b.c";
#else
        input = "/practice/a b.c"; expected = "file:///practice/a%20b.c";
#endif
    } else if (strcmp(name, "capacity") == 0) {
        CHECK(UmiPathUriReference("a b", output, 5U) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(strcmp(output, "sentinel") == 0);
        CHECK(UmiPathUriReference("a b", output, 6U) == UMI_STATUS_OK);
        CHECK(strcmp(output, "a%20b") == 0); return 0;
    } else if (strcmp(name, "alias") == 0) {
        strcpy(output, "src/a b.c");
        CHECK(UmiPathUriReference(output, output, sizeof output) == UMI_STATUS_OK);
        CHECK(strcmp(output, "src/a%20b.c") == 0); return 0;
    } else if (strcmp(name, "invalid-utf8") == 0) {
        input = "src/\xc0\x80"; expected_status = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(name, "empty") == 0) {
        input = ""; expected_status = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(name, "relative-rejected") == 0) {
        absolute = 1; expected_status = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(name, "too-long") == 0) {
        char long_path[UMI_PATH_CAPACITY + 1U];
        memset(long_path, 'x', sizeof long_path - 1U); long_path[sizeof long_path - 1U] = '\0';
        CHECK(UmiPathUriReference(long_path, output, sizeof output) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(strcmp(output, "sentinel") == 0); return 0;
    } else if (strcmp(name, "arguments") == 0) {
        CHECK(UmiPathUriReference(NULL, output, sizeof output) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiPathUriReference(input, NULL, 10U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiPathUriReference(input, output, 0U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiFileUriFromPath(NULL, output, sizeof output) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(strcmp(output, "sentinel") == 0); return 0;
    } else if (strcmp(name, "root-context") == 0) {
#ifdef _WIN32
        CHECK(UmiPathUriReference("C:relative.c", output, sizeof output) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiPathUriReference("\\relative.c", output, sizeof output) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(strcmp(output, "sentinel") == 0);
#else
        CHECK(UmiPathUriReference("C:relative.c", output, sizeof output) == UMI_STATUS_OK);
        CHECK(strcmp(output, "C%3Arelative.c") == 0);
#endif
        return 0;
    } else { fprintf(stderr, "Unknown case: %s\n", name); return 2; }
    status = absolute ? UmiFileUriFromPath(input, output, sizeof output) :
        UmiPathUriReference(input, output, sizeof output);
    CHECK(status == expected_status);
    CHECK(strcmp(output, status == UMI_STATUS_OK ? expected : "sentinel") == 0);
    return 0;
}
