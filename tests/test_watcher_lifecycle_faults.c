/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_watcher_lifecycle_faults.c
 *
 * PURPOSE:
 *   Keep real watcher owners available after injected native lifecycle errors.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "support/assertions.h"
#include "umicom/platform/watcher.h"
#include "umicom/platform/threading.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/clock.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Linker wrapping affects only the selected failure boundary. The worker,
 * cancellation token, join and release implementations otherwise remain real. */
UmiStatus __real_umi_thread_join(UmiThread *thread, int *result);
UmiStatus __real_UmiThreadRelease(UmiThread **thread);
UmiStatus __wrap_umi_thread_join(UmiThread *thread, int *result);
UmiStatus __wrap_UmiThreadRelease(UmiThread **thread);
static int failJoin;
static int failRelease;
static int unavailableAfterJoin;
static unsigned joins;
UmiStatus __wrap_umi_thread_join(UmiThread *thread, int *result)
{
    ++joins;
    if (failJoin) {
        failJoin = 0;
        return UMI_STATUS_INTERNAL_ERROR;
    }
    UmiStatus status = __real_umi_thread_join(thread, result);
    /* A native join can complete while its callback result is unavailable.
     * Consume the real join first so retry would expose a double-join bug. */
    if (status == UMI_STATUS_OK && unavailableAfterJoin) {
        unavailableAfterJoin = 0;
        return UMI_STATUS_UNAVAILABLE;
    }
    return status;
}
UmiStatus __wrap_UmiThreadRelease(UmiThread **thread)
{
    if (failRelease) {
        failRelease = 0;
        return UMI_STATUS_INTERNAL_ERROR;
    }
    return __real_UmiThreadRelease(thread);
}

int main(int argc, char **argv)
{
    assert(argc == 2);
    const int joinFailure = strcmp(argv[1], "join") == 0;
    const int releaseFailure = strcmp(argv[1], "release") == 0;
    const int legacyFailure = strcmp(argv[1], "legacy") == 0;
    const int unavailableFailure = strcmp(argv[1], "joined-without-result") == 0;
    assert(joinFailure || releaseFailure || legacyFailure || unavailableFailure);
    char temporary[UMI_PATH_CAPACITY], root[UMI_PATH_CAPACITY], leaf[128];
    UmiClock clock = umi_clock_system();
    assert(umi_fs_temp_directory(temporary, sizeof(temporary)) == UMI_STATUS_OK);
    (void)snprintf(leaf, sizeof(leaf), "umicom-watcher-fault-%llu-%llu",
        (unsigned long long)umi_thread_current_id(),
        (unsigned long long)clock.monotonic_nanoseconds(&clock));
    assert(umi_path_join(temporary, leaf, root, sizeof(root)) == UMI_STATUS_OK);
    assert(!umi_fs_is_directory(root));
    assert(umi_fs_make_directories(root) == UMI_STATUS_OK);
    UmiWatcherConfig config = umi_watcher_config_default(root);
    config.polling_interval_ms = 1U;
    UmiWatcher *watcher = NULL;
    assert(umi_watcher_create(&config, &watcher) == UMI_STATUS_OK);
    assert(umi_watcher_start(watcher) == UMI_STATUS_OK);
    UmiWatcher *owned = watcher;
    failJoin = joinFailure || legacyFailure;
    failRelease = releaseFailure;
    unavailableAfterJoin = unavailableFailure;
    if (legacyFailure) umi_watcher_destroy(watcher);
    else assert(UmiWatcherRelease(&watcher) ==
        (unavailableFailure ? UMI_STATUS_UNAVAILABLE : UMI_STATUS_INTERNAL_ERROR));
    assert(watcher == owned);
    assert(umi_watcher_stats(watcher).running);
    assert(umi_watcher_start(watcher) == UMI_STATUS_ALREADY_EXISTS);
    assert(UmiWatcherRelease(&watcher) == UMI_STATUS_OK && watcher == NULL);
    /* A successful join must survive a subsequent handle-close failure. */
    assert(joins == (releaseFailure || unavailableFailure ? 1U : 2U));
    assert(umi_fs_remove_tree(root) == UMI_STATUS_OK);
    return 0;
}
