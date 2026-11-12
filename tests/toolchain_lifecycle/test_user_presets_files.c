/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_lifecycle/test_user_presets_files.c
 *
 * PURPOSE:
 *   Preserve existing user files and retain partial new preset evidence on storage failure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/toolchain/user_presets.h"
#include "umicom/toolchain/build.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/clock.h"
#include "umicom/platform/output_file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static const char *mode;
static unsigned creates, writes, closes;
static UmiStatus FixtureCreate(const char *path, UmiOutputFile **out)
{
    ++creates;
    if (!strcmp(mode, "create-failure")) { *out = NULL; return UMI_STATUS_PERMISSION_DENIED; }
    if (!strcmp(mode, "racing-file")) {
        UmiStatus status = umi_fs_write_text(path, "racing owner");
        if (status != UMI_STATUS_OK) return status;
    }
    return UmiOutputFileCreate(path, out);
}
static UmiStatus FixtureWrite(UmiOutputFile *file, const void *bytes, size_t size)
{
    ++writes;
    if (!strcmp(mode, "write-failure")) {
        UmiStatus status = UmiOutputFileWrite(file, bytes, size < 8U ? size : 8U);
        return status == UMI_STATUS_OK ? UMI_STATUS_IO_ERROR : status;
    }
    return UmiOutputFileWrite(file, bytes, size);
}
static UmiStatus FixtureClose(UmiOutputFile *file)
{
    ++closes;
    UmiStatus status = UmiOutputFileClose(file);
    return status == UMI_STATUS_OK && !strcmp(mode, "close-failure") ? UMI_STATUS_IO_ERROR : status;
}
/* Inject failures at real exclusive files. Existing and partial bytes are read
 * back below; these cases do not merely compare a mocked success status. */
#define UmiOutputFileCreate FixtureCreate
#define UmiOutputFileWrite FixtureWrite
#define UmiOutputFileClose FixtureClose
#include "../../src/toolchain/user_presets.c"
#undef UmiOutputFileClose
#undef UmiOutputFileWrite
#undef UmiOutputFileCreate
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s line %d: %s\n", mode, __LINE__, #x); failed = 1; goto finish; } } while (0)
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    mode = argv[1];
    int failed = 0, owns = 0;
    char temp[UMI_PATH_CAPACITY], root[UMI_PATH_CAPACITY] = {0}, file[UMI_PATH_CAPACITY], leaf[160];
    char *readback = NULL;
    char child_root[UMI_PATH_CAPACITY];
    static char expected_json[UMI_BUILD_USER_PRESETS_CAPACITY];
    static UmiToolchainProfile profile; umi_toolchain_profile_init(&profile);
    profile.selected_c_compiler = UMI_TOOL_GCC;
    profile.tools[UMI_TOOL_GCC].state = UMI_TOOL_VALIDATED;
    profile.tools[UMI_TOOL_NINJA].state = UMI_TOOL_VALIDATED;
    profile.tools[UMI_TOOL_PKG_CONFIG].state = UMI_TOOL_VALIDATED;
    strcpy(profile.tools[UMI_TOOL_GCC].path, "/fixture/cc");
    strcpy(profile.tools[UMI_TOOL_NINJA].path, "/fixture/ninja");
    strcpy(profile.tools[UMI_TOOL_PKG_CONFIG].path, "/fixture/pkg");
    CHECK(UmiBuildUserPresetsFormat(&profile, expected_json, sizeof expected_json, NULL) == UMI_STATUS_OK);
    UmiClock clock = umi_clock_system();
    (void)snprintf(leaf, sizeof leaf, "umicom-presets-%s-%llu", mode, (unsigned long long)clock.monotonic_nanoseconds(&clock));
    umi_clock_dispose(&clock);
    CHECK(umi_fs_temp_directory(temp, sizeof temp) == UMI_STATUS_OK);
    CHECK(umi_path_join(temp, leaf, root, sizeof root) == UMI_STATUS_OK);
    CHECK(umi_path_is_within(temp, root) && !umi_path_equal(temp, root) && !umi_fs_exists(root));
    CHECK(umi_fs_make_directories(root) == UMI_STATUS_OK); owns = 1;
    CHECK(umi_path_join(root, "CMakeUserPresets.json", file, sizeof file) == UMI_STATUS_OK);
    const char *selected_root = root, *selected_path = file;
    UmiStatus expected = UMI_STATUS_OK;
    int preview = 0;
    unsigned expected_creates = 1U, expected_writes = 1U, expected_closes = 1U;
    if (!strcmp(mode, "new")) { }
    else if (!strcmp(mode, "preview")) { preview = 1; expected_creates = expected_writes = expected_closes = 0U; }
    else if (!strcmp(mode, "existing") || !strcmp(mode, "existing-preview")) {
        CHECK(umi_fs_write_text(file, "owner customization") == UMI_STATUS_OK);
        preview = !strcmp(mode, "existing-preview"); expected = UMI_STATUS_ALREADY_EXISTS;
        expected_creates = expected_writes = expected_closes = 0U;
    } else if (!strcmp(mode, "racing-file")) {
        expected = UMI_STATUS_ALREADY_EXISTS; expected_writes = expected_closes = 0U;
    } else if (!strcmp(mode, "write-failure") || !strcmp(mode, "close-failure")) expected = UMI_STATUS_IO_ERROR;
    else if (!strcmp(mode, "create-failure")) {
        expected = UMI_STATUS_PERMISSION_DENIED; expected_writes = expected_closes = 0U;
    } else if (!strcmp(mode, "invalid-profile")) {
        profile.structure_size = 0U; expected = UMI_STATUS_INVALID_ARGUMENT;
        expected_creates = expected_writes = expected_closes = 0U;
    } else if (!strcmp(mode, "relative-root")) {
        selected_root = "relative"; expected = UMI_STATUS_INVALID_ARGUMENT;
        expected_creates = expected_writes = expected_closes = 0U;
    } else if (!strcmp(mode, "outside-root")) {
        CHECK(umi_path_join(root, "child", child_root, sizeof child_root) == UMI_STATUS_OK);
        selected_root = child_root; expected = UMI_STATUS_INVALID_ARGUMENT;
        expected_creates = expected_writes = expected_closes = 0U;
    } else if (!strcmp(mode, "same-root")) {
        selected_path = root; expected = UMI_STATUS_INVALID_ARGUMENT;
        expected_creates = expected_writes = expected_closes = 0U;
    } else if (!strcmp(mode, "missing-parent")) {
        CHECK(umi_path_join(root, "missing/presets.json", file, sizeof file) == UMI_STATUS_OK);
        expected = UMI_STATUS_NOT_FOUND; expected_creates = expected_writes = expected_closes = 0U;
    } else if (!strcmp(mode, "invalid-preview")) {
        preview = 2; expected = UMI_STATUS_INVALID_ARGUMENT;
        expected_creates = expected_writes = expected_closes = 0U;
    } else if (!strcmp(mode, "legacy-replace")) {
        CHECK(umi_fs_write_text(file, "previous generated presets") == UMI_STATUS_OK);
        CHECK(umi_build_write_user_presets(&profile, root, file) == UMI_STATUS_OK);
        CHECK(umi_fs_read_text(file, &readback, NULL) == UMI_STATUS_OK);
        CHECK(!strcmp(readback, expected_json));
        goto finish;
    } else { failed = 2; goto finish; }
    CHECK(UmiBuildUserPresetsCreate(&profile, selected_root, selected_path, preview) == expected);
    CHECK(creates == expected_creates && writes == expected_writes && closes == expected_closes);
    if (!strcmp(mode, "new") || !strcmp(mode, "close-failure") || !strcmp(mode, "write-failure")) {
        CHECK(umi_fs_read_text(file, &readback, NULL) == UMI_STATUS_OK);
        if (!strcmp(mode, "write-failure")) CHECK(strlen(readback) == 8U && !memcmp(readback, expected_json, 8U));
        else CHECK(!strcmp(readback, expected_json));
    } else if (!strcmp(mode, "existing") || !strcmp(mode, "existing-preview") || !strcmp(mode, "racing-file")) {
        CHECK(umi_fs_read_text(file, &readback, NULL) == UMI_STATUS_OK);
        CHECK(!strcmp(readback, !strcmp(mode, "racing-file") ? "racing owner" : "owner customization"));
    } else CHECK(!umi_fs_exists(file));
finish:
    umi_fs_free_text(readback);
    if (owns && umi_path_is_within(temp, root) && !umi_path_equal(temp, root))
        if (umi_fs_remove_tree(root) != UMI_STATUS_OK) failed = 1;
    return failed;
}
