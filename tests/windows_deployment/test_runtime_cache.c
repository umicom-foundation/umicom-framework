/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/windows_deployment/test_runtime_cache.c
 *
 * PURPOSE:
 *   Exercise cache relocation, bounds, Unicode bytes and hostile path syntax.
 *   Explicit checks remain active in Release builds; NDEBUG cannot disable them.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "umicom/delivery/windows_runtime_cache.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "Check failed at line %d: %s\n", __LINE__, #condition); \
    return EXIT_FAILURE; } } while (0)

/* Small PE headers are binary-format fixtures, never shipped applications. */
static int UmiWritePeFixture(const char *path, const char *architecture)
{
    unsigned char bytes[256] = {0};
    FILE *output;
    bytes[0] = 'M'; bytes[1] = 'Z'; bytes[60] = 128U;
    bytes[128] = 'P'; bytes[129] = 'E';
    if (strcmp(architecture, "x86") == 0) {
        bytes[132] = 0x4cU; bytes[133] = 0x01U;
    } else {
        bytes[132] = 0x64U; bytes[133] = 0x86U;
    }
    (void)memcpy(bytes + 160, "Umicom PE header fixture - not an application", 44U);
    output = fopen(path, "wb");
    CHECK(output != NULL);
    CHECK(fwrite(bytes, 1U, sizeof(bytes), output) == sizeof(bytes));
    CHECK(fclose(output) == 0);
    return EXIT_SUCCESS;
}

static int UmiCacheCase(const char *name)
{
    static const char token[] = UMI_WINDOWS_RUNTIME_ROOT_TOKEN;
    char output[4096];
    size_t needed = 0U;
    UmiWindowsRuntimeCacheResult result;
    const char *input = "\"@UMICOM_RUNTIME_ROOT@/lib/loader.dll\"\n";
    (void)memset(output, '!', sizeof(output));
    if (strcmp(name, "empty") == 0) {
        CHECK(UmiWindowsRuntimeExpandCache("", 0U, "C:/Umicom", output,
            sizeof(output), &needed) == UMI_WINDOWS_RUNTIME_CACHE_OK);
        CHECK(needed == 1U && output[0] == '\0');
    } else if (strcmp(name, "unchanged") == 0) {
        CHECK(UmiWindowsRuntimeExpandCache("plain\n", 6U, "C:/Umicom", output,
            sizeof(output), &needed) == UMI_WINDOWS_RUNTIME_CACHE_OK);
        CHECK(strcmp(output, "plain\n") == 0 && needed == 7U);
    } else if (strcmp(name, "single") == 0) {
        CHECK(UmiWindowsRuntimeExpandCache(input, strlen(input), "D:/Umicom Suite",
            output, sizeof(output), &needed) == UMI_WINDOWS_RUNTIME_CACHE_OK);
        CHECK(strcmp(output, "\"D:/Umicom Suite/lib/loader.dll\"\n") == 0);
    } else if (strcmp(name, "repeated") == 0) {
        input = "@UMICOM_RUNTIME_ROOT@|@UMICOM_RUNTIME_ROOT@";
        CHECK(UmiWindowsRuntimeExpandCache(input, strlen(input), "D:/Apps", output,
            sizeof(output), &needed) == UMI_WINDOWS_RUNTIME_CACHE_OK);
        CHECK(strcmp(output, "D:/Apps|D:/Apps") == 0);
    } else if (strcmp(name, "partial") == 0) {
        CHECK(UmiWindowsRuntimeExpandCache(token, strlen(token) - 1U, "D:/Apps",
            output, sizeof(output), &needed) == UMI_WINDOWS_RUNTIME_CACHE_OK);
        CHECK(needed == strlen(token));
        CHECK(memcmp(output, token, needed - 1U) == 0);
    } else if (strcmp(name, "unicode") == 0) {
        const char *root = "D:/Umicom \xe6\x96\x87\xe4\xbb\xb6/\xc3\xa9";
        CHECK(UmiWindowsRuntimeExpandCache(token, strlen(token), root, output,
            sizeof(output), &needed) == UMI_WINDOWS_RUNTIME_CACHE_OK);
        CHECK(strcmp(output, root) == 0);
    } else if (strcmp(name, "backslashes") == 0) {
        CHECK(UmiWindowsRuntimeExpandCache(token, strlen(token), "D:\\Umicom\\Apps",
            output, sizeof(output), &needed) == UMI_WINDOWS_RUNTIME_CACHE_OK);
        CHECK(strcmp(output, "D:/Umicom/Apps") == 0);
    } else if (strcmp(name, "short") == 0) {
        CHECK(UmiWindowsRuntimeExpandCache(input, strlen(input), "D:/Apps", output,
            2U, &needed) == UMI_WINDOWS_RUNTIME_CACHE_INSUFFICIENT_CAPACITY);
        CHECK(needed > 2U && output[0] == '!' && output[1] == '!');
    } else if (strcmp(name, "query") == 0 || strcmp(name, "exact") == 0) {
        CHECK(UmiWindowsRuntimeExpandCache(input, strlen(input), "D:/Apps", NULL,
            0U, &needed) == UMI_WINDOWS_RUNTIME_CACHE_OK);
        CHECK(needed == strlen("\"D:/Apps/lib/loader.dll\"\n") + 1U);
        if (strcmp(name, "exact") == 0) {
            const size_t capacity = needed;
            CHECK(UmiWindowsRuntimeExpandCache(input, strlen(input), "D:/Apps", output,
                capacity, &needed) == UMI_WINDOWS_RUNTIME_CACHE_OK);
            CHECK(output[capacity - 1U] == '\0' && output[capacity] == '!');
        }
    } else if (strcmp(name, "invalid-root") == 0) {
        const char *roots[] = {"", "D:/A\"B", "D:/A\nB", "D:/A\rB", "D:/A\tB"};
        for (size_t index = 0U; index < sizeof(roots) / sizeof(roots[0]); ++index) {
            CHECK(UmiWindowsRuntimeExpandCache(input, strlen(input), roots[index],
                output, sizeof(output), &needed) == UMI_WINDOWS_RUNTIME_CACHE_INVALID_ARGUMENT);
            CHECK(needed == 0U && output[0] == '!');
        }
    } else if (strcmp(name, "nul") == 0) {
        const char embedded[] = {'a', '\0', 'b'};
        CHECK(UmiWindowsRuntimeExpandCache(embedded, sizeof(embedded), "D:/Apps",
            output, sizeof(output), &needed) == UMI_WINDOWS_RUNTIME_CACHE_INVALID_ARGUMENT);
    } else if (strcmp(name, "null-arguments") == 0) {
        CHECK(UmiWindowsRuntimeExpandCache(NULL, 0U, "D:/Apps", output,
            sizeof(output), &needed) == UMI_WINDOWS_RUNTIME_CACHE_INVALID_ARGUMENT);
        CHECK(UmiWindowsRuntimeExpandCache(input, strlen(input), NULL, output,
            sizeof(output), &needed) == UMI_WINDOWS_RUNTIME_CACHE_INVALID_ARGUMENT);
        CHECK(UmiWindowsRuntimeExpandCache(input, strlen(input), "D:/Apps", output,
            sizeof(output), NULL) == UMI_WINDOWS_RUNTIME_CACHE_INVALID_ARGUMENT);
        CHECK(UmiWindowsRuntimeExpandCache(input, strlen(input), "D:/Apps", NULL,
            1U, &needed) == UMI_WINDOWS_RUNTIME_CACHE_INVALID_ARGUMENT);
    } else if (strcmp(name, "dollar") == 0) {
        CHECK(UmiWindowsRuntimeExpandCache(token, strlen(token), "D:/$Umicom", output,
            sizeof(output), &needed) == UMI_WINDOWS_RUNTIME_CACHE_OK);
        CHECK(strcmp(output, "D:/$Umicom") == 0);
    } else if (strcmp(name, "many") == 0) {
        char pattern[2048] = "";
        char expected[2048] = "";
        for (size_t count = 1U; count < 60U; ++count) {
            (void)strcat(pattern, token);
            (void)strcat(expected, "D:/A");
            result = UmiWindowsRuntimeExpandCache(pattern, strlen(pattern), "D:/A",
                output, sizeof(output), &needed);
            CHECK(result == UMI_WINDOWS_RUNTIME_CACHE_OK);
            CHECK(strcmp(output, expected) == 0 && needed == strlen(expected) + 1U);
        }
    } else {
        fprintf(stderr, "Unknown test: %s\n", name);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
    if (argc == 4 && strcmp(argv[1], "--pe-fixture") == 0) {
        return UmiWritePeFixture(argv[2], argv[3]);
    }
    if (argc != 2) {
        fprintf(stderr, "Supply a named cache test.\n");
        return EXIT_FAILURE;
    }
    return UmiCacheCase(argv[1]);
}
