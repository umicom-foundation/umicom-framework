/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/build_cache/test_cache.c
 * PURPOSE: Check cache identity, literal values, malformed metadata and atomic publication.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer_project/build_cache.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                          \
            failed = 1;                                                                                      \
            goto cleanup;                                                                                    \
        }                                                                                                    \
    } while (0)
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    int failed = 0;
#ifdef _WIN32
    const char *source = "C:/work/caf\xc3\xa9", *build = "C:/work/caf\xc3\xa9/build";
#else
    const char *source = "/work/caf\xc3\xa9", *build = "/work/caf\xc3\xa9/build";
#endif
    UmiProjectBuildCache before, result;
    memset(&before, 0x5a, sizeof(before));
    result = before;
    size_t capacity = UMI_PROJECT_BUILD_CACHE_BYTE_LIMIT + 2U;
    char *text = malloc(capacity);
    CHECK(text != NULL);
    const char *newline = strcmp(mode, "crlf") == 0 ? "\r\n" : "\n";
    const char *homeKey =
        strcmp(mode, "quoted-key") == 0 ? "\"CMAKE_HOME_DIRECTORY\"" : "CMAKE_HOME_DIRECTORY";
    const char *generator = strcmp(mode, "missing") == 0      ? "UNRELATED:STRING=Ninja"
                            : strcmp(mode, "wrong-type") == 0 ? "CMAKE_GENERATOR:BOOL=Ninja"
                            : strcmp(mode, "malformed") == 0  ? "CMAKE_GENERATOR=Ninja"
                            : strcmp(mode, "controls") == 0   ? "CMAKE_GENERATOR:INTERNAL=Ni\tnja"
                                                              : "CMAKE_GENERATOR:INTERNAL=Ninja";
    int length =
        snprintf(text, capacity,
                 "%s# Cache metadata%s// Comment%s%s:INTERNAL=%s%s%sCMAKE_CACHEFILE_DIR:INTERNAL=%s%s%s%s%s",
                 strcmp(mode, "bom") == 0 ? "\xef\xbb\xbf" : "", newline, newline, homeKey, source,
                 strcmp(mode, "source-mismatch") == 0 ? "/other" : "", newline, build,
                 strcmp(mode, "build-mismatch") == 0 ? "/other" : "", newline, generator, newline);
    CHECK(length > 0 && (size_t)length < capacity);
    size_t size = (size_t)length;
    const char *extra = "CMAKE_C_COMPILER:FILEPATH=clang\nCMAKE_BUILD_TYPE:STRING=Debug\nCMAKE_CONFIGURATION_"
                        "TYPES:STRING=Debug;Release\n";
    if (strcmp(mode, "duplicate") == 0)
        extra = "CMAKE_GENERATOR:INTERNAL=Ninja\n";
    if (strcmp(mode, "unicode-invalid") == 0)
        extra = "IGNORED:STRING=\xc0\xaf\n";
    if (strcmp(mode, "optional-empty") == 0)
        extra = "CMAKE_C_COMPILER:FILEPATH=\nCMAKE_BUILD_TYPE:STRING=\n";
    if (strcmp(mode, "literal-values") == 0)
        extra = "CMAKE_C_COMPILER:FILEPATH=C:\\tools\\clang.exe\nCMAKE_INSTALL_PREFIX:PATH=folder with "
                "spaces;a=b\nCMAKE_TOOLCHAIN_FILE:FILEPATH=$env{HOME}/toolchain.cmake\n";
    memcpy(text + size, extra, strlen(extra) + 1U);
    size += strlen(extra);
    if (strcmp(mode, "embedded-nul") == 0)
        text[size - 1U] = '\0';
    if (strcmp(mode, "field-limit") == 0)
    {
        const char *key = "CMAKE_TOOLCHAIN_FILE:FILEPATH=";
        memcpy(text + size, key, strlen(key));
        size += strlen(key);
        memset(text + size, 'x', UMI_BUILD_PATH_CAPACITY);
        size += UMI_BUILD_PATH_CAPACITY;
    }
    if (strcmp(mode, "byte-limit") == 0)
        size = UMI_PROJECT_BUILD_CACHE_BYTE_LIMIT + 1U;
    UmiStatus expected = strcmp(mode, "duplicate") == 0 ? UMI_STATUS_ALREADY_EXISTS
                         : (strcmp(mode, "field-limit") == 0 || strcmp(mode, "byte-limit") == 0)
                             ? UMI_STATUS_CAPACITY_EXCEEDED
                         : (strcmp(mode, "missing") == 0 || strcmp(mode, "wrong-type") == 0 ||
                            strcmp(mode, "malformed") == 0 || strcmp(mode, "controls") == 0 ||
                            strcmp(mode, "unicode-invalid") == 0 || strcmp(mode, "embedded-nul") == 0)
                             ? UMI_STATUS_PARSE_ERROR
                             : UMI_STATUS_OK;
    UmiStatus status = UmiProjectBuildCacheParse(text, size, source, build, &result);
    CHECK(status == expected);
    if (expected != UMI_STATUS_OK)
        CHECK(memcmp(&result, &before, sizeof(result)) == 0);
    else
    {
        CHECK(result.source_matches == (strcmp(mode, "source-mismatch") != 0));
        CHECK(result.build_matches == (strcmp(mode, "build-mismatch") != 0));
        CHECK(strcmp(result.generator, "Ninja") == 0);
        if (strcmp(mode, "literal-values") == 0)
        {
            CHECK(strcmp(result.compiler, "C:\\tools\\clang.exe") == 0);
            CHECK(strcmp(result.install_directory, "folder with spaces;a=b") == 0);
            CHECK(strcmp(result.toolchain_file, "$env{HOME}/toolchain.cmake") == 0);
        }
        else if (strcmp(mode, "optional-empty") == 0)
        {
            CHECK(result.compiler[0] == '\0' && result.configuration[0] == '\0' &&
                  result.configurations[0] == '\0');
        }
        else
            CHECK(strcmp(result.compiler, "clang") == 0 &&
                  strcmp(result.configurations, "Debug;Release") == 0);
    }
cleanup:
    free(text);
    return failed;
}
