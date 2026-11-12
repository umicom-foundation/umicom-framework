/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_lifecycle/test_cmake_cache.c
 *
 * PURPOSE:
 *   Verify exact selected cache entries and preserve prior output after malformed input.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/toolchain/cmake_cache.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define REQUIRE(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); return 1; } } while (0)
typedef struct Fixture { const char *name, *text; UmiStatus status; const char *compiler; unsigned present; } Fixture;
static const Fixture fixtures[] = {
    {"exact", "CMAKE_C_COMPILER:FILEPATH=/tools/cc\n", (UmiStatus)0, "/tools/cc", 8U},
    {"prefix", "CMAKE_C_COMPILER:FILEPATH=/tools/cc-extra\n", (UmiStatus)0, "/tools/cc-extra", 8U},
    {"comments", "# CMAKE_C_COMPILER:FILEPATH=/wrong\n// CMAKE_C_COMPILER:FILEPATH=/wrong\nCMAKE_C_COMPILER:FILEPATH=/right\n", (UmiStatus)0, "/right", 8U},
    {"comment-only", "# CMAKE_C_COMPILER:FILEPATH=/wrong\n", (UmiStatus)0, "", 0U},
    {"quoted-key", "\"CMAKE_C_COMPILER\":FILEPATH=/tools/cc\n", (UmiStatus)0, "/tools/cc", 8U},
    {"crlf", "CMAKE_C_COMPILER:FILEPATH=/tools/cc\r\n", (UmiStatus)0, "/tools/cc", 8U},
    {"bom", "\357\273\277CMAKE_C_COMPILER:FILEPATH=/tools/cc\n", (UmiStatus)0, "/tools/cc", 8U},
    {"no-final-newline", "CMAKE_C_COMPILER:FILEPATH=/tools/cc", (UmiStatus)0, "/tools/cc", 8U},
    {"literal", "CMAKE_C_COMPILER:FILEPATH=C:\\tools with spaces\\cc=a;b.exe\n", (UmiStatus)0, "C:\\tools with spaces\\cc=a;b.exe", 8U},
    {"optional-empty", "CMAKE_C_COMPILER:FILEPATH=\n", (UmiStatus)0, "", 8U},
    {"unknown", "OTHER:BOOL=ON\n", (UmiStatus)0, "", 0U},
    {"prefix-key", "CMAKE_C_COMPILER_EXTRA:FILEPATH=/wrong\n", (UmiStatus)0, "", 0U},
    {"duplicate", "CMAKE_C_COMPILER:FILEPATH=/first\nCMAKE_C_COMPILER:FILEPATH=/second\n", (UmiStatus)5, "", 0U},
    {"duplicate-empty", "CMAKE_C_COMPILER:FILEPATH=\nCMAKE_C_COMPILER:FILEPATH=/second\n", (UmiStatus)5, "", 0U},
    {"wrong-type", "CMAKE_C_COMPILER:BOOL=ON\n", (UmiStatus)9, "", 0U},
    {"missing-type", "CMAKE_C_COMPILER=/tools/cc\n", (UmiStatus)9, "", 0U},
    {"missing-equals", "CMAKE_C_COMPILER:FILEPATH\n", (UmiStatus)9, "", 0U},
    {"tab-value", "CMAKE_C_COMPILER:FILEPATH=/tools/\tcc\n", (UmiStatus)9, "", 0U},
    {"late-malformed", "CMAKE_C_COMPILER:FILEPATH=/tools/cc\nCMAKE_GENERATOR:BOOL=Ninja\n", (UmiStatus)9, "", 0U},
    {"leading-space", " \tCMAKE_C_COMPILER:FILEPATH=/tools/cc\n", (UmiStatus)0, "/tools/cc", 8U},
    {"empty", "", (UmiStatus)0, "", 0U}
};
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    UmiCMakeCache before, result;
    memset(&before, 0x5a, sizeof before); result = before;
    for (size_t i = 0U; i < sizeof fixtures / sizeof fixtures[0]; ++i) {
        const Fixture *f = &fixtures[i];
        if (strcmp(argv[1], f->name)) continue;
        REQUIRE(UmiCMakeCacheParse(f->text, strlen(f->text), &result) == f->status);
        if (f->status != UMI_STATUS_OK) REQUIRE(!memcmp(&result, &before, sizeof result));
        else { REQUIRE(!strcmp(result.compiler, f->compiler)); REQUIRE(result.present == f->present); }
        return 0;
    }
    const char *text = "CMAKE_C_COMPILER:FILEPATH=/tools/cc";
    UmiStatus expected = UMI_STATUS_PARSE_ERROR;
    size_t length = strlen(text);
    unsigned char storage[UMI_BUILD_PATH_CAPACITY + 128U];
    memcpy(storage, text, length + 1U);
    if (!strcmp(argv[1], "embedded-nul")) storage[5] = 0U;
    else if (!strcmp(argv[1], "invalid-utf8")) storage[5] = 0xffU;
    else if (!strcmp(argv[1], "field-limit")) {
        const char *prefix = "CMAKE_C_COMPILER:FILEPATH=";
        length = strlen(prefix); memcpy(storage, prefix, length);
        memset(storage + length, 'a', UMI_BUILD_PATH_CAPACITY); length += UMI_BUILD_PATH_CAPACITY;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (!strcmp(argv[1], "byte-limit")) {
        length = UMI_CMAKE_CACHE_BYTE_LIMIT + 1U; expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (!strcmp(argv[1], "null-input")) {
        REQUIRE(UmiCMakeCacheParse(NULL, 0U, &result) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(!memcmp(&result, &before, sizeof result)); return 0;
    } else if (!strcmp(argv[1], "null-output")) {
        REQUIRE(UmiCMakeCacheParse(storage, length, NULL) == UMI_STATUS_INVALID_ARGUMENT); return 0;
    } else return 2;
    REQUIRE(UmiCMakeCacheParse(storage, length, &result) == expected);
    REQUIRE(!memcmp(&result, &before, sizeof result));
    return 0;
}
