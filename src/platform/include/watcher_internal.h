/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/include/watcher_internal.h
 *
 * PURPOSE:
 *   Share private watcher state between the public lifecycle implementation,
 *   polling backend and deterministic fake backend.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_WATCHER_INTERNAL_H
#define UMICOM_PLATFORM_WATCHER_INTERNAL_H

#include "umicom/platform/cancellation.h"
#include "umicom/platform/directory.h"
#include "umicom/platform/threading.h"
#include "umicom/platform/watcher.h"

typedef struct UmiWatcherEntry {
    char path[UMI_PATH_CAPACITY];
    uint64_t size;
    uint64_t modified_nanoseconds;
    int directory;
} UmiWatcherEntry;

struct UmiWatcher {
    UmiWatcherConfig config;
    char root[UMI_PATH_CAPACITY];
    UmiWatcherEntry *entries;
    size_t entry_count;
    size_t entry_capacity;
    UmiThread *thread;
    UmiMutex *mutex;
    UmiCancellationToken *cancellation;
    UmiWatcherStats stats;
    uint64_t next_sequence;
};

UmiStatus umi_watcher_polling_scan(UmiWatcher *watcher);
UmiStatus umi_watcher_emit(UmiWatcher *watcher,
                           UmiWatchEventKind kind,
                           const char *path,
                           const char *previous_path,
                           int directory);

#endif
