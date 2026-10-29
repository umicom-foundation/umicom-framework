/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/watcher_lifecycle.inc
 *
 * PURPOSE:
 *   Retain watcher ownership until native join and handle release both succeed.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Controller operations are serialised by the owner. The mutex protects the
 * observed running flag; it must never be held while joining a callback which
 * can itself request statistics or emit an event. */
UmiStatus umi_watcher_start(UmiWatcher *watcher)
{
    UmiStatus status;
    if (watcher == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (WatcherHasActiveCallback(watcher)) return UMI_STATUS_INVALID_STATE;
    if (watcher->mutex == NULL || watcher->cancellation == NULL)
        return UMI_STATUS_INVALID_STATE;
    status = umi_mutex_lock(watcher->mutex);
    if (status != UMI_STATUS_OK) return status;
    if (watcher->stats.running || watcher->thread != NULL) {
        (void)umi_mutex_unlock(watcher->mutex);
        /* Preserve the fake backend's established idempotent start. */
        return watcher->config.backend == UMI_WATCHER_FAKE
            ? UMI_STATUS_OK : UMI_STATUS_ALREADY_EXISTS;
    }
    umi_cancellation_token_reset(watcher->cancellation);
    watcher->thread_joined = 0;
    watcher->stats.running = 1;
    if (watcher->config.backend == UMI_WATCHER_FAKE) {
        return umi_mutex_unlock(watcher->mutex);
    }
    /* Publish the thread handle before releasing the lifecycle state lock.
     * A worker event callback is refused by the callback guard before it can
     * attempt a reentrant start/stop/release operation. */
    status = umi_thread_start(watcher_thread_entry, watcher, &watcher->thread);
    if (status != UMI_STATUS_OK) watcher->stats.running = 0;
    UmiStatus unlocked = umi_mutex_unlock(watcher->mutex);
    return status != UMI_STATUS_OK ? status : unlocked;
}

UmiStatus UmiWatcherRequestStop(UmiWatcher *watcher)
{
    if (watcher == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (watcher->cancellation == NULL) return UMI_STATUS_INVALID_STATE;
    umi_cancellation_token_request(watcher->cancellation);
    return UMI_STATUS_OK;
}

UmiStatus umi_watcher_stop(UmiWatcher *watcher)
{
    UmiStatus status;
    if (watcher == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Includes synchronous fake/polling callbacks and nested callback chains,
     * not merely the background thread whose handle has already been stored. */
    if (WatcherHasActiveCallback(watcher)) return UMI_STATUS_INVALID_STATE;
    if (watcher->cancellation != NULL)
        umi_cancellation_token_request(watcher->cancellation);
    if (watcher->thread != NULL) {
        if (!watcher->thread_joined) {
            status = umi_thread_join(watcher->thread, NULL);
            /* Native exit without an integer result still consumed the join.
             * Report it once and retain the owner; retry can close the handle. */
            if (status == UMI_STATUS_OK || status == UMI_STATUS_UNAVAILABLE)
                watcher->thread_joined = 1;
            if (status != UMI_STATUS_OK) return status;
        }
        status = UmiThreadRelease(&watcher->thread);
        if (status != UMI_STATUS_OK) return status;
    }
    /* Construction failure can leave an unstarted watcher without a mutex.
     * There is no published worker in that state, so partial release is safe. */
    if (watcher->mutex != NULL) {
        status = umi_mutex_lock(watcher->mutex);
        if (status != UMI_STATUS_OK) return status;
        watcher->stats.running = 0;
        return umi_mutex_unlock(watcher->mutex);
    }
    watcher->stats.running = 0;
    return UMI_STATUS_OK;
}

UmiStatus UmiWatcherRelease(UmiWatcher **inOutWatcher)
{
    UmiWatcher *watcher;
    UmiStatus status;
    if (inOutWatcher == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    watcher = *inOutWatcher;
    if (watcher == NULL) return UMI_STATUS_OK;
    status = umi_watcher_stop(watcher);
    if (status != UMI_STATUS_OK) return status;
    free(watcher->entries);
    umi_cancellation_token_destroy(watcher->cancellation);
    umi_mutex_destroy(watcher->mutex);
    free(watcher);
    *inOutWatcher = NULL;
    return UMI_STATUS_OK;
}

void umi_watcher_destroy(UmiWatcher *watcher)
{
    /* Compatibility callers keep the void interface. Failure now retains the
     * watcher rather than freeing data still used by a worker. New lifecycle
     * controllers use the checked release and keep the owner for retry. */
    (void)UmiWatcherRelease(&watcher);
}
