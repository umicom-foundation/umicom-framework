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
    size_t allocated;
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

/* Grow only the private storage actually needed by the index. The configured
 * maximum remains a hard limit, not a request to allocate every possible row. */
static UmiStatus ReserveEntries(UmiFileIndexEntry **entries, size_t *allocated,
                                size_t required, size_t maximum)
{
    size_t capacity = *allocated;
    UmiFileIndexEntry *candidate;
    if (required > maximum || maximum > SIZE_MAX / sizeof(**entries))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (required <= capacity) return UMI_STATUS_OK;
    if (capacity == 0U) capacity = maximum < 128U ? maximum : 128U;
    while (capacity < required) {
        capacity = capacity > maximum / 2U ? maximum : capacity * 2U;
    }
    candidate = realloc(*entries, capacity * sizeof(**entries));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *entries = candidate;
    *allocated = capacity;
    return UMI_STATUS_OK;
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
    /* An empty index needs no row allocation. Reject impossible bounds now,
     * before any watcher or refresh can use them in allocation arithmetic. */
    if (effective.maximum_files > SIZE_MAX / sizeof(*index->entries)) {
        umi_file_index_destroy(index);
        return UMI_STATUS_CAPACITY_EXCEEDED;
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
    char root[UMI_PATH_CAPACITY];
    UmiFileIndexEntry *entries;
    size_t maximum;
    size_t count;
    size_t allocated;
} BuildContext;

/* Construct a row before publishing it; a failed relative-path conversion must
 * not put an incomplete row into a snapshot. Names remain display previews. */
static UmiStatus BuildEntry(const char *root, const UmiFileInfo *info,
                            UmiFileIndexEntry *entry)
{
    UmiStatus status;
    (void)memset(entry, 0, sizeof(*entry));
    status = umi_path_copy(entry->path, sizeof(entry->path), info->path);
    if (status == UMI_STATUS_OK)
        status = umi_path_relative(root, info->path, entry->relative_path,
                                    sizeof(entry->relative_path));
    if (status != UMI_STATUS_OK) return status;
    copy_bounded(entry->name, sizeof(entry->name), info->name);
    (void)umi_path_extension(info->path, entry->extension, sizeof(entry->extension));
    entry->size = info->size;
    entry->modified_nanoseconds = info->modified_nanoseconds;
    return UMI_STATUS_OK;
}

/* Provide the build visitor operation used by this module and its client applications. */
static UmiStatus build_visitor(const UmiFileInfo *info, void *user_data)
{
    BuildContext *context = user_data;
    UmiStatus status;
    if (info->kind != UMI_FILE_KIND_REGULAR) return UMI_STATUS_OK;
    if (context->count >= context->maximum) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = ReserveEntries(&context->entries, &context->allocated,
                             context->count + 1U, context->maximum);
    if (status == UMI_STATUS_OK) {
        status = BuildEntry(context->root, info, &context->entries[context->count]);
        if (status == UMI_STATUS_OK) ++context->count;
    }
    return status;
}

/*
 * Provide the file index rebuild operation used by this module and its client
 * applications.
 */
UmiStatus umi_file_index_rebuild(UmiFileIndex *index)
{
    UmiDirectoryWalkOptions options;
    BuildContext context = {0};
    UmiStatus status;
    uint64_t revision;
    if (index == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(index->mutex);
    (void)memcpy(context.root, index->root, sizeof(context.root));
    context.maximum = index->config.maximum_files;
    revision = index->stats.revision;
    options = umi_directory_walk_options_default();
    options.max_depth = index->config.maximum_depth;
    options.include_hidden = index->config.include_hidden;
    options.include_files = 1;
    options.include_directories = 0;
    (void)umi_mutex_unlock(index->mutex);

    /* Walking a large tree does not hold the reader mutex or overwrite the
     * published rows. Failed scans leave the last usable snapshot untouched. */
    status = umi_directory_walk(context.root, &options, build_visitor, &context);
    if (status == UMI_STATUS_OK && context.count > 1U)
        qsort(context.entries, context.count, sizeof(*context.entries), compare_entries);
    if (status == UMI_STATUS_OK) {
        (void)umi_mutex_lock(index->mutex);
        /* A watcher or a workspace switch wins over an older in-flight scan. */
        if (index->stats.revision != revision) {
            status = UMI_STATUS_BUSY;
        } else {
            const int changed = index->count != context.count ||
                (context.count != 0U && memcmp(index->entries, context.entries,
                    context.count * sizeof(*context.entries)) != 0);
            if (changed) {
                UmiFileIndexEntry *oldEntries = index->entries;
                index->entries = context.entries;
                index->allocated = context.allocated;
                index->count = context.count;
                index->stats.files = context.count;
                ++index->stats.revision;
                context.entries = oldEntries;
            }
            ++index->stats.rebuilds;
        }
        (void)umi_mutex_unlock(index->mutex);
    }
    free(context.entries);
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
    /* Root, relative-path calculation and publication share one lock. A
     * concurrent root change cannot admit a row belonging to the old project. */
    (void)umi_mutex_lock(index->mutex);
    if (info.kind != UMI_FILE_KIND_REGULAR ||
        !umi_path_is_within(index->root, info.path)) {
        (void)umi_mutex_unlock(index->mutex);
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = BuildEntry(index->root, &info, &entry);
    if (status != UMI_STATUS_OK) {
        (void)umi_mutex_unlock(index->mutex);
        return status;
    }
    position = find_path(index, info.path);
    /* Apply this branch only when its contract condition is satisfied. */
    if (position == SIZE_MAX) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index->count >= index->config.maximum_files) {
            (void)umi_mutex_unlock(index->mutex);
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        status = ReserveEntries(&index->entries, &index->allocated,
                                 index->count + 1U, index->config.maximum_files);
        if (status != UMI_STATUS_OK) {
            (void)umi_mutex_unlock(index->mutex);
            return status;
        }
        position = index->count++;
    } else if (memcmp(&index->entries[position], &entry, sizeof(entry)) == 0) {
        /* Duplicate watcher notifications should not steal native row focus. */
        ++index->stats.updates;
        (void)umi_mutex_unlock(index->mutex);
        return UMI_STATUS_OK;
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

/* Copy one coherent filtered page. An expected revision makes paging across
 * asynchronous refreshes explicit; zero requests whichever snapshot is current. */
UmiStatus UmiFileIndexReadPage(const UmiFileIndex *index, const char *query,
    int caseSensitive, size_t offset, uint64_t expectedRevision,
    UmiFileIndexEntry *entries, size_t capacity, UmiFileIndexPage *page)
{
    UmiFileIndex *mutableIndex = (UmiFileIndex *)index;
    UmiFileIndexPage result = {0};
    size_t position;
    if (index == NULL || query == NULL || page == NULL ||
        (capacity != 0U && entries == NULL)) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(mutableIndex->mutex);
    if (expectedRevision != 0U && expectedRevision != index->stats.revision) {
        (void)umi_mutex_unlock(mutableIndex->mutex);
        return UMI_STATUS_BUSY;
    }
    result.stats = index->stats;
    result.offset = offset;
    for (position = 0U; position < index->count; ++position) {
        const UmiFileIndexEntry *entry = &index->entries[position];
        if (contains_text(entry->relative_path, query, caseSensitive)) {
            if (result.matched >= offset && result.count < capacity)
                entries[result.count++] = *entry;
            ++result.matched;
        }
    }
    (void)umi_mutex_unlock(mutableIndex->mutex);
    /* Subtract after checking offset; an arbitrarily large offset never wraps. */
    result.has_more = offset < result.matched &&
        result.count < result.matched - offset;
    *page = result;
    return UMI_STATUS_OK;
}
