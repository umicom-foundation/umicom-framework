/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/watcher_polling.c
 *
 * PURPOSE:
 *   Implement a portable polling watcher by comparing sorted directory
 *   snapshots.  This backend works in ordinary Windows and POSIX processes
 *   and provides a deterministic baseline for future native OS adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "include/watcher_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int compare_entries(const void *left, const void *right)
{
    const UmiWatcherEntry *a = (const UmiWatcherEntry *)left;
    const UmiWatcherEntry *b = (const UmiWatcherEntry *)right;
#ifdef _WIN32
    return _stricmp(a->path, b->path);
#else
    return strcmp(a->path, b->path);
#endif
}

typedef struct UmiCaptureContext {
    UmiWatcherEntry *entries;
    size_t count;
    size_t capacity;
    size_t maximum;
} UmiCaptureContext;

static UmiStatus capture_visitor(const UmiFileInfo *info, void *user_data)
{
    UmiCaptureContext *context = (UmiCaptureContext *)user_data;
    UmiWatcherEntry *resized;
    size_t capacity;
    if (context->count >= context->maximum) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (context->count == context->capacity) {
        capacity = context->capacity == 0U ? 256U : context->capacity * 2U;
        if (capacity > context->maximum) capacity = context->maximum;
        resized = (UmiWatcherEntry *)realloc(
            context->entries,
            capacity * sizeof(*resized));
        if (resized == NULL) return UMI_STATUS_OUT_OF_MEMORY;
        context->entries = resized;
        context->capacity = capacity;
    }
    (void)memset(&context->entries[context->count],
                 0,
                 sizeof(context->entries[context->count]));
    (void)snprintf(context->entries[context->count].path,
                   sizeof(context->entries[context->count].path),
                   "%s",
                   info->path);
    context->entries[context->count].size = info->size;
    context->entries[context->count].modified_nanoseconds =
        info->modified_nanoseconds;
    context->entries[context->count].directory =
        info->kind == UMI_FILE_KIND_DIRECTORY;
    context->count += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_watcher_polling_scan(UmiWatcher *watcher)
{
    UmiDirectoryWalkOptions options;
    UmiCaptureContext capture;
    size_t old_index = 0U;
    size_t new_index = 0U;
    UmiStatus status;

    (void)memset(&capture, 0, sizeof(capture));
    capture.maximum = watcher->config.maximum_entries;
    options = umi_directory_walk_options_default();
    options.recursive = watcher->config.recursive;
    options.include_files = 1;
    options.include_directories = 1;
    options.include_hidden = watcher->config.include_hidden;
    status = umi_directory_walk(watcher->root,
                                &options,
                                capture_visitor,
                                &capture);
    if (status == UMI_STATUS_CAPACITY_EXCEEDED) {
        (void)umi_watcher_emit(watcher,
                               UMI_WATCH_OVERFLOW,
                               watcher->root,
                               NULL,
                               1);
        free(capture.entries);
        return status;
    }
    if (status != UMI_STATUS_OK) {
        free(capture.entries);
        return status;
    }
    qsort(capture.entries,
          capture.count,
          sizeof(*capture.entries),
          compare_entries);

    while (old_index < watcher->entry_count || new_index < capture.count) {
        int comparison;
        if (old_index >= watcher->entry_count) comparison = 1;
        else if (new_index >= capture.count) comparison = -1;
        else comparison = compare_entries(&watcher->entries[old_index],
                                           &capture.entries[new_index]);
        if (comparison < 0) {
            (void)umi_watcher_emit(watcher,
                                   UMI_WATCH_DELETED,
                                   watcher->entries[old_index].path,
                                   NULL,
                                   watcher->entries[old_index].directory);
            ++old_index;
        } else if (comparison > 0) {
            (void)umi_watcher_emit(watcher,
                                   UMI_WATCH_CREATED,
                                   capture.entries[new_index].path,
                                   NULL,
                                   capture.entries[new_index].directory);
            ++new_index;
        } else {
            if (watcher->entries[old_index].size !=
                    capture.entries[new_index].size ||
                watcher->entries[old_index].modified_nanoseconds !=
                    capture.entries[new_index].modified_nanoseconds) {
                (void)umi_watcher_emit(watcher,
                                       UMI_WATCH_MODIFIED,
                                       capture.entries[new_index].path,
                                       NULL,
                                       capture.entries[new_index].directory);
            }
            ++old_index;
            ++new_index;
        }
    }

    free(watcher->entries);
    watcher->entries = capture.entries;
    watcher->entry_count = capture.count;
    watcher->entry_capacity = capture.capacity;
    (void)umi_mutex_lock(watcher->mutex);
    watcher->stats.scans += 1U;
    watcher->stats.tracked_entries = watcher->entry_count;
    (void)umi_mutex_unlock(watcher->mutex);
    return UMI_STATUS_OK;
}
