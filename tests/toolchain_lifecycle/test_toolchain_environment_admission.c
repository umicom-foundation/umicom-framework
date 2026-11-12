/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_lifecycle/test_toolchain_environment_admission.c
 *
 * PURPOSE:
 *   Verify complete profile environment construction and propagation of optional-tool errors.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/toolchain/environment.h"
#include "umicom/platform/filesystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *inherited_path = "synthetic/inherited";
static unsigned allocations, environment_reads, parent_calls;
static unsigned fail_parent;
static int refuse_allocation;
static void *StageAllocate(size_t count, size_t size)
{
    ++allocations;
    return refuse_allocation ? NULL : calloc(count, size);
}
static char *ReadEnvironment(const char *name)
{
    ++environment_reads;
    return !strcmp(name, "PATH") ? (char *)inherited_path : NULL;
}
static UmiStatus ParentPath(char *output, size_t capacity, const char *input)
{
    ++parent_calls;
    if (parent_calls == fail_parent) return UMI_STATUS_IO_ERROR;
    const char *expected = parent_calls == 1U ? "synthetic/java/bin/java" : "synthetic/java/bin";
    const char *result = parent_calls == 1U ? "synthetic/java/bin" : "synthetic/java";
    if (strcmp(input, expected) || strlen(result) >= capacity) return UMI_STATUS_INVALID_ARGUMENT;
    strcpy(output, result);
    return UMI_STATUS_OK;
}
/* Profile storage/accessors and environment construction remain real. Only
 * host environment, candidate allocation and path-parent failures are injected. */
#define calloc StageAllocate
#define getenv ReadEnvironment
#define umi_fs_parent ParentPath
#include "../../src/toolchain/environment.c"
#undef umi_fs_parent
#undef getenv
#undef calloc

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #condition); failed = 1; goto done; \
} } while (0)

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    int failed = 0;
    UmiToolchainProfile *profile = calloc(1U, sizeof *profile);
    UmiEnvironmentPlan *output = calloc(1U, sizeof *output);
    UmiEnvironmentPlan *before = calloc(1U, sizeof *before);
    char *long_path = malloc(UMI_ENVIRONMENT_VALUE_CAPACITY + 1U);
    if (!profile || !output || !before || !long_path) { failed = 1; goto done; }
    CHECK(umi_toolchain_profile_initialize(profile, sizeof *profile) == UMI_STATUS_OK);
    profile->selected_c_compiler = UMI_TOOL_GCC;
    profile->selected_cpp_compiler = UMI_TOOL_GXX;
    strcpy(profile->bin_directory, "synthetic/tools");
    strcpy(profile->prefix_directory, "synthetic/sdk");
    profile->tools[UMI_TOOL_GCC].state = UMI_TOOL_VALIDATED;
    strcpy(profile->tools[UMI_TOOL_GCC].path, "synthetic/gcc");
    CHECK(umi_environment_plan_add(output, "PREVIOUS", "retained") == UMI_STATUS_OK);
    *before = *output;
    const char *mode = argv[1];
    UmiStatus expected = UMI_STATUS_INVALID_ARGUMENT;
    int early = 0;
    if (!strcmp(mode, "valid") || !strcmp(mode, "java-parent-first") ||
        !strcmp(mode, "java-parent-second") || !strcmp(mode, "java-unterminated") ||
        !strcmp(mode, "java-newline")) {
        profile->tools[UMI_TOOL_JAVA].state = UMI_TOOL_VALIDATED;
        strcpy(profile->tools[UMI_TOOL_JAVA].path, "synthetic/java/bin/java");
        if (!strcmp(mode, "valid")) expected = UMI_STATUS_OK;
        else if (!strcmp(mode, "java-parent-first") || !strcmp(mode, "java-parent-second")) {
            fail_parent = !strcmp(mode, "java-parent-first") ? 1U : 2U;
            expected = UMI_STATUS_IO_ERROR;
        } else if (!strcmp(mode, "java-unterminated"))
            memset(profile->tools[UMI_TOOL_JAVA].path, 'x', sizeof profile->tools[UMI_TOOL_JAVA].path);
        else strcpy(profile->tools[UMI_TOOL_JAVA].path, "synthetic/java/bin/ja\nva");
    } else if (!strcmp(mode, "missing-optional")) expected = UMI_STATUS_OK;
    else if (!strcmp(mode, "missing-c")) {
        profile->tools[UMI_TOOL_GCC].state = UMI_TOOL_MISSING; expected = UMI_STATUS_NOT_FOUND;
    } else if (!strcmp(mode, "declared-count")) {
        profile->tool_count = (size_t)UMI_TOOL_GCC; expected = UMI_STATUS_NOT_FOUND;
    } else if (!strcmp(mode, "profile-size")) { --profile->structure_size; early = 1; }
    else if (!strcmp(mode, "profile-version")) { ++profile->api_version; early = 1; }
    else if (!strcmp(mode, "profile-count")) { profile->tool_count = UMI_TOOLCHAIN_PROFILE_TOOL_CAPACITY + 1U; early = 1; }
    else if (!strcmp(mode, "bin-unterminated")) { memset(profile->bin_directory, 'x', sizeof profile->bin_directory); early = 1; }
    else if (!strcmp(mode, "prefix-unterminated")) { memset(profile->prefix_directory, 'x', sizeof profile->prefix_directory); early = 1; }
    else if (!strcmp(mode, "c-unterminated"))
        memset(profile->tools[UMI_TOOL_GCC].path, 'x', sizeof profile->tools[UMI_TOOL_GCC].path);
    else if (!strcmp(mode, "cpp-unterminated")) {
        profile->tools[UMI_TOOL_GXX].state = UMI_TOOL_VALIDATED;
        memset(profile->tools[UMI_TOOL_GXX].path, 'x', sizeof profile->tools[UMI_TOOL_GXX].path);
    } else if (!strcmp(mode, "cpp-newline") || !strcmp(mode, "pkg-newline") ||
        !strcmp(mode, "ninja-newline") || !strcmp(mode, "rc-newline")) {
        UmiToolKind kind = !strcmp(mode, "cpp-newline") ? UMI_TOOL_GXX :
            !strcmp(mode, "pkg-newline") ? UMI_TOOL_PKG_CONFIG :
            !strcmp(mode, "ninja-newline") ? UMI_TOOL_NINJA : UMI_TOOL_WINDRES;
        profile->tools[kind].state = UMI_TOOL_VALIDATED;
        strcpy(profile->tools[kind].path, "synthetic/invalid\npath");
    } else if (!strcmp(mode, "prefix-newline")) strcpy(profile->prefix_directory, "invalid\nprefix");
    else if (!strcmp(mode, "allocation")) { refuse_allocation = 1; expected = UMI_STATUS_OUT_OF_MEMORY; }
    else if (!strcmp(mode, "path-long")) {
        memset(long_path, 'x', UMI_ENVIRONMENT_VALUE_CAPACITY); long_path[UMI_ENVIRONMENT_VALUE_CAPACITY] = 0;
        inherited_path = long_path; expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (!strcmp(mode, "path-unset") || !strcmp(mode, "path-empty")) {
        inherited_path = !strcmp(mode, "path-unset") ? NULL : ""; expected = UMI_STATUS_OK;
    } else { failed = 2; goto done; }
    CHECK(umi_environment_plan_from_toolchain(profile, output) == expected);
    if (expected != UMI_STATUS_OK) {
        CHECK(!memcmp(output, before, sizeof *output));
        if (early) CHECK(allocations == 0U && environment_reads == 0U && parent_calls == 0U);
        if (refuse_allocation) CHECK(allocations == 1U && environment_reads == 0U);
        if (!strcmp(mode, "java-unterminated") || !strcmp(mode, "java-newline")) CHECK(parent_calls == 0U);
        if (fail_parent) CHECK(parent_calls == fail_parent);
    } else {
        CHECK(umi_environment_plan_validate(output) == UMI_STATUS_OK);
        CHECK(umi_environment_plan_find(output, "PREVIOUS") == NULL);
        CHECK(!strcmp(umi_environment_plan_find(output, "CC"), "synthetic/gcc"));
        if (!strcmp(mode, "path-unset") || !strcmp(mode, "path-empty"))
            CHECK(!strcmp(umi_environment_plan_find(output, "PATH"), "synthetic/tools"));
        else {
#ifdef _WIN32
            CHECK(!strcmp(umi_environment_plan_find(output, "PATH"), "synthetic/tools;synthetic/inherited"));
#else
            CHECK(!strcmp(umi_environment_plan_find(output, "PATH"), "synthetic/tools:synthetic/inherited"));
#endif
        }
        if (!strcmp(mode, "valid")) {
            CHECK(!strcmp(umi_environment_plan_find(output, "JAVA_HOME"), "synthetic/java"));
            CHECK(parent_calls == 2U);
        } else CHECK(umi_environment_plan_find(output, "JAVA_HOME") == NULL);
        for (size_t i = 0; i < output->count; ++i) {
            CHECK(output->process_entries[i].name == output->entries[i].name);
            CHECK(output->process_entries[i].value == output->entries[i].value);
        }
        CHECK(allocations == 1U && environment_reads == 1U);
    }
done:
    free(long_path); free(before); free(output); free(profile);
    return failed;
}
