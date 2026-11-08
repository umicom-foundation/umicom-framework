/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_lifecycle/test_version_inventory.c
 *
 * PURPOSE:
 *   Check evidence meaning, complete escaping and malformed discovery admission without executing tools.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/version_inventory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); failed = 1; goto finish; } } while (0)
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    int failed = 0;
    UmiToolchainDiscoveryReport *r = calloc(1U, sizeof(*r));
    char *json = malloc(UMI_TOOLCHAIN_VERSION_INVENTORY_CAPACITY);
    char *again = malloc(UMI_TOOLCHAIN_VERSION_INVENTORY_CAPACITY);
    CHECK(r && json && again);
    umi_toolchain_profile_init(&r->profile);
    UmiToolInfo *t = &r->profile.tools[UMI_TOOL_GCC];
    t->state = UMI_TOOL_VALIDATED; t->required = 1;
    strcpy(t->path, "C:/tool set/gcc.exe"); strcpy(t->version, "fixture-1");
    r->tools_found = 1U; r->required_tools = 2U; r->required_tools_missing = 1U;
    r->profile.selected_c_compiler = UMI_TOOL_GCC;
    UmiStatus discovery = UMI_STATUS_NOT_FOUND, expected = UMI_STATUS_INVALID_ARGUMENT;
    size_t size = sizeof(*r), capacity = UMI_TOOLCHAIN_VERSION_INVENTORY_CAPACITY, length = 99U;
    const UmiToolchainDiscoveryReport *input = r;
    char *output = json;
    const char *mode = argv[1];
    if (!strcmp(mode, "incomplete")) expected = UMI_STATUS_OK;
    else if (!strcmp(mode, "complete")) {
        discovery = UMI_STATUS_OK; r->required_tools_missing = 0U;
        r->profile.complete = 1; expected = UMI_STATUS_OK;
    } else if (!strcmp(mode, "escaped")) {
        strcpy(t->version, "a\"b\\c\n\r\t\001\033"); expected = UMI_STATUS_OK;
    } else if (!strcmp(mode, "unicode")) {
        strcpy(t->path, "C:/tools/\xc3\xa9/\xf0\x9f\x94\xa7/gcc.exe"); expected = UMI_STATUS_OK;
    } else if (!strcmp(mode, "maximum-text")) {
        memset(t->path, 1, sizeof(t->path) - 1U); t->path[sizeof(t->path) - 1U] = '\0';
        expected = UMI_STATUS_OK;
    } else if (!strcmp(mode, "null-report")) input = NULL;
    else if (!strcmp(mode, "stale-size")) --size;
    else if (!strcmp(mode, "stale-profile")) --r->profile.structure_size;
    else if (!strcmp(mode, "stale-api")) ++r->profile.api_version;
    else if (!strcmp(mode, "null-output")) output = NULL;
    else if (!strcmp(mode, "zero-capacity")) capacity = 0U;
    else if (!strcmp(mode, "small-output")) { capacity = 8U; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (!strcmp(mode, "invalid-status")) discovery = (UmiStatus)999;
    else if (!strcmp(mode, "tool-count")) r->profile.tool_count = UMI_TOOLCHAIN_PROFILE_TOOL_CAPACITY + 1U;
    else if (!strcmp(mode, "duplicate-kind")) r->profile.tools[0].kind = r->profile.tools[1].kind;
    else if (!strcmp(mode, "invalid-kind")) t->kind = UMI_TOOL_COUNT;
    else if (!strcmp(mode, "invalid-state")) t->state = (UmiToolState)99;
    else if (!strcmp(mode, "invalid-family")) r->profile.family = (UmiToolchainFamily)99;
    else if (!strcmp(mode, "invalid-compiler")) r->profile.selected_c_compiler = (UmiToolKind)-1;
    else if (!strcmp(mode, "invalid-cpp")) r->profile.selected_cpp_compiler = (UmiToolKind)(UMI_TOOL_COUNT + 1);
    else if (!strcmp(mode, "unterminated-path")) memset(t->path, 'x', sizeof(t->path));
    else if (!strcmp(mode, "unterminated-version")) memset(t->version, 'x', sizeof(t->version));
    else if (!strcmp(mode, "unterminated-profile")) memset(r->profile.profile_id, 'x', sizeof(r->profile.profile_id));
    else if (!strcmp(mode, "invalid-utf8")) { t->version[0] = (char)0xc0; t->version[1] = (char)0x80; t->version[2] = 0; }
    else if (!strcmp(mode, "found-count")) r->tools_found = UMI_TOOL_COUNT + 1U;
    else if (!strcmp(mode, "required-count")) r->required_tools = UMI_TOOL_COUNT + 2U;
    else if (!strcmp(mode, "missing-count")) r->required_tools_missing = r->required_tools + 1U;
    else if (!strcmp(mode, "compile-pass")) { r->compile_probe_passed = 1; expected = UMI_STATUS_INVALID_STATE; }
    else if (!strcmp(mode, "link-pass")) { r->link_probe_passed = 1; expected = UMI_STATUS_INVALID_STATE; }
    else if (!strcmp(mode, "runtime-pass")) { r->runtime_probe_passed = 1; expected = UMI_STATUS_INVALID_STATE; }
    else if (!strcmp(mode, "c23-pass")) { r->c23_probe_passed = 1; expected = UMI_STATUS_INVALID_STATE; }
    else if (!strcmp(mode, "c23-capable")) { r->profile.c23_capable = 1; expected = UMI_STATUS_INVALID_STATE; }
    else { failed = 2; goto finish; }
    memset(json, '!', 16U);
    UmiStatus result = umi_toolchain_version_inventory_format_sized(input, size,
        discovery, output, capacity, &length);
    CHECK(result == expected);
    if (result == UMI_STATUS_OK) {
        CHECK(length == strlen(json) && length != 0U && json[length - 1U] == '\n');
        CHECK(strstr(json, "\"qualificationPassed\":false") != NULL);
        CHECK(strstr(json, "\"compile\":\"not-run\"") != NULL);
        CHECK(strstr(json, "\"runtime\":\"not-run\"") != NULL);
        CHECK(strstr(json, "\"state\":\"MISSING\"") != NULL);
        CHECK(strstr(json, "\"toolsFound\":1") != NULL);
        CHECK(umi_toolchain_version_inventory_format(r, discovery, again,
            UMI_TOOLCHAIN_VERSION_INVENTORY_CAPACITY, NULL) == UMI_STATUS_OK);
        CHECK(!strcmp(json, again));
        if (!strcmp(mode, "incomplete")) CHECK(strstr(json, "\"discoveryStatus\":4") != NULL);
        if (!strcmp(mode, "escaped")) CHECK(strstr(json, "a\\\"b\\\\c\\n\\r\\t\\u0001\\u001b") != NULL);
        if (!strcmp(mode, "unicode")) CHECK(strstr(json, "\xc3\xa9/\xf0\x9f\x94\xa7") != NULL);
        /* Exact capacity succeeds; one byte less must fail without a truncated document. */
        CHECK(umi_toolchain_version_inventory_format(r, discovery, again, length + 1U, NULL) == UMI_STATUS_OK);
        CHECK(umi_toolchain_version_inventory_format(r, discovery, again, length, NULL) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(again[0] == '\0');
    } else {
        CHECK(length == 0U);
        if (output != NULL && capacity != 0U) CHECK(json[0] == '\0');
        if (capacity == 8U) CHECK(json[8] == '!');
    }
finish:
    free(again); free(json); free(r);
    return failed;
}
