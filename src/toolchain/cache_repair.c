/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/cache_repair.c
 *
 * PURPOSE:
 *   Inspect stale compiler caches and preserve build directories with no destination replacement.
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
#include "umicom/base/text.h"
#include "umicom/platform/clock.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/output_file.h"
#include "umicom/platform/workspace_rename.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

UmiStatus umi_build_repair_cache(const UmiToolchainProfile *profile,
    const char *build_directory, char *out_recovery_path, size_t capacity, int dry_run)
{
    /* A deterministic empty result means no recovery was proposed/performed.
     * Inputs must not alias the optional output storage. */
    if (out_recovery_path != NULL && capacity != 0U) out_recovery_path[0] = '\0';
    if (profile == NULL || build_directory == NULL || (dry_run != 0 && dry_run != 1) ||
        (out_recovery_path == NULL) != (capacity == 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    char supplied[UMI_PATH_CAPACITY], build[UMI_PATH_CAPACITY], cache_path[UMI_PATH_CAPACITY];
    UmiStatus status = umi_text_replace(supplied, sizeof supplied, build_directory);
    if (status == UMI_STATUS_OK) status = UmiOutputFileValidatePath(supplied);
    if (status == UMI_STATUS_OK) status = umi_path_normalise(supplied, build, sizeof build);
    if (status == UMI_STATUS_OK) status = umi_path_join(build, "CMakeCache.txt", cache_path, sizeof cache_path);
    if (status != UMI_STATUS_OK) return status;
    char parent[UMI_PATH_CAPACITY], basename[UMI_PATH_CAPACITY];
    status = umi_path_parent(build, parent, sizeof parent);
    if (status == UMI_STATUS_OK && umi_path_equal(build, parent)) status = UMI_STATUS_INVALID_ARGUMENT;
    if (status == UMI_STATUS_OK) status = umi_path_basename(build, basename, sizeof basename);
    if (status != UMI_STATUS_OK) return status;
    const UmiToolInfo *compiler = umi_toolchain_profile_c_compiler(profile);
    if (compiler == NULL || compiler->state != UMI_TOOL_VALIDATED) return UMI_STATUS_NOT_FOUND;
    if (compiler->kind != profile->selected_c_compiler ||
        (compiler->kind != UMI_TOOL_GCC && compiler->kind != UMI_TOOL_CLANG &&
         compiler->kind != UMI_TOOL_MSVC_CL)) return UMI_STATUS_INVALID_ARGUMENT;
    char compiler_path[UMI_TOOL_PATH_CAPACITY];
    status = umi_text_replace(compiler_path, sizeof compiler_path, compiler->path);
    if (status == UMI_STATUS_OK) status = UmiOutputFileValidatePath(compiler_path);
    if (status != UMI_STATUS_OK) return status;
    UmiCMakeCache *cache = calloc(1U, sizeof *cache);
    if (cache == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = UmiCMakeCacheRead(cache_path, cache);
    if (status == UMI_STATUS_NOT_FOUND) { free(cache); return UMI_STATUS_OK; }
    /* Missing/ambiguous compiler metadata is an inspection failure, not
     * permission to move an arbitrary directory. Match a complete entry,
     * with platform lexical path equality rather than substring searching. */
    if (status == UMI_STATUS_OK && (!(cache->present & UMI_CMAKE_CACHE_COMPILER) || !cache->compiler[0]))
        status = UMI_STATUS_PARSE_ERROR;
    int current = status == UMI_STATUS_OK && umi_path_equal(cache->compiler, compiler_path);
    free(cache);
    if (status != UMI_STATUS_OK || current) return status;
    UmiClock clock = umi_clock_system();
    uint64_t stamp = clock.wall_nanoseconds(&clock);
    umi_clock_dispose(&clock);
    char recovery_name[UMI_PATH_CAPACITY];
    int written = snprintf(recovery_name, sizeof recovery_name, "%s.recovery-%" PRIu64, basename, stamp);
    if (written < 0 || (size_t)written >= sizeof recovery_name) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiWorkspaceGraph *graph = NULL;
    status = umi_workspace_graph_create(&graph);
    /* This short-lived graph scopes the explicitly requested operation to
     * this build directory's existing parent. It performs no discovery and
     * changes no persistent workspace trust setting. Callers must own and
     * serialize the source/parent; this is not a hostile-filesystem sandbox. */
    if (status == UMI_STATUS_OK) status = umi_workspace_graph_open(graph, parent, 1);
    UmiWorkspaceRenameResult result = {0};
    if (status == UMI_STATUS_OK)
        status = UmiWorkspaceRenamePreview(graph, 0U, basename, recovery_name, &result);
    if (status == UMI_STATUS_OK && out_recovery_path != NULL && strlen(result.destination) >= capacity)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK && umi_fs_exists(result.destination)) status = UMI_STATUS_ALREADY_EXISTS;
    if (status == UMI_STATUS_OK && !dry_run)
        status = UmiWorkspaceRenameEntry(graph, 0U, basename, recovery_name, &result);
    /* A completed native rename remains authoritative even if closing a
     * handle subsequently fails. Report its path so callers never retry it
     * blindly. The native operation itself prevents destination replacement. */
    if (out_recovery_path != NULL && (status == UMI_STATUS_OK || result.renamed))
        memcpy(out_recovery_path, result.destination, strlen(result.destination) + 1U);
    umi_workspace_graph_destroy(graph);
    return status;
}
