/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_kit/test_profile_admission.c
 *
 * PURPOSE:
 *   Verify declared profile access and all-or-nothing developer-kit construction.
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

static unsigned allocations;
static int refuse_allocation;
static void *StageAllocate(size_t count, size_t size)
{
    ++allocations;
    return refuse_allocation ? NULL : calloc(count, size);
}
/* Only candidate allocation is substituted. Profile access and all kit
 * copying, capability calculation and structural admission remain real. */
#define calloc StageAllocate
#include "../../src/toolchain/kit/kit.c"
#undef calloc

typedef struct ProfileText { const char *name; size_t offset, capacity; } ProfileText;
#define FIELD(name) {#name, offsetof(UmiToolchainProfile, name), sizeof(((UmiToolchainProfile *)0)->name)}
static const ProfileText text_fields[] = {
    FIELD(profile_id), FIELD(display_name), FIELD(target_triple), FIELD(generator)
};
#undef FIELD
typedef struct ToolCase { const char *name; UmiToolKind kind; } ToolCase;
static const ToolCase tool_fields[] = {
    {"c", UMI_TOOL_GCC}, {"cpp", UMI_TOOL_GXX}, {"linker", UMI_TOOL_MSVC_LINK},
    {"debugger", UMI_TOOL_GDB}, {"cmake", UMI_TOOL_CMAKE}, {"ctest", UMI_TOOL_CTEST},
    {"ninja", UMI_TOOL_NINJA}, {"git", UMI_TOOL_GIT}, {"java", UMI_TOOL_JAVA},
    {"javac", UMI_TOOL_JAVAC}
};
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #condition); failed = 1; goto done; \
} } while (0)

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    int failed = 0, early = 0;
    UmiToolchainProfile *profile = calloc(1U, sizeof *profile);
    UmiToolchainProfile *original = calloc(1U, sizeof *original);
    UmiToolchainKitSnapshot *kit = calloc(1U, sizeof *kit);
    UmiToolchainKitSnapshot *before = calloc(1U, sizeof *before);
    UmiSdkSnapshot *sdk = calloc(1U, sizeof *sdk);
    if (!profile || !original || !kit || !before || !sdk) { failed = 1; goto done; }
    CHECK(umi_toolchain_profile_initialize(profile, sizeof *profile) == UMI_STATUS_OK);
    strcpy(profile->profile_id, "practice");
    strcpy(profile->display_name, "Practice compiler");
    strcpy(profile->target_triple, "synthetic-target");
    profile->selected_c_compiler = UMI_TOOL_GCC;
    profile->selected_cpp_compiler = UMI_TOOL_GXX;
    profile->tools[UMI_TOOL_GCC].state = UMI_TOOL_VALIDATED;
    strcpy(profile->tools[UMI_TOOL_GCC].path, "synthetic/gcc");
    profile->complete = 1; profile->c23_capable = 1;
    /* Preserve existing SDK input compatibility: this constructor consumes
     * availability and bounded identity, not the SDK discovery metadata. */
    sdk->available = 1; strcpy(sdk->id, "synthetic-sdk");
    umi_toolchain_kit_init(kit, "previous", "Previous kit");
    *before = *kit;
    const char *mode = argv[1], *host = "synthetic-host";
    char long_host[UMI_TOOLCHAIN_KIT_TRIPLE_CAPACITY + 1U];
    UmiStatus expected = UMI_STATUS_OK;
    const UmiToolchainProfile *input = profile;
    if (!strncmp(mode, "accessor.", 9U)) {
        UmiToolKind kind = UMI_TOOL_GCC;
        const UmiToolInfo *wanted = NULL;
        if (!strcmp(mode, "accessor.zero")) profile->tool_count = 0U;
        else if (!strcmp(mode, "accessor.boundary")) profile->tool_count = (size_t)kind;
        else if (!strcmp(mode, "accessor.last")) {
            profile->tool_count = (size_t)kind + 1U; wanted = &profile->tools[kind];
        } else if (!strcmp(mode, "accessor.negative")) kind = (UmiToolKind)-1;
        else if (!strcmp(mode, "accessor.unknown")) kind = UMI_TOOL_COUNT;
        else if (!strcmp(mode, "accessor.count")) profile->tool_count = UMI_TOOLCHAIN_PROFILE_TOOL_CAPACITY + 1U;
        else if (!strcmp(mode, "accessor.version")) ++profile->api_version;
        else if (!strcmp(mode, "accessor.null")) input = NULL;
        else { failed = 2; goto done; }
        *original = *profile;
        CHECK(umi_toolchain_profile_tool(input, kind) == wanted);
        CHECK(umi_toolchain_profile_tool_mutable(input ? profile : NULL, kind) == wanted);
        CHECK(!memcmp(profile, original, sizeof *profile));
        CHECK(allocations == 0U);
        goto done;
    }
    if (!strncmp(mode, "text.", 5U)) {
        size_t found = 0;
        for (size_t i = 0; i < sizeof text_fields / sizeof text_fields[0]; ++i) {
            if (strcmp(mode + 5, text_fields[i].name)) continue;
            memset((char *)profile + text_fields[i].offset, 'x', text_fields[i].capacity);
            found = 1U; break;
        }
        CHECK(found); expected = UMI_STATUS_INVALID_ARGUMENT; early = 1;
    } else if (!strncmp(mode, "tool.", 5U)) {
        size_t found = 0;
        for (size_t i = 0; i < sizeof tool_fields / sizeof tool_fields[0]; ++i) {
            if (strcmp(mode + 5, tool_fields[i].name)) continue;
            UmiToolKind kind = tool_fields[i].kind;
            profile->tools[kind].state = UMI_TOOL_VALIDATED;
            memset(profile->tools[kind].path, 'x', sizeof profile->tools[kind].path);
            if (kind == UMI_TOOL_MSVC_LINK) profile->family = UMI_TOOLCHAIN_MSVC;
            found = 1U; break;
        }
        CHECK(found); expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (!strcmp(mode, "valid")) { }
    else if (!strcmp(mode, "native")) host = "synthetic-target";
    else if (!strcmp(mode, "fallback-display")) profile->display_name[0] = 0;
    else if (!strcmp(mode, "fallback-target")) profile->target_triple[0] = 0;
    else if (!strcmp(mode, "host-alias")) { profile->target_triple[0] = 0; host = kit->display_name; }
    else if (!strcmp(mode, "missing-target")) {
        profile->target_triple[0] = 0; host = NULL; expected = UMI_STATUS_NOT_FOUND;
    } else if (!strcmp(mode, "found-compiler")) profile->tools[UMI_TOOL_GCC].state = UMI_TOOL_FOUND;
    else if (!strcmp(mode, "missing-compiler")) {
        profile->tools[UMI_TOOL_GCC].state = UMI_TOOL_MISSING; profile->complete = 0;
    } else if (!strcmp(mode, "ready-without-compiler")) {
        profile->tools[UMI_TOOL_GCC].state = UMI_TOOL_MISSING; expected = UMI_STATUS_INVALID_STATE;
    } else if (!strcmp(mode, "declared-count")) {
        profile->tool_count = (size_t)UMI_TOOL_GCC; expected = UMI_STATUS_INVALID_STATE;
    } else if (!strcmp(mode, "profile-size")) { --profile->structure_size; expected = UMI_STATUS_INVALID_ARGUMENT; early = 1; }
    else if (!strcmp(mode, "profile-version")) { ++profile->api_version; expected = UMI_STATUS_INVALID_ARGUMENT; early = 1; }
    else if (!strcmp(mode, "profile-count")) { profile->tool_count = UMI_TOOLCHAIN_PROFILE_TOOL_CAPACITY + 1U; expected = UMI_STATUS_INVALID_ARGUMENT; early = 1; }
    else if (!strcmp(mode, "null-profile")) { input = NULL; expected = UMI_STATUS_INVALID_ARGUMENT; early = 1; }
    else if (!strcmp(mode, "missing-profile-id")) { profile->profile_id[0] = 0; expected = UMI_STATUS_INVALID_ARGUMENT; early = 1; }
    else if (!strcmp(mode, "long-id")) {
        memset(profile->profile_id, 'x', UMI_TOOLCHAIN_KIT_ID_CAPACITY);
        profile->profile_id[UMI_TOOLCHAIN_KIT_ID_CAPACITY] = 0;
        expected = UMI_STATUS_CAPACITY_EXCEEDED; early = 1;
    } else if (!strcmp(mode, "long-host")) {
        memset(long_host, 'x', sizeof long_host - 1U); long_host[sizeof long_host - 1U] = 0;
        host = long_host; expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (!strcmp(mode, "sdk-text")) {
        memset(sdk->id, 'x', sizeof sdk->id); expected = UMI_STATUS_INVALID_ARGUMENT; early = 1;
    } else if (!strcmp(mode, "allocation")) {
        refuse_allocation = 1; expected = UMI_STATUS_OUT_OF_MEMORY;
    } else { failed = 2; goto done; }
    *original = *profile;
    CHECK(umi_toolchain_kit_from_profile(kit, input, sdk, host) == expected);
    CHECK(!memcmp(profile, original, sizeof *profile));
    if (expected != UMI_STATUS_OK) {
        CHECK(!memcmp(kit, before, sizeof *kit));
        CHECK(allocations == (early ? 0U : 1U));
    } else {
        CHECK(allocations == 1U);
        CHECK(umi_toolchain_kit_validate(kit) == UMI_STATUS_OK);
        CHECK(!strcmp(kit->id, "kit.practice"));
        CHECK(!strcmp(kit->profile_id, "practice"));
        CHECK(!strcmp(kit->sdk_id, "synthetic-sdk"));
        CHECK(!strcmp(kit->cmake_generator, "Ninja"));
        CHECK(!strcmp(kit->display_name, !strcmp(mode, "fallback-display") ? "practice" : "Practice compiler"));
        CHECK(!strcmp(kit->target_triple, !strcmp(mode, "fallback-target") ? "synthetic-host" :
            !strcmp(mode, "host-alias") ? "Previous kit" : "synthetic-target"));
        if (!strcmp(mode, "missing-compiler")) {
            CHECK(kit->state == UMI_TOOLCHAIN_KIT_UNAVAILABLE);
            CHECK(!kit->c_compiler[0]);
            CHECK(!(kit->capabilities & UMI_TOOLCHAIN_KIT_CAPABILITY_BUILD));
        } else {
            CHECK(kit->state == UMI_TOOLCHAIN_KIT_READY);
            CHECK(!strcmp(kit->c_compiler, "synthetic/gcc"));
            CHECK(!strcmp(kit->assembler, "synthetic/gcc"));
            CHECK(!strcmp(kit->linker, "synthetic/gcc"));
        }
        if (!strcmp(mode, "native") || !strcmp(mode, "fallback-target") || !strcmp(mode, "host-alias"))
            CHECK(kit->capabilities & UMI_TOOLCHAIN_KIT_CAPABILITY_RUN);
        else CHECK(kit->capabilities & UMI_TOOLCHAIN_KIT_CAPABILITY_CROSS_COMPILE);
    }
done:
    free(sdk); free(before); free(kit); free(original); free(profile);
    return failed;
}
