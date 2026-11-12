/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_kit/test_environment_admission.c
 *
 * PURPOSE:
 *   Check bounded kit fields, override failures and complete environment publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/toolchain/kit_environment.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned allocations, profile_calls;
static int refuse_allocation, fill_capacity, fail_profile;
static void *StageAllocate(size_t count, size_t size)
{
    ++allocations;
    return refuse_allocation ? NULL : calloc(count, size);
}
static UmiStatus PrepareProfile(const UmiToolchainProfile *profile, UmiEnvironmentPlan *plan)
{
    ++profile_calls;
    if (!profile) return UMI_STATUS_INVALID_ARGUMENT;
    if (fill_capacity) {
        for (size_t i = 0; i < UMI_ENVIRONMENT_PLAN_MAX - 1U; ++i) {
            char name[32]; snprintf(name, sizeof name, "BASE_%zu", i);
            UmiStatus status = umi_environment_plan_add(plan, name, "profile");
            if (status != UMI_STATUS_OK) return status;
        }
    } else {
        UmiStatus status = umi_environment_plan_add(plan, "BASE", "retained");
        if (status == UMI_STATUS_OK) status = umi_environment_plan_add(plan, "CC", "profile-compiler");
        if (status != UMI_STATUS_OK) return status;
    }
    return fail_profile ? UMI_STATUS_UNAVAILABLE : UMI_STATUS_OK;
}
/* Exercise real kit validation, overrides and environment projection. Profile
 * preparation has separate regressions; inject its status and capacity here. */
#define calloc StageAllocate
#define umi_environment_plan_from_toolchain PrepareProfile
#include "../../src/toolchain/kit/kit_environment.c"
#undef umi_environment_plan_from_toolchain
#undef calloc

typedef struct TextField { const char *name; size_t offset, capacity; } TextField;
#define FIELD(name) {#name, offsetof(UmiToolchainKitSnapshot, name), sizeof(((UmiToolchainKitSnapshot *)0)->name)}
static const TextField fields[] = {
    FIELD(id), FIELD(display_name), FIELD(profile_id), FIELD(sdk_id),
    FIELD(host_triple), FIELD(target_triple), FIELD(sysroot), FIELD(cmake_generator),
    FIELD(cmake_toolchain_file), FIELD(environment_profile), FIELD(c_compiler),
    FIELD(cpp_compiler), FIELD(assembler), FIELD(linker), FIELD(debugger),
    FIELD(cmake), FIELD(ctest), FIELD(ninja), FIELD(git), FIELD(python), FIELD(java), FIELD(javac)
};
#undef FIELD
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #condition); failed = 1; goto done; \
} } while (0)

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    int failed = 0, early = 0;
    UmiToolchainKitSnapshot *kit = calloc(1U, sizeof *kit);
    UmiToolchainProfile *profile = calloc(1U, sizeof *profile);
    UmiEnvironmentPlan *output = calloc(1U, sizeof *output);
    UmiEnvironmentPlan *before = calloc(1U, sizeof *before);
    if (!kit || !profile || !output || !before) { failed = 1; goto done; }
    umi_toolchain_kit_init(kit, "kit.practice", "Practice kit");
    strcpy(kit->profile_id, "practice");
    strcpy(kit->target_triple, "synthetic-target");
    strcpy(kit->c_compiler, "kit-compiler");
    strcpy(kit->javac, "synthetic-javac");
    CHECK(umi_environment_plan_add(output, "PREVIOUS", "retained") == UMI_STATUS_OK);
    *before = *output;
    const UmiToolchainKitSnapshot *input = kit;
    UmiEnvironmentPlan *destination = output;
    const UmiToolchainProfile *base = NULL;
    UmiStatus expected = UMI_STATUS_OK;
    const char *mode = argv[1];
    if (!strncmp(mode, "field.", 6U)) {
        size_t found = 0;
        for (size_t i = 0; i < sizeof fields / sizeof fields[0]; ++i) {
            if (strcmp(mode + 6, fields[i].name)) continue;
            memset((char *)kit + fields[i].offset, 'x', fields[i].capacity);
            found = 1U; break;
        }
        CHECK(found);
        expected = UMI_STATUS_INVALID_ARGUMENT; early = 1;
    } else if (!strcmp(mode, "no-profile")) {
        /* The kit is sufficient for this structural in-memory projection. */
    } else if (!strcmp(mode, "profile")) base = profile;
    else if (!strcmp(mode, "profile-failure")) {
        base = profile; fail_profile = 1; expected = UMI_STATUS_UNAVAILABLE;
    } else if (!strcmp(mode, "capacity")) {
        base = profile; fill_capacity = 1; expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (!strcmp(mode, "allocation")) {
        base = profile; refuse_allocation = 1; expected = UMI_STATUS_OUT_OF_MEMORY;
    } else if (!strcmp(mode, "late-invalid")) {
        strcpy(kit->javac, "invalid\ncompiler"); expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (!strcmp(mode, "null-kit")) {
        input = NULL; expected = UMI_STATUS_INVALID_ARGUMENT; early = 1;
    } else if (!strcmp(mode, "null-output")) {
        destination = NULL; expected = UMI_STATUS_INVALID_ARGUMENT; early = 1;
    } else if (!strcmp(mode, "small-record")) {
        --kit->struct_size; expected = UMI_STATUS_INVALID_ARGUMENT; early = 1;
    } else if (!strcmp(mode, "version")) {
        ++kit->api_version; expected = UMI_STATUS_INVALID_ARGUMENT; early = 1;
    } else if (!strcmp(mode, "missing-id")) {
        kit->id[0] = 0; expected = UMI_STATUS_INVALID_ARGUMENT; early = 1;
    } else if (!strcmp(mode, "ready-without-compiler")) {
        kit->state = UMI_TOOLCHAIN_KIT_READY; kit->c_compiler[0] = 0;
        expected = UMI_STATUS_INVALID_STATE; early = 1;
    } else { failed = 2; goto done; }
    CHECK(umi_toolchain_kit_environment_plan(input, base, destination) == expected);
    if (expected != UMI_STATUS_OK) {
        CHECK(!memcmp(output, before, sizeof *output));
        if (early) CHECK(allocations == 0U && profile_calls == 0U);
        if (refuse_allocation) CHECK(allocations == 1U && profile_calls == 0U);
        if (base && !refuse_allocation) CHECK(profile_calls == 1U);
    } else {
        CHECK(allocations == 1U && profile_calls == (base ? 1U : 0U));
        CHECK(umi_environment_plan_find(output, "PREVIOUS") == NULL);
        CHECK(!strcmp(umi_environment_plan_find(output, "CC"), "kit-compiler"));
        CHECK(!strcmp(umi_environment_plan_find(output, "UMICOM_TOOLCHAIN_KIT"), "kit.practice"));
        if (base) CHECK(!strcmp(umi_environment_plan_find(output, "BASE"), "retained"));
        for (size_t i = 0; i < output->count; ++i) {
            CHECK(output->process_entries[i].name == output->entries[i].name);
            CHECK(output->process_entries[i].value == output->entries[i].value);
        }
    }
done:
    free(before); free(output); free(profile); free(kit);
    return failed;
}
