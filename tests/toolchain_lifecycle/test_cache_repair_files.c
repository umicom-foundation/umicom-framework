/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_lifecycle/test_cache_repair_files.c
 *
 * PURPOSE:
 *   Verify recovery preserves source and destination bytes using owner-run temporary directories.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/toolchain/build.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/clock.h"
#include "umicom/platform/workspace_rename.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static const char *mode;
static UmiClock FixtureClock(void) { return umi_clock_fake(41U); }
static UmiStatus FixtureMove(const UmiWorkspaceGraph *graph, uint64_t revision,
    const char *source, const char *name, UmiWorkspaceRenameResult *result)
{
    if (!strcmp(mode, "racing-collision")) {
        UmiStatus status = UmiWorkspaceRenamePreview(graph, revision, source, name, result);
        if (status == UMI_STATUS_OK) status = umi_fs_make_directories(result->destination);
        if (status != UMI_STATUS_OK) return status;
    }
    return UmiWorkspaceRenameEntry(graph, revision, source, name, result);
}
/* Fix only the recovery suffix and inject one destination race. Cache reads,
 * graph ownership and the native no-replacement rename are real operations. */
#define umi_clock_system FixtureClock
#define UmiWorkspaceRenameEntry FixtureMove
#include "../../src/toolchain/cache_repair.c"
#undef UmiWorkspaceRenameEntry
#undef umi_clock_system
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s line %d: %s\n", mode, __LINE__, #x); failed = 1; goto finish; } } while (0)
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    mode = argv[1];
    if (strcmp(mode, "healthy") && strcmp(mode, "missing") && strcmp(mode, "preview") && strcmp(mode, "apply") && strcmp(mode, "collision") && strcmp(mode, "racing-collision") && strcmp(mode, "malformed") && strcmp(mode, "cache-directory")) return 2;
    int failed = 0, owns = 0;
    char temp[UMI_PATH_CAPACITY], root[UMI_PATH_CAPACITY] = {0}, build[UMI_PATH_CAPACITY];
    char cache[UMI_PATH_CAPACITY], recovery[UMI_PATH_CAPACITY], output[UMI_PATH_CAPACITY];
    char leaf[160], content[UMI_TOOL_PATH_CAPACITY + 64U];
    char *readback = NULL;
    UmiClock clock = umi_clock_system();
    (void)snprintf(leaf, sizeof leaf, "umicom-cache-repair-%s-%" PRIu64, mode, clock.monotonic_nanoseconds(&clock));
    umi_clock_dispose(&clock);
    CHECK(umi_fs_temp_directory(temp, sizeof temp) == UMI_STATUS_OK);
    CHECK(umi_path_join(temp, leaf, root, sizeof root) == UMI_STATUS_OK);
    CHECK(umi_path_is_within(temp, root) && !umi_path_equal(temp, root) && !umi_fs_exists(root));
    CHECK(umi_fs_make_directories(root) == UMI_STATUS_OK); owns = 1;
    CHECK(umi_path_join(root, "build", build, sizeof build) == UMI_STATUS_OK);
    CHECK(umi_path_join(root, "build.recovery-41", recovery, sizeof recovery) == UMI_STATUS_OK);
    CHECK(umi_path_join(build, "CMakeCache.txt", cache, sizeof cache) == UMI_STATUS_OK);
    CHECK(umi_fs_make_directories(build) == UMI_STATUS_OK);
    static UmiToolchainProfile profile; umi_toolchain_profile_init(&profile);
    profile.selected_c_compiler = UMI_TOOL_GCC; profile.tools[UMI_TOOL_GCC].state = UMI_TOOL_VALIDATED;
    CHECK(umi_path_join(root, "compiler", profile.tools[UMI_TOOL_GCC].path, sizeof profile.tools[UMI_TOOL_GCC].path) == UMI_STATUS_OK);
    int n = snprintf(content, sizeof content, "CMAKE_C_COMPILER:FILEPATH=%s%s\n", profile.tools[UMI_TOOL_GCC].path,
        !strcmp(mode, "healthy") ? "" : "-old");
    CHECK(n > 0 && (size_t)n < sizeof content);
    if (!strcmp(mode, "malformed")) strcpy(content, "CMAKE_C_COMPILER:BOOL=ON\n");
    if (!strcmp(mode, "cache-directory")) CHECK(umi_fs_make_directories(cache) == UMI_STATUS_OK);
    else if (strcmp(mode, "missing")) CHECK(umi_fs_write_text(cache, content) == UMI_STATUS_OK);
    if (!strcmp(mode, "collision")) CHECK(umi_fs_make_directories(recovery) == UMI_STATUS_OK);
    UmiStatus status = umi_build_repair_cache(&profile, build, output, sizeof output, !strcmp(mode, "preview"));
    if (status == UMI_STATUS_NOT_IMPLEMENTED && (!strcmp(mode, "apply") || !strcmp(mode, "racing-collision"))) {
        failed = 77; goto finish;
    }
    if (!strcmp(mode, "apply")) {
        CHECK(status == UMI_STATUS_OK && umi_path_equal(output, recovery));
        CHECK(!umi_fs_exists(build) && umi_fs_is_directory(recovery));
        CHECK(umi_path_join(recovery, "CMakeCache.txt", cache, sizeof cache) == UMI_STATUS_OK);
    } else {
        CHECK(umi_fs_is_directory(build));
        if (!strcmp(mode, "collision") || !strcmp(mode, "racing-collision")) {
            CHECK(status == UMI_STATUS_ALREADY_EXISTS && output[0] == '\0');
            CHECK(umi_fs_is_directory(recovery));
        } else if (!strcmp(mode, "malformed")) CHECK(status == UMI_STATUS_PARSE_ERROR && output[0] == '\0');
        else if (!strcmp(mode, "cache-directory")) CHECK(status != UMI_STATUS_OK && output[0] == '\0');
        else {
            CHECK(status == UMI_STATUS_OK);
            if (!strcmp(mode, "preview")) CHECK(umi_path_equal(output, recovery) && !umi_fs_exists(recovery));
            else CHECK(output[0] == '\0');
        }
    }
    if (strcmp(mode, "missing") && strcmp(mode, "cache-directory")) {
        CHECK(umi_fs_read_text(cache, &readback, NULL) == UMI_STATUS_OK);
        CHECK(!strcmp(readback, content));
    }
finish:
    umi_fs_free_text(readback);
    /* Only a newly created, non-root child of the discovered temporary folder
     * is removed. Never reuse a pre-existing test directory as owned storage. */
    if (owns && umi_path_is_within(temp, root) && !umi_path_equal(temp, root))
        if (umi_fs_remove_tree(root) != UMI_STATUS_OK) failed = 1;
    return failed;
}
