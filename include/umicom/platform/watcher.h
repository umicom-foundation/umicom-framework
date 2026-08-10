/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/watcher.h
 *
 * PURPOSE:
 *   Define portable recursive file-watching events, lifecycle and statistics.
 *   The first backend is deterministic polling; a fake backend supports tests.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_WATCHER_H
#define UMICOM_PLATFORM_WATCHER_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/platform/path.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiWatchEventKind {
    UMI_WATCH_CREATED = 1,
    UMI_WATCH_MODIFIED = 2,
    UMI_WATCH_DELETED = 3,
    UMI_WATCH_RENAMED = 4,
    UMI_WATCH_OVERFLOW = 5,
    UMI_WATCH_RESCAN_REQUIRED = 6
} UmiWatchEventKind;

typedef enum UmiWatcherBackend {
    UMI_WATCHER_POLLING = 1,
    UMI_WATCHER_FAKE = 2
} UmiWatcherBackend;

typedef struct UmiWatchEvent {
    uint64_t sequence;
    UmiWatchEventKind kind;
    char path[UMI_PATH_CAPACITY];
    char previous_path[UMI_PATH_CAPACITY];
    int directory;
} UmiWatchEvent;

typedef void (*UmiWatchEventSink)(const UmiWatchEvent *event,
                                  void *user_data);

typedef struct UmiWatcherConfig {
    const char *root;
    UmiWatcherBackend backend;
    int recursive;
    int include_hidden;
    uint32_t polling_interval_ms;
    size_t maximum_entries;
    UmiWatchEventSink sink;
    void *sink_user_data;
} UmiWatcherConfig;

typedef struct UmiWatcherStats {
    UmiWatcherBackend backend;
    int running;
    size_t tracked_entries;
    uint64_t scans;
    uint64_t events;
    uint64_t created;
    uint64_t modified;
    uint64_t deleted;
    uint64_t overflow;
} UmiWatcherStats;

typedef struct UmiWatcher UmiWatcher;

UmiWatcherConfig umi_watcher_config_default(const char *root);
UmiStatus umi_watcher_create(const UmiWatcherConfig *config,
                             UmiWatcher **out_watcher);
void umi_watcher_destroy(UmiWatcher *watcher);
UmiStatus umi_watcher_set_root(UmiWatcher *watcher,
                               const char *root);
UmiStatus umi_watcher_start(UmiWatcher *watcher);
UmiStatus umi_watcher_stop(UmiWatcher *watcher);
UmiStatus umi_watcher_scan_once(UmiWatcher *watcher);
UmiStatus umi_watcher_fake_emit(UmiWatcher *watcher,
                                UmiWatchEventKind kind,
                                const char *path,
                                const char *previous_path,
                                int directory);
UmiWatcherStats umi_watcher_stats(const UmiWatcher *watcher);

#ifdef __cplusplus
}
#endif

#endif
