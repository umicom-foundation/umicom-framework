/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/file_index.h
 *
 * PURPOSE:
 *   Define a bounded, thread-safe workspace file index with deterministic
 *   snapshots and lightweight name/path queries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_FILE_INDEX_H
#define UMICOM_PLATFORM_FILE_INDEX_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/platform/path.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_FILE_INDEX_NAME_CAPACITY 512U
#define UMI_FILE_INDEX_EXTENSION_CAPACITY 64U

/**
 * Represent the file index config data shared with callers of this public contract.
 */
typedef struct UmiFileIndexConfig {
    const char *root;
    size_t maximum_files;
    size_t maximum_depth;
    int include_hidden;
} UmiFileIndexConfig;

/**
 * Represent the file index entry data shared with callers of this public contract.
 */
typedef struct UmiFileIndexEntry {
    char path[UMI_PATH_CAPACITY];
    char relative_path[UMI_PATH_CAPACITY];
    char name[UMI_FILE_INDEX_NAME_CAPACITY];
    char extension[UMI_FILE_INDEX_EXTENSION_CAPACITY];
    uint64_t size;
    uint64_t modified_nanoseconds;
} UmiFileIndexEntry;

/**
 * Represent the file index stats data shared with callers of this public contract.
 */
typedef struct UmiFileIndexStats {
    char root[UMI_PATH_CAPACITY];
    size_t files;
    size_t capacity;
    uint64_t revision;
    uint64_t rebuilds;
    uint64_t updates;
    uint64_t removals;
} UmiFileIndexStats;

/**
 * Represent the file index data shared with callers of this public contract.
 */
typedef struct UmiFileIndex UmiFileIndex;

/**
 * Provide the file index config default operation used by this module and its client
 * applications.
 */
UmiFileIndexConfig umi_file_index_config_default(const char *root);
/**
 * Initialise file index from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_file_index_create(const UmiFileIndexConfig *config,
                                UmiFileIndex **out_index);
/**
 * Release or reset state held by file index so the same storage can be reused safely.
 */
void umi_file_index_destroy(UmiFileIndex *index);
/**
 * Provide the file index set root operation used by this module and its client
 * applications.
 */
UmiStatus umi_file_index_set_root(UmiFileIndex *index,
                                  const char *root);
/**
 * Release or reset state held by file index so the same storage can be reused safely.
 */
UmiStatus umi_file_index_clear(UmiFileIndex *index);
/**
 * Provide the file index rebuild operation used by this module and its client
 * applications.
 */
UmiStatus umi_file_index_rebuild(UmiFileIndex *index);
/**
 * Provide the file index update operation used by this module and its client applications.
 */
UmiStatus umi_file_index_update(UmiFileIndex *index, const char *path);
/**
 * Remove file index while keeping the remaining records in a valid and discoverable state.
 */
UmiStatus umi_file_index_remove(UmiFileIndex *index, const char *path);
/**
 * Find file index while leaving the underlying catalogue or model owned by this module.
 */
UmiStatus umi_file_index_at(const UmiFileIndex *index,
                            size_t position,
                            UmiFileIndexEntry *out_entry);
/**
 * Find file index while leaving the underlying catalogue or model owned by this module.
 */
UmiStatus umi_file_index_find(const UmiFileIndex *index,
                              const char *query,
                              int case_sensitive,
                              UmiFileIndexEntry *out_entries,
                              size_t capacity,
                              size_t *out_count);
/**
 * Provide the file index stats operation used by this module and its client applications.
 */
UmiFileIndexStats umi_file_index_stats(const UmiFileIndex *index);

#ifdef __cplusplus
}
#endif

#endif
