/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_lifecycle/test_cache_repair.c
 *
 * PURPOSE:
 *   Exercise repair admission and outcome reporting without filesystem mutation or tool execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/toolchain/build.h"
#include "umicom/toolchain/cmake_cache.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/clock.h"
#include "umicom/platform/workspace_rename.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define REQUIRE(x) do { if (!(x)) { fprintf(stderr, "%s line %d: %s\n", mode, __LINE__, #x); return 1; } } while (0)
#ifdef _WIN32
#define FIXTURE_ROOT "C:/fixture"
#define FIXTURE_COMPILER "C:/tools/cc.exe"
#else
#define FIXTURE_ROOT "/fixture"
#define FIXTURE_COMPILER "/tools/cc"
#endif
static const char *mode;
static unsigned reads, moves, opens;
static int graph_token;
static char graph_root[UMI_PATH_CAPACITY];
static UmiStatus FixtureRead(const char *path, UmiCMakeCache *out)
{
    ++reads;
    if (!strstr(path, "CMakeCache.txt")) return UMI_STATUS_INTERNAL_ERROR;
    if (!strcmp(mode, "missing")) return UMI_STATUS_NOT_FOUND;
    if (!strcmp(mode, "read-error")) return UMI_STATUS_IO_ERROR;
    if (!strcmp(mode, "read-busy")) return UMI_STATUS_BUSY;
    if (!strcmp(mode, "read-limit")) return UMI_STATUS_CAPACITY_EXCEEDED;
    const char *value = !strcmp(mode, "healthy") ? FIXTURE_COMPILER : FIXTURE_COMPILER "-old";
    char text[512];
    int n = snprintf(text, sizeof text, "%sCMAKE_C_COMPILER:FILEPATH=%s\n%s",
        !strcmp(mode, "comment-only") ? "# " : "", value,
        !strcmp(mode, "duplicate") ? "CMAKE_C_COMPILER:FILEPATH=/duplicate\n" : "");
    if (n < 0 || (size_t)n >= sizeof text) return UMI_STATUS_INTERNAL_ERROR;
    return UmiCMakeCacheParse(text, (size_t)n, out);
}
static UmiStatus FixtureCreate(UmiWorkspaceGraph **out)
{ *out = (UmiWorkspaceGraph *)(void *)&graph_token; return UMI_STATUS_OK; }
static UmiStatus FixtureOpen(UmiWorkspaceGraph *graph, const char *root, int trusted)
{
    (void)graph; ++opens;
    if (!trusted || strlen(root) >= sizeof graph_root) return UMI_STATUS_INVALID_ARGUMENT;
    strcpy(graph_root, root);
    return !strcmp(mode, "graph-failure") ? UMI_STATUS_NOT_FOUND : UMI_STATUS_OK;
}
static void FixtureDestroy(UmiWorkspaceGraph *graph) { (void)graph; }
static UmiStatus FixturePreview(const UmiWorkspaceGraph *graph, uint64_t revision,
    const char *source, const char *name, UmiWorkspaceRenameResult *out)
{
    (void)graph; (void)revision; memset(out, 0, sizeof *out);
    UmiStatus status = umi_path_join(graph_root, source, out->source, sizeof out->source);
    if (status == UMI_STATUS_OK) status = umi_path_join(graph_root, name, out->destination, sizeof out->destination);
    return status;
}
static UmiStatus FixtureMove(const UmiWorkspaceGraph *graph, uint64_t revision,
    const char *source, const char *name, UmiWorkspaceRenameResult *out)
{
    ++moves;
    UmiStatus status = FixturePreview(graph, revision, source, name, out);
    if (status != UMI_STATUS_OK) return status;
    if (!strcmp(mode, "racing-collision")) return UMI_STATUS_ALREADY_EXISTS;
    if (!strcmp(mode, "rename-failure")) return UMI_STATUS_PERMISSION_DENIED;
    out->renamed = 1; out->directory = 1;
    return !strcmp(mode, "close-failure") ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
}
static int FixtureExists(const char *path) { (void)path; return !strcmp(mode, "collision"); }
/* Only the filesystem/clock-independent observation boundaries are replaced.
 * Path admission and the shared selected-entry parser remain real code. */
#define UmiCMakeCacheRead FixtureRead
#define umi_workspace_graph_create FixtureCreate
#define umi_workspace_graph_open FixtureOpen
#define umi_workspace_graph_destroy FixtureDestroy
#define UmiWorkspaceRenamePreview FixturePreview
#define UmiWorkspaceRenameEntry FixtureMove
#define umi_fs_exists FixtureExists
#include "../../src/toolchain/cache_repair.c"
#undef umi_fs_exists
#undef UmiWorkspaceRenameEntry
#undef UmiWorkspaceRenamePreview
#undef umi_workspace_graph_destroy
#undef umi_workspace_graph_open
#undef umi_workspace_graph_create
#undef UmiCMakeCacheRead
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    mode = argv[1];
    static UmiToolchainProfile profile;
    umi_toolchain_profile_init(&profile);
    profile.selected_c_compiler = UMI_TOOL_GCC;
    profile.tools[UMI_TOOL_GCC].state = UMI_TOOL_VALIDATED;
    strcpy(profile.tools[UMI_TOOL_GCC].path, FIXTURE_COMPILER);
    const UmiToolchainProfile *selected = &profile;
    const char *build = FIXTURE_ROOT "/build";
    char long_path[UMI_PATH_CAPACITY + 1U], output[UMI_PATH_CAPACITY];
    strcpy(output, "previous observation");
    char *out = output; size_t capacity = sizeof output;
    int preview = 1;
    UmiStatus expected = UMI_STATUS_OK;
    unsigned expected_reads = 1U, expected_moves = 0U;
    int expect_path = 1;
    if (!strcmp(mode, "healthy") || !strcmp(mode, "missing")) expect_path = 0;
    else if (!strcmp(mode, "stale-preview") || !strcmp(mode, "prefix")) { }
    else if (!strcmp(mode, "stale-apply")) { preview = 0; expected_moves = 1U; }
    else if (!strcmp(mode, "comment-only")) { expected = UMI_STATUS_PARSE_ERROR; expect_path = 0; }
    else if (!strcmp(mode, "duplicate")) { expected = UMI_STATUS_ALREADY_EXISTS; expect_path = 0; }
    else if (!strcmp(mode, "read-error")) { expected = UMI_STATUS_IO_ERROR; expect_path = 0; }
    else if (!strcmp(mode, "read-busy")) { expected = UMI_STATUS_BUSY; expect_path = 0; }
    else if (!strcmp(mode, "read-limit")) { expected = UMI_STATUS_CAPACITY_EXCEEDED; expect_path = 0; }
    else if (!strcmp(mode, "invalid-flag")) { preview = 2; expected = UMI_STATUS_INVALID_ARGUMENT; expected_reads = 0; expect_path = 0; }
    else if (!strcmp(mode, "null-profile")) { selected = NULL; expected = UMI_STATUS_INVALID_ARGUMENT; expected_reads = 0; expect_path = 0; }
    else if (!strcmp(mode, "null-build")) { build = NULL; expected = UMI_STATUS_INVALID_ARGUMENT; expected_reads = 0; expect_path = 0; }
    else if (!strcmp(mode, "relative-build")) { build = "relative/build"; expected = UMI_STATUS_INVALID_ARGUMENT; expected_reads = 0; expect_path = 0; }
    else if (!strcmp(mode, "root-build")) {
#ifdef _WIN32
        build = "C:/";
#else
        build = "/";
#endif
        expected = UMI_STATUS_INVALID_ARGUMENT; expected_reads = 0; expect_path = 0;
    } else if (!strcmp(mode, "build-long") || !strcmp(mode, "join-limit")) {
        memset(long_path, 'a', sizeof long_path); memcpy(long_path, FIXTURE_ROOT "/", strlen(FIXTURE_ROOT "/"));
        long_path[!strcmp(mode, "build-long") ? UMI_PATH_CAPACITY : UMI_PATH_CAPACITY - 2U] = '\0';
        build = long_path; expected = UMI_STATUS_CAPACITY_EXCEEDED; expected_reads = 0; expect_path = 0;
    } else if (!strcmp(mode, "compiler-long")) {
        memset(profile.tools[UMI_TOOL_GCC].path, 'x', sizeof profile.tools[UMI_TOOL_GCC].path);
        expected = UMI_STATUS_CAPACITY_EXCEEDED; expected_reads = 0; expect_path = 0;
    } else if (!strcmp(mode, "compiler-relative")) {
        strcpy(profile.tools[UMI_TOOL_GCC].path, "cc"); expected = UMI_STATUS_INVALID_ARGUMENT; expected_reads = 0; expect_path = 0;
    } else if (!strcmp(mode, "compiler-missing")) {
        profile.tools[UMI_TOOL_GCC].state = UMI_TOOL_MISSING; expected = UMI_STATUS_NOT_FOUND; expected_reads = 0; expect_path = 0;
    } else if (!strcmp(mode, "compiler-kind")) {
        profile.tools[UMI_TOOL_GCC].kind = UMI_TOOL_NINJA; expected = UMI_STATUS_INVALID_ARGUMENT; expected_reads = 0; expect_path = 0;
    } else if (!strcmp(mode, "profile-size")) {
        profile.structure_size = 0U; expected = UMI_STATUS_NOT_FOUND; expected_reads = 0; expect_path = 0;
    } else if (!strcmp(mode, "output-pair-null")) {
        out = NULL; expected = UMI_STATUS_INVALID_ARGUMENT; expected_reads = 0; expect_path = 0;
    } else if (!strcmp(mode, "output-pair-zero")) {
        capacity = 0U; expected = UMI_STATUS_INVALID_ARGUMENT; expected_reads = 0; expect_path = 0;
    } else if (!strcmp(mode, "small-output")) {
        capacity = 2U; preview = 0; expected = UMI_STATUS_CAPACITY_EXCEEDED; expect_path = 0;
    } else if (!strcmp(mode, "no-output")) { out = NULL; capacity = 0U; preview = 0; expected_moves = 1U; expect_path = 0; }
    else if (!strcmp(mode, "collision")) { preview = 0; expected = UMI_STATUS_ALREADY_EXISTS; expect_path = 0; }
    else if (!strcmp(mode, "racing-collision")) { preview = 0; expected_moves = 1U; expected = UMI_STATUS_ALREADY_EXISTS; expect_path = 0; }
    else if (!strcmp(mode, "rename-failure")) { preview = 0; expected_moves = 1U; expected = UMI_STATUS_PERMISSION_DENIED; expect_path = 0; }
    else if (!strcmp(mode, "close-failure")) { preview = 0; expected_moves = 1U; expected = UMI_STATUS_IO_ERROR; }
    else if (!strcmp(mode, "graph-failure")) { expected = UMI_STATUS_NOT_FOUND; expect_path = 0; }
    else return 2;
    REQUIRE(umi_build_repair_cache(selected, build, out, capacity, preview) == expected);
    REQUIRE(reads == expected_reads && moves == expected_moves);
    if (out != NULL && capacity != 0U) {
        if (expect_path) REQUIRE(strstr(output, ".recovery-") != NULL);
        else REQUIRE(output[0] == '\0');
    } else REQUIRE(!strcmp(output, "previous observation"));
    if (!expected_reads) REQUIRE(opens == 0U);
    return 0;
}
