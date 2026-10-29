/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/watcher.h
 *
 * PURPOSE:
 *   Define portable recursive file-watching events, lifecycle and statistics.
 *   The first backend is deterministic polling; a fake backend supports tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * List the named watch event kind values accepted by this public contract.
 */
typedef enum UmiWatchEventKind {
    UMI_WATCH_CREATED = 1,
    UMI_WATCH_MODIFIED = 2,
    UMI_WATCH_DELETED = 3,
    UMI_WATCH_RENAMED = 4,
    UMI_WATCH_OVERFLOW = 5,
    UMI_WATCH_RESCAN_REQUIRED = 6
} UmiWatchEventKind;

/**
 * List the named watcher backend values accepted by this public contract.
 */
typedef enum UmiWatcherBackend {
    UMI_WATCHER_POLLING = 1,
    UMI_WATCHER_FAKE = 2
} UmiWatcherBackend;

/**
 * Represent the watch event data shared with callers of this public contract.
 */
typedef struct UmiWatchEvent {
    uint64_t sequence;
    UmiWatchEventKind kind;
    char path[UMI_PATH_CAPACITY];
    char previous_path[UMI_PATH_CAPACITY];
    int directory;
} UmiWatchEvent;

typedef void (*UmiWatchEventSink)(const UmiWatchEvent *event,
                                  void *user_data);

/**
 * Represent the watcher config data shared with callers of this public contract.
 */
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

/**
 * Represent the watcher stats data shared with callers of this public contract.
 */
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

/**
 * Represent the watcher data shared with callers of this public contract.
 */
typedef struct UmiWatcher UmiWatcher;

/* Lifecycle ownership: serialize start, stop, set-root, explicit scans and
 * release from one controller. Do not scan concurrently with the background
 * polling worker. Exclude all external users before release. The sink and its
 * user_data are borrowed through callback completion; callbacks must return
 * normally. A callback cannot start, stop, release, change the root or recursively
 * scan its own watcher (including one active earlier in a nested sink chain).
 * These calls return INVALID_STATE and leave ownership intact.
 *
 * stop is blocking. A join/handle-release error preserves the watcher for
 * retry, including successful join progress. running remains set until the
 * controller finishes stop. destroy retains its void compatibility interface
 * and does not free a watcher after failed stop; use checked release to observe
 * that failure. A request to stop is not evidence that callbacks have finished. */

/** Request cooperative cancellation without joining or releasing anything.
 * Safe from an event callback while its borrowed watcher remains alive.
 * The lifecycle controller must later stop/join and release the watcher.
 * Do not race this borrowed access with release or a new start. */
UmiStatus UmiWatcherRequestStop(UmiWatcher *watcher);

/** Stop, join and release the caller's watcher, clearing its pointer on success.
 * Failure retains the pointer and any uncompleted lifecycle obligations.
 * NULL pointer storage is invalid; an already-empty owner is an OK no-op.
 * No forced thread termination or implicit ownership of sink_user_data. */
UmiStatus UmiWatcherRelease(UmiWatcher **inOutWatcher);

/**
 * Provide the watcher config default operation used by this module and its client
 * applications.
 */
UmiWatcherConfig umi_watcher_config_default(const char *root);
/**
 * Initialise watcher from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_watcher_create(const UmiWatcherConfig *config,
                             UmiWatcher **out_watcher);
/**
 * Release or reset state held by watcher so the same storage can be reused safely.
 */
void umi_watcher_destroy(UmiWatcher *watcher);
/**
 * Provide the watcher set root operation used by this module and its client applications.
 */
UmiStatus umi_watcher_set_root(UmiWatcher *watcher,
                               const char *root);
/**
 * Provide the watcher start operation used by this module and its client applications.
 */
UmiStatus umi_watcher_start(UmiWatcher *watcher);
/**
 * Provide the watcher stop operation used by this module and its client applications.
 */
UmiStatus umi_watcher_stop(UmiWatcher *watcher);
/**
 * Provide the watcher scan once operation used by this module and its client applications.
 */
UmiStatus umi_watcher_scan_once(UmiWatcher *watcher);
/**
 * Provide the watcher fake emit operation used by this module and its client applications.
 */
UmiStatus umi_watcher_fake_emit(UmiWatcher *watcher,
                                UmiWatchEventKind kind,
                                const char *path,
                                const char *previous_path,
                                int directory);
/**
 * Provide the watcher stats operation used by this module and its client applications.
 */
UmiWatcherStats umi_watcher_stats(const UmiWatcher *watcher);

#ifdef __cplusplus
}
#endif

#endif
