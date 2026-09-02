/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/directory.h
 *
 * PURPOSE:
 *   Define deterministic directory traversal and file metadata contracts used
 *   by workspaces, indexes, search, watchers and repository tools.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_DIRECTORY_H
#define UMICOM_PLATFORM_DIRECTORY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/platform/path.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named file kind values accepted by this public contract.
 */
typedef enum UmiFileKind {
    UMI_FILE_KIND_UNKNOWN = 0,
    UMI_FILE_KIND_REGULAR = 1,
    UMI_FILE_KIND_DIRECTORY = 2,
    UMI_FILE_KIND_SYMBOLIC_LINK = 3,
    UMI_FILE_KIND_OTHER = 4
} UmiFileKind;

/**
 * Represent the file info data shared with callers of this public contract.
 */
typedef struct UmiFileInfo {
    char path[UMI_PATH_CAPACITY];
    char name[UMI_PATH_CAPACITY];
    UmiFileKind kind;
    uint64_t size;
    uint64_t modified_nanoseconds;
    size_t depth;
    int hidden;
} UmiFileInfo;

/**
 * Represent the directory walk options data shared with callers of this public contract.
 */
typedef struct UmiDirectoryWalkOptions {
    size_t max_depth;
    int recursive;
    int include_files;
    int include_directories;
    int include_hidden;
    int follow_symbolic_links;
} UmiDirectoryWalkOptions;

typedef UmiStatus (*UmiDirectoryVisitor)(const UmiFileInfo *info,
                                         void *user_data);

/**
 * Provide the directory walk options default operation used by this module and its client
 * applications.
 */
UmiDirectoryWalkOptions umi_directory_walk_options_default(void);
/**
 * Provide the directory stat operation used by this module and its client applications.
 */
UmiStatus umi_directory_stat(const char *path, UmiFileInfo *out_info);
/**
 * Provide the directory walk operation used by this module and its client applications.
 */
UmiStatus umi_directory_walk(const char *root,
                             const UmiDirectoryWalkOptions *options,
                             UmiDirectoryVisitor visitor,
                             void *user_data);
/**
 * Return the number of records represented by directory without changing their state.
 */
UmiStatus umi_directory_count(const char *root,
                              const UmiDirectoryWalkOptions *options,
                              size_t *out_count);

#ifdef __cplusplus
}
#endif

#endif
