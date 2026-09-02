/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/file_index.c
 *
 * PURPOSE:
 *   Implement workspace indexing from the portable directory service and keep
 *   sorted file metadata current after watcher events or explicit refreshes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/file_index.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/platform/directory.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/threading.h"

struct UmiFileIndex {
    UmiFileIndexConfig config;
    char root[UMI_PATH_CAPACITY];
    UmiFileIndexEntry *entries;
    size_t count;
    UmiMutex *mutex;
    UmiFileIndexStats stats;
};

/* Provide the copy bounded operation used by this module and its client applications. */
static void copy_bounded(char *destination,
                         size_t capacity,
                         const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL) {
        destination[0] = '\0';
        return;
    }
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) length = capacity - 1U;
    (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

/* Provide the compare entries operation used by this module and its client applications. */
static int compare_entries(const void *left, const void *right)
{
    const UmiFileIndexEntry *a = (const UmiFileIndexEntry *)left;
    const UmiFileIndexEntry *b = (const UmiFileIndexEntry *)right;
#ifdef _WIN32
    return _stricmp(a->relative_path, b->relative_path);
#else
    return strcmp(a->relative_path, b->relative_path);
#endif
}

/* Provide the contains text operation used by this module and its client applications. */
static int contains_text(const char *text,
                         const char *query,
                         int case_sensitive)
{
    size_t text_length;
    size_t query_length;
    size_t start;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || query == NULL) return 0;
    text_length = strlen(text);
    query_length = strlen(query);
    /* Apply this branch only when its contract condition is satisfied. */
    if (query_length == 0U) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (query_length > text_length) return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (start = 0U; start + query_length <= text_length; ++start) {
        size_t index;
        int match = 1;
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < query_length; ++index) {
            char left = text[start + index];
            char right = query[index];
            /* Apply this branch only when its contract condition is satisfied. */
            if (!case_sensitive) {
                left = (char)tolower((unsigned char)left);
                right = (char)tolower((unsigned char)right);
            }
            /* Apply this branch only when its contract condition is satisfied. */
            if (left != right) {
                match = 0;
                break;
            }
        }
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (match) return 1;
    }
    return 0;
}

/*
 * Provide the file index config default operation used by this module and its client
 * applications.
 */
UmiFileIndexConfig umi_file_index_config_default(const char *root)
{
    UmiFileIndexConfig config;
    config.root = root;
    config.maximum_files = 100000U;
    config.maximum_depth = 64U;
    config.include_hidden = 0;
    return config;
}

/*
 * Initialise file index from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_file_index_create(const UmiFileIndexConfig *config,
                                UmiFileIndex **out_index)
{
    UmiFileIndexConfig effective;
    UmiFileIndex *index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (config == NULL || config->root == NULL || out_index == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_index = NULL;
    effective = *config;
    /* Apply this branch only when its contract condition is satisfied. */
    if (effective.maximum_files == 0U) effective.maximum_files = 100000U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (effective.maximum_depth == 0U) effective.maximum_depth = 64U;
    index = (UmiFileIndex *)calloc(1U, sizeof(*index));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    index->config = effective;
    status = umi_path_normalise(config->root,
                                index->root,
                                sizeof(index->root));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK ||
        umi_mutex_create(&index->mutex) != UMI_STATUS_OK) {
        umi_file_index_destroy(index);
        return status != UMI_STATUS_OK ? status : UMI_STATUS_OUT_OF_MEMORY;
    }
    index->config.root = index->root;
    index->entries = (UmiFileIndexEntry *)calloc(effective.maximum_files,
                                                  sizeof(*index->entries));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index->entries == NULL) {
        umi_file_index_destroy(index);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    (void)snprintf(index->stats.root,
                   sizeof(index->stats.root),
                   "%s",
                   index->root);
    index->stats.capacity = effective.maximum_files;
    index->stats.revision = 1U;
    *out_index = index;
    return UMI_STATUS_OK;
}

/* Release or reset state held by file index so the same storage can be reused safely. */
void umi_file_index_destroy(UmiFileIndex *index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index == NULL) return;
    umi_mutex_destroy(index->mutex);
    free(index->entries);
    free(index);
}

/*
 * Provide the file index set root operation used by this module and its client
 * applications.
 */
UmiStatus umi_file_index_set_root(UmiFileIndex *index,
                                  const char *root)
{
    char normalised[UMI_PATH_CAPACITY];
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index == NULL || root == NULL || root[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_path_normalise(root, normalised, sizeof(normalised));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_fs_is_directory(normalised)) return UMI_STATUS_NOT_FOUND;

    (void)umi_mutex_lock(index->mutex);
    (void)snprintf(index->root, sizeof(index->root), "%s", normalised);
    index->config.root = index->root;
    index->count = 0U;
    index->stats.files = 0U;
    (void)snprintf(index->stats.root,
                   sizeof(index->stats.root),
                   "%s",
                   index->root);
    index->stats.revision += 1U;
    (void)umi_mutex_unlock(index->mutex);
    return UMI_STATUS_OK;
}

/* Release or reset state held by file index so the same storage can be reused safely. */
UmiStatus umi_file_index_clear(UmiFileIndex *index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(index->mutex);
    index->count = 0U;
    index->stats.files = 0U;
    index->stats.revision += 1U;
    (void)umi_mutex_unlock(index->mutex);
    return UMI_STATUS_OK;
}

typedef struct BuildContext {
    UmiFileIndex *index;
    size_t count;
} BuildContext;

/* Provide the build visitor operation used by this module and its client applications. */
static UmiStatus build_visitor(const UmiFileInfo *info, void *user_data)
{
    BuildContext *context = (BuildContext *)user_data;
    UmiFileIndexEntry *entry;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (info->kind != UMI_FILE_KIND_REGULAR) return UMI_STATUS_OK;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (context->count >= context->index->config.maximum_files) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    entry = &context->index->entries[context->count++];
    (void)memset(entry, 0, sizeof(*entry));
    (void)snprintf(entry->path, sizeof(entry->path), "%s", info->path);
    (void)umi_path_relative(context->index->root,
                            info->path,
                            entry->relative_path,
                            sizeof(entry->relative_path));
    copy_bounded(entry->name, sizeof(entry->name), info->name);
    (void)umi_path_extension(info->path,
                             entry->extension,
                             sizeof(entry->extension));
    entry->size = info->size;
    entry->modified_nanoseconds = info->modified_nanoseconds;
    return UMI_STATUS_OK;
}

/*
 * Provide the file index rebuild operation used by this module and its client
 * applications.
 */
UmiStatus umi_file_index_rebuild(UmiFileIndex *index)
{
    UmiDirectoryWalkOptions options;
    BuildContext context;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(index->mutex);
    context.index = index;
    context.count = 0U;
    options = umi_directory_walk_options_default();
    options.max_depth = index->config.maximum_depth;
    options.include_hidden = index->config.include_hidden;
    options.include_files = 1;
    options.include_directories = 0;
    status = umi_directory_walk(index->root,
                                &options,
                                build_visitor,
                                &context);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        index->count = context.count;
        qsort(index->entries,
              index->count,
              sizeof(*index->entries),
              compare_entries);
        index->stats.files = index->count;
        index->stats.revision += 1U;
        index->stats.rebuilds += 1U;
    }
    (void)umi_mutex_unlock(index->mutex);
    return status;
}

/* Provide the find path operation used by this module and its client applications. */
static size_t find_path(const UmiFileIndex *index, const char *path)
{
    size_t position;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < index->count; ++position) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_path_equal(index->entries[position].path, path)) {
            return position;
        }
    }
    return SIZE_MAX;
}

/* Provide the file index update operation used by this module and its client applications. */
UmiStatus umi_file_index_update(UmiFileIndex *index, const char *path)
{
    UmiFileInfo info;
    size_t position;
    UmiFileIndexEntry entry;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index == NULL || path == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_directory_stat(path, &info);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (info.kind != UMI_FILE_KIND_REGULAR ||
        !umi_path_is_within(index->root, info.path)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(&entry, 0, sizeof(entry));
    (void)snprintf(entry.path, sizeof(entry.path), "%s", info.path);
    (void)umi_path_relative(index->root,
                            info.path,
                            entry.relative_path,
                            sizeof(entry.relative_path));
    copy_bounded(entry.name, sizeof(entry.name), info.name);
    (void)umi_path_extension(info.path,
                             entry.extension,
                             sizeof(entry.extension));
    entry.size = info.size;
    entry.modified_nanoseconds = info.modified_nanoseconds;
    (void)umi_mutex_lock(index->mutex);
    position = find_path(index, info.path);
    /* Apply this branch only when its contract condition is satisfied. */
    if (position == SIZE_MAX) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index->count >= index->config.maximum_files) {
            (void)umi_mutex_unlock(index->mutex);
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        position = index->count++;
    }
    index->entries[position] = entry;
    qsort(index->entries,
          index->count,
          sizeof(*index->entries),
          compare_entries);
    index->stats.files = index->count;
    index->stats.revision += 1U;
    index->stats.updates += 1U;
    (void)umi_mutex_unlock(index->mutex);
    return UMI_STATUS_OK;
}

/* Remove file index while keeping the remaining records in a valid and discoverable state. */
UmiStatus umi_file_index_remove(UmiFileIndex *index, const char *path)
{
    size_t position;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index == NULL || path == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(index->mutex);
    position = find_path(index, path);
    /* Apply this branch only when its contract condition is satisfied. */
    if (position == SIZE_MAX) {
        (void)umi_mutex_unlock(index->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position + 1U < index->count) {
        (void)memmove(&index->entries[position],
                      &index->entries[position + 1U],
                      (index->count - position - 1U) * sizeof(index->entries[0]));
    }
    index->count -= 1U;
    index->stats.files = index->count;
    index->stats.revision += 1U;
    index->stats.removals += 1U;
    (void)umi_mutex_unlock(index->mutex);
    return UMI_STATUS_OK;
}

/* Find file index while leaving the underlying catalogue or model owned by this module. */
UmiStatus umi_file_index_at(const UmiFileIndex *index,
                            size_t position,
                            UmiFileIndexEntry *out_entry)
{
    UmiFileIndex *mutable_index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index == NULL || out_entry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    mutable_index = (UmiFileIndex *)index;
    (void)umi_mutex_lock(mutable_index->mutex);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position >= index->count) {
        (void)umi_mutex_unlock(mutable_index->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    *out_entry = index->entries[position];
    (void)umi_mutex_unlock(mutable_index->mutex);
    return UMI_STATUS_OK;
}

/* Find file index while leaving the underlying catalogue or model owned by this module. */
UmiStatus umi_file_index_find(const UmiFileIndex *index,
                              const char *query,
                              int case_sensitive,
                              UmiFileIndexEntry *out_entries,
                              size_t capacity,
                              size_t *out_count)
{
    UmiFileIndex *mutable_index;
    size_t position;
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index == NULL || query == NULL || out_count == NULL ||
        (capacity > 0U && out_entries == NULL)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    mutable_index = (UmiFileIndex *)index;
    (void)umi_mutex_lock(mutable_index->mutex);
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < index->count && count < capacity; ++position) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (contains_text(index->entries[position].relative_path,
                          query,
                          case_sensitive)) {
            out_entries[count++] = index->entries[position];
        }
    }
    (void)umi_mutex_unlock(mutable_index->mutex);
    *out_count = count;
    return UMI_STATUS_OK;
}

/* Provide the file index stats operation used by this module and its client applications. */
UmiFileIndexStats umi_file_index_stats(const UmiFileIndex *index)
{
    UmiFileIndexStats stats;
    UmiFileIndex *mutable_index;
    (void)memset(&stats, 0, sizeof(stats));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index == NULL) return stats;
    mutable_index = (UmiFileIndex *)index;
    (void)umi_mutex_lock(mutable_index->mutex);
    stats = index->stats;
    (void)umi_mutex_unlock(mutable_index->mutex);
    return stats;
}
