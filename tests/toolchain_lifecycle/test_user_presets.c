/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_lifecycle/test_user_presets.c
 *
 * PURPOSE:
 *   Verify bounded JSON and literal CMake macro formatting with unchanged output on refusal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/toolchain/user_presets.h"
#include "umicom/protocol/json.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define REQUIRE(x) do { if (!(x)) { fprintf(stderr, "%s line %d: %s\n", mode, __LINE__, #x); return 1; } } while (0)
static void Profile(UmiToolchainProfile *profile)
{
    umi_toolchain_profile_init(profile);
    profile->selected_c_compiler = UMI_TOOL_GCC;
    profile->tools[UMI_TOOL_GCC].state = UMI_TOOL_VALIDATED;
    profile->tools[UMI_TOOL_NINJA].state = UMI_TOOL_VALIDATED;
    profile->tools[UMI_TOOL_PKG_CONFIG].state = UMI_TOOL_VALIDATED;
    strcpy(profile->tools[UMI_TOOL_GCC].path, "C:\\tools\\cc.exe");
    strcpy(profile->tools[UMI_TOOL_NINJA].path, "C:\\tools\\ninja.exe");
    strcpy(profile->tools[UMI_TOOL_PKG_CONFIG].path, "C:\\tools\\pkg.exe");
    strcpy(profile->prefix_directory, "C:\\sdk");
}
int main(int argc, char **argv)
{
    if (argc < 2 || argc > 3) return 2;
    const char *mode = argv[1];
    static UmiToolchainProfile profile; Profile(&profile);
    static char json[UMI_BUILD_USER_PRESETS_CAPACITY], before[UMI_BUILD_USER_PRESETS_CAPACITY];
    memset(before, 0x5a, sizeof before); memcpy(json, before, sizeof json);
    char *out = json; size_t capacity = sizeof json, size = 12345U;
    const UmiToolchainProfile *selected = &profile;
    UmiStatus expected = UMI_STATUS_OK;
    if (!strcmp(mode, "cmake-fixture") && argc == 3) {
        REQUIRE(strlen(argv[2]) < sizeof profile.tools[UMI_TOOL_NINJA].path);
        strcpy(profile.tools[UMI_TOOL_NINJA].path, argv[2]);
        strcpy(profile.tools[UMI_TOOL_GCC].path, "C:/training/${sourceDir}/cc\"quoted\\tail");
        strcpy(profile.prefix_directory, "C:/training/$env{PATH};C:/literal/${dollar}");
        REQUIRE(UmiBuildUserPresetsFormat(&profile, json, sizeof json, &size) == UMI_STATUS_OK);
        REQUIRE(fwrite(json, 1U, size, stdout) == size);
        return ferror(stdout) ? 1 : 0;
    }
    if (argc != 2) return 2;
    if (!strcmp(mode, "valid")) { }
    else if (!strcmp(mode, "quotes")) strcpy(profile.tools[UMI_TOOL_GCC].path, "C:/a\"b/cc");
    else if (!strcmp(mode, "controls")) strcpy(profile.prefix_directory, "x\n\r\t\b\f\001y");
    else if (!strcmp(mode, "unicode")) strcpy(profile.prefix_directory, "C:/caf\303\251/sdk");
    else if (!strcmp(mode, "macros")) strcpy(profile.prefix_directory, "$env{PATH}/${sourceDir}/$vendor{x}/$");
    else if (!strcmp(mode, "maximum")) {
        memset(profile.tools[UMI_TOOL_GCC].path, '$', sizeof profile.tools[UMI_TOOL_GCC].path - 1U);
        profile.tools[UMI_TOOL_GCC].path[sizeof profile.tools[UMI_TOOL_GCC].path - 1U] = '\0';
        strcpy(profile.tools[UMI_TOOL_NINJA].path, profile.tools[UMI_TOOL_GCC].path);
        strcpy(profile.tools[UMI_TOOL_PKG_CONFIG].path, profile.tools[UMI_TOOL_GCC].path);
        strcpy(profile.prefix_directory, profile.tools[UMI_TOOL_GCC].path);
    } else if (!strcmp(mode, "empty-prefix")) profile.prefix_directory[0] = '\0';
    else if (!strcmp(mode, "small-output")) { capacity = 2U; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (!strcmp(mode, "null-output")) { out = NULL; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(mode, "zero-output")) { capacity = 0U; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(mode, "null-profile")) { selected = NULL; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(mode, "profile-size")) { profile.structure_size = 0U; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(mode, "profile-version")) { ++profile.api_version; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(mode, "profile-count")) { profile.tool_count = UMI_TOOLCHAIN_PROFILE_TOOL_CAPACITY + 1U; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(mode, "not-compiler")) { profile.selected_c_compiler = UMI_TOOL_NINJA; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(mode, "missing-compiler")) { profile.tools[UMI_TOOL_GCC].state = UMI_TOOL_FOUND; expected = UMI_STATUS_NOT_FOUND; }
    else if (!strcmp(mode, "missing-ninja")) { profile.tools[UMI_TOOL_NINJA].state = UMI_TOOL_MISSING; expected = UMI_STATUS_NOT_FOUND; }
    else if (!strcmp(mode, "missing-pkg")) { profile.tools[UMI_TOOL_PKG_CONFIG].state = UMI_TOOL_FAILED; expected = UMI_STATUS_NOT_FOUND; }
    else if (!strcmp(mode, "slot-kind")) { profile.tools[UMI_TOOL_NINJA].kind = UMI_TOOL_GIT; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(mode, "empty-compiler")) { profile.tools[UMI_TOOL_GCC].path[0] = '\0'; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(mode, "compiler-limit")) { memset(profile.tools[UMI_TOOL_GCC].path, 'x', sizeof profile.tools[UMI_TOOL_GCC].path); expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (!strcmp(mode, "ninja-limit")) { memset(profile.tools[UMI_TOOL_NINJA].path, 'x', sizeof profile.tools[UMI_TOOL_NINJA].path); expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (!strcmp(mode, "pkg-limit")) { memset(profile.tools[UMI_TOOL_PKG_CONFIG].path, 'x', sizeof profile.tools[UMI_TOOL_PKG_CONFIG].path); expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (!strcmp(mode, "prefix-limit")) { memset(profile.prefix_directory, 'x', sizeof profile.prefix_directory); expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (!strcmp(mode, "invalid-utf8")) { profile.prefix_directory[0] = (char)0xff; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else return 2;
    REQUIRE(UmiBuildUserPresetsFormat(selected, out, capacity, &size) == expected);
    if (expected != UMI_STATUS_OK) {
        REQUIRE(!memcmp(json, before, sizeof json) && size == 12345U);
        return 0;
    }
    REQUIRE(size == strlen(json));
    REQUIRE(strstr(json, "\"version\":6") != NULL);
    REQUIRE(strstr(json, "\"binaryDir\":\"${sourceDir}/build/umicom-auto-debug\"") != NULL);
    char decoded[UMI_TOOL_PATH_CAPACITY * 9U + 1U];
    if (!strcmp(mode, "valid") || !strcmp(mode, "quotes")) {
        REQUIRE(umi_json_get_string(json, "CMAKE_C_COMPILER", decoded, sizeof decoded) == UMI_STATUS_OK);
        REQUIRE(!strcmp(decoded, profile.tools[UMI_TOOL_GCC].path));
    }
    if (!strcmp(mode, "controls")) REQUIRE(strstr(json, "x\\n\\r\\t\\u0008\\u000c\\u0001y") != NULL);
    if (!strcmp(mode, "unicode")) REQUIRE(strstr(json, "caf\303\251") != NULL);
    if (!strcmp(mode, "macros")) {
        REQUIRE(umi_json_get_string(json, "CMAKE_PREFIX_PATH", decoded, sizeof decoded) == UMI_STATUS_OK);
        REQUIRE(!strcmp(decoded, "${dollar}env{PATH}/${dollar}{sourceDir}/${dollar}vendor{x}/${dollar}"));
    }
    if (!strcmp(mode, "maximum")) REQUIRE(size > 4U * (UMI_TOOL_PATH_CAPACITY - 1U) * 9U);
    /* Capacity is inclusive of the NUL; exactly-sized output must also work. */
    REQUIRE(UmiBuildUserPresetsFormat(&profile, json, size + 1U, NULL) == UMI_STATUS_OK);
    return 0;
}
