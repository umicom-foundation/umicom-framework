/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_kit/test_mutation_admission.c
 *
 * PURPOSE:
 *   Verify bounded incremental kit updates, revision exhaustion and partial discovery publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/toolchain/kit.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int refuse_candidate;
static void *CandidateAllocate(size_t count, size_t size)
{
    return refuse_candidate ? NULL : calloc(count, size);
}
/* Exercise the production update paths; substitute only candidate allocation
 * so a refused allocation can be checked without launching a compiler. */
#define calloc CandidateAllocate
#include "../../src/toolchain/kit/kit.c"
#undef calloc

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #condition); failed = 1; goto done; \
} } while (0)
typedef struct TextField { const char *name; size_t offset, capacity; } TextField;
#define CROSS_FIELD(name) {#name, offsetof(UmiCrossToolchainSnapshot, name), sizeof(((UmiCrossToolchainSnapshot *)0)->name)}
static const TextField cross_fields[] = {
    CROSS_FIELD(target_triple), CROSS_FIELD(compiler_path), CROSS_FIELD(cpp_compiler_path),
    CROSS_FIELD(assembler_path), CROSS_FIELD(linker_path), CROSS_FIELD(debugger_path), CROSS_FIELD(sysroot)
};
#undef CROSS_FIELD

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    int failed = 0;
    UmiToolchainKitSnapshot *kit = calloc(1U, sizeof *kit);
    UmiToolchainKitSnapshot *before = calloc(1U, sizeof *before);
    UmiCrossToolchainSnapshot *cross = calloc(1U, sizeof *cross);
    UmiCrossToolchainSnapshot *cross_before = calloc(1U, sizeof *cross_before);
    UmiPythonRuntimeSnapshot *runtime = calloc(1U, sizeof *runtime);
    UmiPythonRuntimeSnapshot *runtime_before = calloc(1U, sizeof *runtime_before);
    char *text = calloc(UMI_TOOL_PATH_CAPACITY + 1U, 1U);
    if (!kit || !before || !cross || !cross_before || !runtime || !runtime_before || !text) {
        failed = 1; goto done;
    }
    umi_toolchain_kit_init(kit, "practice", "Practice kit");
    strcpy(kit->profile_id, "practice-profile");
    strcpy(kit->host_triple, "native-host");
    strcpy(kit->target_triple, "native-host");
    strcpy(kit->c_compiler, "previous/cc");
    strcpy(kit->cpp_compiler, "previous/cxx");
    strcpy(kit->python, "previous/python");
    strcpy(kit->sysroot, "old/sysroot");
    strcpy(kit->cmake_toolchain_file, "old/toolchain.cmake");
    strcpy(kit->environment_profile, "old/environment");
    kit->state = UMI_TOOLCHAIN_KIT_READY;
    kit->capabilities = UMI_TOOLCHAIN_KIT_CAPABILITY_C23;
    kit->revision = 19U;
    cross->struct_size = (uint32_t)sizeof *cross;
    cross->api_version = UMI_CROSS_TOOLCHAIN_API_VERSION;
    cross->state = UMI_CROSS_TOOLCHAIN_READY;
    strcpy(cross->target_triple, "cross-target");
    strcpy(cross->compiler_path, "cross/cc");
    strcpy(cross->assembler_path, "cross/as");
    strcpy(cross->linker_path, "cross/ld");
    strcpy(cross->debugger_path, "cross/gdb");
    strcpy(cross->sysroot, "cross/sysroot");
    runtime->struct_size = (uint32_t)sizeof *runtime;
    runtime->api_version = UMI_PYTHON_RUNTIME_API_VERSION;
    runtime->available = 1;
    strcpy(runtime->executable, "synthetic/python");
    const char *mode = argv[1];
    UmiStatus expected = UMI_STATUS_OK, actual;
    if (!strncmp(mode, "cross.", 6U)) {
        const char *detail = mode + 6U;
        const UmiCrossToolchainSnapshot *input = cross;
        int publishes = 0;
        if (!strcmp(detail, "ready")) publishes = 1;
        else if (!strcmp(detail, "partial")) {
            cross->state = UMI_CROSS_TOOLCHAIN_PARTIAL; expected = UMI_STATUS_UNAVAILABLE; publishes = 1;
        } else if (!strcmp(detail, "unavailable")) {
            cross->state = UMI_CROSS_TOOLCHAIN_UNAVAILABLE; expected = UMI_STATUS_UNAVAILABLE; publishes = 1;
        } else if (!strcmp(detail, "unknown")) {
            cross->state = (UmiCrossToolchainState)99; expected = UMI_STATUS_INVALID_ARGUMENT;
        } else if (!strcmp(detail, "empty-target")) {
            cross->target_triple[0] = '\0'; expected = UMI_STATUS_INVALID_ARGUMENT;
        } else if (!strcmp(detail, "null")) { input = NULL; expected = UMI_STATUS_INVALID_ARGUMENT; }
        else if (!strcmp(detail, "size")) { --cross->struct_size; expected = UMI_STATUS_INVALID_ARGUMENT; }
        else if (!strcmp(detail, "version")) { ++cross->api_version; expected = UMI_STATUS_INVALID_ARGUMENT; }
        else if (!strcmp(detail, "allocation")) { refuse_candidate = 1; expected = UMI_STATUS_OUT_OF_MEMORY; }
        else if (!strcmp(detail, "exhausted")) { kit->revision = UINT64_MAX; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
        else if (!strcmp(detail, "kit-storage")) { memset(kit->java, 'x', sizeof kit->java); expected = UMI_STATUS_INVALID_ARGUMENT; }
        else if (!strcmp(detail, "last-revision")) { kit->revision = UINT64_MAX - 1U; publishes = 1; }
        else if (!strncmp(detail, "text.", 5U)) {
            size_t found = 0U;
            for (size_t i = 0U; i < sizeof cross_fields / sizeof cross_fields[0]; ++i) {
                if (!strcmp(detail + 5U, cross_fields[i].name)) {
                    memset((char *)cross + cross_fields[i].offset, 'x', cross_fields[i].capacity); ++found;
                }
            }
            CHECK(found == 1U); expected = UMI_STATUS_INVALID_ARGUMENT;
        } else { failed = 2; goto done; }
        *before = *kit; *cross_before = *cross;
        actual = umi_toolchain_kit_apply_cross_toolchain(kit, input);
        CHECK(actual == expected);
        CHECK(memcmp(cross, cross_before, sizeof *cross) == 0);
        if (!publishes) CHECK(memcmp(kit, before, sizeof *kit) == 0);
        else {
            CHECK(kit->revision == before->revision + 1U);
            CHECK(!strcmp(kit->target_triple, cross->target_triple));
            CHECK(!strcmp(kit->c_compiler, cross->compiler_path));
            CHECK(!strcmp(kit->cpp_compiler, before->cpp_compiler));
            CHECK(!strcmp(kit->assembler, cross->assembler_path));
            CHECK(!strcmp(kit->linker, cross->linker_path));
            CHECK(!strcmp(kit->debugger, cross->debugger_path));
            CHECK(!strcmp(kit->sysroot, cross->sysroot));
            CHECK(!strcmp(kit->python, before->python));
            CHECK(!strcmp(kit->id, before->id));
            CHECK(kit->state == (cross->state == UMI_CROSS_TOOLCHAIN_READY ? UMI_TOOLCHAIN_KIT_READY :
                cross->state == UMI_CROSS_TOOLCHAIN_PARTIAL ? UMI_TOOLCHAIN_KIT_DEGRADED : UMI_TOOLCHAIN_KIT_UNAVAILABLE));
            CHECK(umi_toolchain_kit_supports(kit, UMI_TOOLCHAIN_KIT_CAPABILITY_C23 | UMI_TOOLCHAIN_KIT_CAPABILITY_CROSS_COMPILE));
            CHECK(!umi_toolchain_kit_supports(kit, UMI_TOOLCHAIN_KIT_CAPABILITY_RUN));
        }
    } else if (!strncmp(mode, "runtime.", 8U)) {
        const char *detail = mode + 8U;
        const UmiPythonRuntimeSnapshot *input = runtime;
        if (!strcmp(detail, "valid")) { /* Available need not mean version-validated. */ }
        else if (!strcmp(detail, "unavailable")) { runtime->available = 0; expected = UMI_STATUS_NOT_FOUND; }
        else if (!strcmp(detail, "empty")) { runtime->executable[0] = '\0'; expected = UMI_STATUS_NOT_FOUND; }
        else if (!strcmp(detail, "text")) { memset(runtime->executable, 'x', sizeof runtime->executable); expected = UMI_STATUS_INVALID_ARGUMENT; }
        else if (!strcmp(detail, "null")) { input = NULL; expected = UMI_STATUS_INVALID_ARGUMENT; }
        else if (!strcmp(detail, "size")) { --runtime->struct_size; expected = UMI_STATUS_INVALID_ARGUMENT; }
        else if (!strcmp(detail, "version")) { ++runtime->api_version; expected = UMI_STATUS_INVALID_ARGUMENT; }
        else if (!strcmp(detail, "exhausted")) { kit->revision = UINT64_MAX; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
        else { failed = 2; goto done; }
        *before = *kit; *runtime_before = *runtime;
        actual = umi_toolchain_kit_apply_python_runtime(kit, input);
        CHECK(actual == expected);
        CHECK(memcmp(runtime, runtime_before, sizeof *runtime) == 0);
        if (actual != UMI_STATUS_OK) CHECK(memcmp(kit, before, sizeof *kit) == 0);
        else {
            CHECK(!strcmp(kit->python, runtime->executable));
            CHECK(kit->revision == before->revision + 1U);
            CHECK(umi_toolchain_kit_supports(kit, UMI_TOOLCHAIN_KIT_CAPABILITY_PYTHON));
            CHECK(!strcmp(kit->c_compiler, before->c_compiler));
        }
    } else if (!strncmp(mode, "query.", 6U)) {
        int expected_cross = 0;
        if (!strcmp(mode, "query.cross")) { strcpy(kit->target_triple, "other-target"); expected_cross = 1; }
        else if (!strcmp(mode, "query.text")) memset(kit->target_triple, 'x', sizeof kit->target_triple);
        else if (!strcmp(mode, "query.metadata")) --kit->struct_size;
        else { failed = 2; goto done; }
        *before = *kit;
        CHECK(umi_toolchain_kit_is_cross_compile(kit) == expected_cross);
        CHECK(memcmp(kit, before, sizeof *kit) == 0);
    } else {
        UmiStatus (*setter)(UmiToolchainKitSnapshot *, const char *) = NULL;
        char *field = NULL;
        size_t capacity = 0U;
        const char *detail = strchr(mode, '.');
        CHECK(detail != NULL); ++detail;
        if (!strncmp(mode, "sysroot.", 8U)) {
            setter = umi_toolchain_kit_set_sysroot; field = kit->sysroot; capacity = sizeof kit->sysroot;
        } else if (!strncmp(mode, "cmake.", 6U)) {
            setter = umi_toolchain_kit_set_cmake_toolchain_file; field = kit->cmake_toolchain_file; capacity = sizeof kit->cmake_toolchain_file;
        } else if (!strncmp(mode, "environment.", 12U)) {
            setter = umi_toolchain_kit_set_environment_profile; field = kit->environment_profile; capacity = sizeof kit->environment_profile;
        } else { failed = 2; goto done; }
        const char *input = "replacement";
        if (!strcmp(detail, "valid")) {
            /* Incremental assembly remains supported: readiness is separate. */
            kit->profile_id[0] = '\0'; kit->target_triple[0] = '\0'; kit->state = UMI_TOOLCHAIN_KIT_UNKNOWN;
        } else if (!strcmp(detail, "clear")) input = "";
        else if (!strcmp(detail, "alias")) input = field + 4U;
        else if (!strcmp(detail, "limit")) { memset(text, 'x', capacity - 1U); input = text; }
        else if (!strcmp(detail, "oversize")) { memset(text, 'x', capacity); input = text; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
        else if (!strcmp(detail, "exhausted")) { kit->revision = UINT64_MAX; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
        else if (!strcmp(detail, "null")) { input = NULL; expected = UMI_STATUS_INVALID_ARGUMENT; }
        else if (!strcmp(detail, "malformed")) { memset(kit->java, 'x', sizeof kit->java); expected = UMI_STATUS_INVALID_ARGUMENT; }
        else if (!strcmp(detail, "metadata")) { ++kit->api_version; expected = UMI_STATUS_INVALID_ARGUMENT; }
        else { failed = 2; goto done; }
        /* Save expected text independently before testing a borrowed slice. */
        char *wanted = NULL;
        if (expected == UMI_STATUS_OK) {
            wanted = malloc(strlen(input) + 1U);
            CHECK(wanted != NULL); strcpy(wanted, input);
        }
        *before = *kit;
        actual = setter(kit, input);
        int text_matches = wanted == NULL || !strcmp(field, wanted);
        free(wanted);
        CHECK(actual == expected);
        if (actual != UMI_STATUS_OK) CHECK(memcmp(kit, before, sizeof *kit) == 0);
        else {
            CHECK(text_matches); CHECK(kit->revision == before->revision + 1U);
            /* Exactly the selected field and revision may change. */
            size_t offset = (size_t)(field - (char *)kit);
            memcpy((char *)before + offset, field, capacity); before->revision = kit->revision;
            CHECK(memcmp(kit, before, sizeof *kit) == 0);
        }
    }
done:
    free(text); free(runtime_before); free(runtime); free(cross_before); free(cross); free(before); free(kit);
    return failed;
}
