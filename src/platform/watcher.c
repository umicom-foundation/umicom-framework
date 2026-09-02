/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/watcher.c
 *
 * PURPOSE:
 *   Own watcher lifecycle, background execution, event sequencing and shared
 *   operational statistics independently from a particular backend.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/watcher.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/watcher_internal.h"
#include "umicom/platform/filesystem.h"

/*
 * Provide the watcher thread entry operation used by this module and its client
 * applications.
 */
static int watcher_thread_entry(void *user_data)
{
    UmiWatcher *watcher = (UmiWatcher *)user_data;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (!umi_cancellation_token_is_requested(watcher->cancellation)) {
        (void)umi_watcher_scan_once(watcher);
        umi_thread_sleep_ms(watcher->config.polling_interval_ms);
    }
    return 0;
}

/*
 * Provide the watcher config default operation used by this module and its client
 * applications.
 */
UmiWatcherConfig umi_watcher_config_default(const char *root)
{
    UmiWatcherConfig config;
    (void)memset(&config, 0, sizeof(config));
    config.root = root;
    config.backend = UMI_WATCHER_POLLING;
    config.recursive = 1;
    config.include_hidden = 0;
    config.polling_interval_ms = 500U;
    config.maximum_entries = 32768U;
    return config;
}

/*
 * Initialise watcher from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_watcher_create(const UmiWatcherConfig *config,
                             UmiWatcher **out_watcher)
{
    UmiWatcherConfig effective;
    UmiWatcher *watcher;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (config == NULL || config->root == NULL || out_watcher == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_watcher = NULL;
    effective = *config;
    /* Apply this branch only when its contract condition is satisfied. */
    if (effective.polling_interval_ms == 0U) {
        effective.polling_interval_ms = 500U;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (effective.maximum_entries == 0U) {
        effective.maximum_entries = 32768U;
    }
    watcher = (UmiWatcher *)calloc(1U, sizeof(*watcher));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (watcher == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    watcher->config = effective;
    status = umi_path_normalise(config->root,
                                watcher->root,
                                sizeof(watcher->root));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK ||
        umi_mutex_create(&watcher->mutex) != UMI_STATUS_OK ||
        umi_cancellation_token_create(&watcher->cancellation) != UMI_STATUS_OK) {
        umi_watcher_destroy(watcher);
        return status != UMI_STATUS_OK ? status : UMI_STATUS_OUT_OF_MEMORY;
    }
    watcher->config.root = watcher->root;
    watcher->stats.backend = effective.backend;
    watcher->next_sequence = 1U;
    *out_watcher = watcher;
    return UMI_STATUS_OK;
}

/* Release or reset state held by watcher so the same storage can be reused safely. */
void umi_watcher_destroy(UmiWatcher *watcher)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (watcher == NULL) return;
    (void)umi_watcher_stop(watcher);
    free(watcher->entries);
    umi_cancellation_token_destroy(watcher->cancellation);
    umi_mutex_destroy(watcher->mutex);
    free(watcher);
}

/* Provide the watcher set root operation used by this module and its client applications. */
UmiStatus umi_watcher_set_root(UmiWatcher *watcher,
                               const char *root)
{
    char normalised[UMI_PATH_CAPACITY];
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (watcher == NULL || root == NULL || root[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_path_normalise(root, normalised, sizeof(normalised));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_fs_is_directory(normalised)) return UMI_STATUS_NOT_FOUND;

    (void)umi_mutex_lock(watcher->mutex);
    /* Apply this branch only when its contract condition is satisfied. */
    if (watcher->stats.running) {
        (void)umi_mutex_unlock(watcher->mutex);
        return UMI_STATUS_BUSY;
    }
    free(watcher->entries);
    watcher->entries = NULL;
    watcher->entry_count = 0U;
    watcher->entry_capacity = 0U;
    (void)snprintf(watcher->root, sizeof(watcher->root), "%s", normalised);
    watcher->config.root = watcher->root;
    watcher->stats.tracked_entries = 0U;
    watcher->stats.scans = 0U;
    watcher->stats.events = 0U;
    watcher->stats.created = 0U;
    watcher->stats.modified = 0U;
    watcher->stats.deleted = 0U;
    watcher->stats.overflow = 0U;
    watcher->next_sequence = 1U;
    (void)umi_mutex_unlock(watcher->mutex);
    return UMI_STATUS_OK;
}

/* Provide the watcher start operation used by this module and its client applications. */
UmiStatus umi_watcher_start(UmiWatcher *watcher)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (watcher == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (watcher->config.backend == UMI_WATCHER_FAKE) {
        watcher->stats.running = 1;
        return UMI_STATUS_OK;
    }
    (void)umi_mutex_lock(watcher->mutex);
    /* Apply this branch only when its contract condition is satisfied. */
    if (watcher->stats.running) {
        (void)umi_mutex_unlock(watcher->mutex);
        return UMI_STATUS_ALREADY_EXISTS;
    }
    umi_cancellation_token_reset(watcher->cancellation);
    watcher->stats.running = 1;
    (void)umi_mutex_unlock(watcher->mutex);
    status = umi_thread_start(watcher_thread_entry,
                              watcher,
                              &watcher->thread);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        watcher->stats.running = 0;
    }
    return status;
}

/* Provide the watcher stop operation used by this module and its client applications. */
UmiStatus umi_watcher_stop(UmiWatcher *watcher)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (watcher == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!watcher->stats.running) return UMI_STATUS_OK;
    umi_cancellation_token_request(watcher->cancellation);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (watcher->thread != NULL) {
        (void)umi_thread_join(watcher->thread, NULL);
        umi_thread_destroy(watcher->thread);
        watcher->thread = NULL;
    }
    watcher->stats.running = 0;
    return UMI_STATUS_OK;
}

/* Provide the watcher scan once operation used by this module and its client applications. */
UmiStatus umi_watcher_scan_once(UmiWatcher *watcher)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (watcher == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (watcher->config.backend == UMI_WATCHER_FAKE) {
        return UMI_STATUS_OK;
    }
    return umi_watcher_polling_scan(watcher);
}

/* Provide the watcher stats operation used by this module and its client applications. */
UmiWatcherStats umi_watcher_stats(const UmiWatcher *watcher)
{
    UmiWatcherStats stats;
    UmiWatcher *mutable_watcher;
    (void)memset(&stats, 0, sizeof(stats));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (watcher == NULL) return stats;
    mutable_watcher = (UmiWatcher *)watcher;
    (void)umi_mutex_lock(mutable_watcher->mutex);
    stats = watcher->stats;
    (void)umi_mutex_unlock(mutable_watcher->mutex);
    return stats;
}

/* Provide the watcher emit operation used by this module and its client applications. */
UmiStatus umi_watcher_emit(UmiWatcher *watcher,
                           UmiWatchEventKind kind,
                           const char *path,
                           const char *previous_path,
                           int directory)
{
    UmiWatchEvent event;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (watcher == NULL || path == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(&event, 0, sizeof(event));
    event.sequence = watcher->next_sequence++;
    event.kind = kind;
    event.directory = directory;
    (void)snprintf(event.path, sizeof(event.path), "%s", path);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (previous_path != NULL) {
        (void)snprintf(event.previous_path,
                       sizeof(event.previous_path),
                       "%s",
                       previous_path);
    }
    (void)umi_mutex_lock(watcher->mutex);
    watcher->stats.events += 1U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (kind == UMI_WATCH_CREATED) watcher->stats.created += 1U;
    else /* Apply this branch only when its contract condition is satisfied. */ if (kind == UMI_WATCH_MODIFIED) watcher->stats.modified += 1U;
    else /* Apply this branch only when its contract condition is satisfied. */ if (kind == UMI_WATCH_DELETED) watcher->stats.deleted += 1U;
    else /* Apply this branch only when its contract condition is satisfied. */ if (kind == UMI_WATCH_OVERFLOW) watcher->stats.overflow += 1U;
    (void)umi_mutex_unlock(watcher->mutex);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (watcher->config.sink != NULL) {
        watcher->config.sink(&event, watcher->config.sink_user_data);
    }
    return UMI_STATUS_OK;
}
