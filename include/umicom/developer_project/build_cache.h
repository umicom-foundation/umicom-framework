/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/build_cache.h
 * PURPOSE: Inspect configured CMake identity and toolchain metadata without executing project scripts.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEVELOPER_PROJECT_BUILD_CACHE_H
#define UMICOM_DEVELOPER_PROJECT_BUILD_CACHE_H
#include "umicom/build/profile.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_PROJECT_BUILD_CACHE_BYTE_LIMIT (8U * 1024U * 1024U)
    typedef struct UmiProjectBuildCache
    {
        char source_directory[UMI_BUILD_PATH_CAPACITY];
        char build_directory[UMI_BUILD_PATH_CAPACITY];
        char generator[UMI_BUILD_NAME_CAPACITY];
        char compiler[UMI_BUILD_PATH_CAPACITY];
        char configuration[UMI_BUILD_NAME_CAPACITY];
        char configurations[UMI_BUILD_PATH_CAPACITY];
        char install_directory[UMI_BUILD_PATH_CAPACITY];
        char toolchain_file[UMI_BUILD_PATH_CAPACITY];
        int source_matches;
        int build_matches;
    } UmiProjectBuildCache;
    /* Inspect selected ordinary CMAKE_* cache entries. Unknown lines and entries
 * are ignored; this is not a general CMake cache editor or script evaluator.
 * Required entries: HOME_DIRECTORY, CACHEFILE_DIR and GENERATOR. Optional
 * entries are empty when absent. Values are literal, never shell-expanded.
 * The entire input must be UTF-8 (an initial BOM is accepted), without NULs.
 * Reject duplicate selected keys, unsupported types and control bytes in their
 * values. Both expected directories must be absolute. Identity comparisons
 * are lexical; aliases are not resolved. A mismatch is a successful inspection
 * with a false match flag, so a presenter can explain the recorded identity.
 * Output changes only on complete success; it must not overlap input storage. */
    UmiStatus UmiProjectBuildCacheParse(const void *bytes, size_t size, const char *sourceDirectory,
                                        const char *buildDirectory, UmiProjectBuildCache *out);
    /* Read CMakeCache.txt as a regular file using the shared bounded reader. The
 * final component cannot be a link/device; parent aliases may be followed.
 * No cache write, directory creation, project execution or trust grant occurs.
 * Run this blocking read on a worker when called from an interactive UI. A
 * readable cache is a snapshot, not proof that Configure or Build succeeded. */
    UmiStatus UmiProjectBuildCacheRead(const char *sourceDirectory, const char *buildDirectory,
                                       UmiProjectBuildCache *out);
#ifdef __cplusplus
}
#endif
#endif
