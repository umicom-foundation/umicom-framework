/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_watcher_lifecycle.c
 *
 * PURPOSE:
 *   Check callback ownership, nested sinks and restart after a joined stop.
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
#include <stdatomic.h>
#include <stdio.h>
#include <string.h>

typedef struct LifecycleState {
    UmiWatcher *owner;
    UmiWatcher *ancestor;
    UmiWatcher *nested;
    atomic_uint calls;
} LifecycleState;

static void RefuseActiveRelease(UmiWatcher *watcher)
{
    UmiWatcher *borrowed = watcher;
    assert(umi_watcher_stop(watcher) == UMI_STATUS_INVALID_STATE);
    assert(UmiWatcherRelease(&borrowed) == UMI_STATUS_INVALID_STATE);
    assert(borrowed == watcher);
    assert(umi_watcher_start(watcher) == UMI_STATUS_INVALID_STATE);
    assert(umi_watcher_set_root(watcher, ".") == UMI_STATUS_INVALID_STATE);
    assert(umi_watcher_scan_once(watcher) == UMI_STATUS_INVALID_STATE);
    /* The compatibility wrapper also preserves an active callback owner. */
    umi_watcher_destroy(watcher);
}

static void LifecycleSink(const UmiWatchEvent *event, void *user_data)
{
    LifecycleState *state = user_data;
    assert(event != NULL);
    RefuseActiveRelease(state->owner);
    if (state->ancestor != NULL) RefuseActiveRelease(state->ancestor);
    if (state->nested != NULL) {
        assert(umi_watcher_fake_emit(state->nested, UMI_WATCH_CREATED,
            "nested.c", NULL, 0) == UMI_STATUS_OK);
    }
    assert(UmiWatcherRequestStop(state->owner) == UMI_STATUS_OK);
    atomic_fetch_add(&state->calls, 1U);
}

static void FakeLifecycle(void)
{
    LifecycleState outer = {0}, inner = {0};
    atomic_init(&outer.calls, 0U);
    atomic_init(&inner.calls, 0U);
    UmiWatcherConfig config = umi_watcher_config_default(".");
    config.backend = UMI_WATCHER_FAKE;
    config.sink = LifecycleSink;
    config.sink_user_data = &outer;
    assert(umi_watcher_create(&config, &outer.owner) == UMI_STATUS_OK);
    config.sink_user_data = &inner;
    assert(umi_watcher_create(&config, &inner.owner) == UMI_STATUS_OK);
    outer.nested = inner.owner;
    inner.ancestor = outer.owner;
    assert(umi_watcher_start(outer.owner) == UMI_STATUS_OK);
    assert(umi_watcher_start(outer.owner) == UMI_STATUS_OK);
    assert(umi_watcher_fake_emit(outer.owner, UMI_WATCH_CREATED,
        "outer.c", NULL, 0) == UMI_STATUS_OK);
    assert(atomic_load(&outer.calls) == 1U && atomic_load(&inner.calls) == 1U);
    assert(umi_watcher_stats(outer.owner).running);
    assert(umi_watcher_stop(outer.owner) == UMI_STATUS_OK);
    assert(!umi_watcher_stats(outer.owner).running);
    assert(UmiWatcherRelease(&inner.owner) == UMI_STATUS_OK && inner.owner == NULL);
    assert(UmiWatcherRelease(&outer.owner) == UMI_STATUS_OK && outer.owner == NULL);
    assert(UmiWatcherRelease(&outer.owner) == UMI_STATUS_OK);
}

static void WaitForCalls(LifecycleState *state, unsigned expected)
{
    for (unsigned attempt = 0U; attempt < 5000U; ++attempt) {
        if (atomic_load(&state->calls) >= expected) return;
        umi_thread_sleep_ms(1U);
    }
    assert(!"Watcher callback did not arrive within the fixture budget");
}

static void PollingLifecycle(void)
{
    char temporary[UMI_PATH_CAPACITY], root[UMI_PATH_CAPACITY];
    char file[UMI_PATH_CAPACITY], leaf[128];
    UmiClock clock = umi_clock_system();
    assert(umi_fs_temp_directory(temporary, sizeof(temporary)) == UMI_STATUS_OK);
    (void)snprintf(leaf, sizeof(leaf), "umicom-watcher-lifecycle-%llu-%llu",
        (unsigned long long)umi_thread_current_id(),
        (unsigned long long)clock.monotonic_nanoseconds(&clock));
    assert(umi_path_join(temporary, leaf, root, sizeof(root)) == UMI_STATUS_OK);
    assert(!umi_fs_is_directory(root));
    assert(umi_fs_make_directories(root) == UMI_STATUS_OK);
    assert(umi_path_join(root, "entry.txt", file, sizeof(file)) == UMI_STATUS_OK);
    LifecycleState state = {0};
    atomic_init(&state.calls, 0U);
    UmiWatcherConfig config = umi_watcher_config_default(root);
    config.polling_interval_ms = 1U;
    config.sink = LifecycleSink;
    config.sink_user_data = &state;
    assert(umi_watcher_create(&config, &state.owner) == UMI_STATUS_OK);
    assert(umi_watcher_scan_once(state.owner) == UMI_STATUS_OK);
    assert(umi_fs_write_text(file, "owned fixture") == UMI_STATUS_OK);
    assert(umi_watcher_start(state.owner) == UMI_STATUS_OK);
    WaitForCalls(&state, 1U);
    assert(umi_watcher_stop(state.owner) == UMI_STATUS_OK);
    assert(!umi_watcher_stats(state.owner).running);
    assert(remove(file) == 0);
    assert(umi_watcher_start(state.owner) == UMI_STATUS_OK);
    WaitForCalls(&state, 2U);
    assert(umi_watcher_stop(state.owner) == UMI_STATUS_OK);
    assert(UmiWatcherRelease(&state.owner) == UMI_STATUS_OK && state.owner == NULL);
    assert(umi_fs_remove_tree(root) == UMI_STATUS_OK);
}

int main(void)
{
    assert(UmiWatcherRelease(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    assert(UmiWatcherRequestStop(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_watcher_stop(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    FakeLifecycle();
    PollingLifecycle();
    puts("Watcher lifecycle ownership checks completed.");
    return 0;
}
