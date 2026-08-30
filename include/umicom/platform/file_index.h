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

typedef struct UmiFileIndexConfig {
    const char *root;
    size_t maximum_files;
    size_t maximum_depth;
    int include_hidden;
} UmiFileIndexConfig;

typedef struct UmiFileIndexEntry {
    char path[UMI_PATH_CAPACITY];
    char relative_path[UMI_PATH_CAPACITY];
    char name[UMI_FILE_INDEX_NAME_CAPACITY];
    char extension[UMI_FILE_INDEX_EXTENSION_CAPACITY];
    uint64_t size;
    uint64_t modified_nanoseconds;
} UmiFileIndexEntry;

typedef struct UmiFileIndexStats {
    char root[UMI_PATH_CAPACITY];
    size_t files;
    size_t capacity;
    uint64_t revision;
    uint64_t rebuilds;
    uint64_t updates;
    uint64_t removals;
} UmiFileIndexStats;

typedef struct UmiFileIndex UmiFileIndex;

UmiFileIndexConfig umi_file_index_config_default(const char *root);
UmiStatus umi_file_index_create(const UmiFileIndexConfig *config,
                                UmiFileIndex **out_index);
void umi_file_index_destroy(UmiFileIndex *index);
UmiStatus umi_file_index_set_root(UmiFileIndex *index,
                                  const char *root);
UmiStatus umi_file_index_clear(UmiFileIndex *index);
UmiStatus umi_file_index_rebuild(UmiFileIndex *index);
UmiStatus umi_file_index_update(UmiFileIndex *index, const char *path);
UmiStatus umi_file_index_remove(UmiFileIndex *index, const char *path);
UmiStatus umi_file_index_at(const UmiFileIndex *index,
                            size_t position,
                            UmiFileIndexEntry *out_entry);
UmiStatus umi_file_index_find(const UmiFileIndex *index,
                              const char *query,
                              int case_sensitive,
                              UmiFileIndexEntry *out_entries,
                              size_t capacity,
                              size_t *out_count);
UmiFileIndexStats umi_file_index_stats(const UmiFileIndex *index);

#ifdef __cplusplus
}
#endif

#endif
