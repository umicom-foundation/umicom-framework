/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_watcher.c
 *
 * PURPOSE:
 *   Verify deterministic fake events and polling detection for file creation,
 *   modification and deletion.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/watcher.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/platform/filesystem.h"
#include "umicom/platform/threading.h"

typedef struct WatchState {
    size_t created;
    size_t modified;
    size_t deleted;
    uint64_t last_sequence;
} WatchState;

static void watch_sink(const UmiWatchEvent *event, void *user_data)
{
    WatchState *state = (WatchState *)user_data;
    assert(event->sequence > state->last_sequence);
    state->last_sequence = event->sequence;
    if (event->kind == UMI_WATCH_CREATED) state->created += 1U;
    else if (event->kind == UMI_WATCH_MODIFIED) state->modified += 1U;
    else if (event->kind == UMI_WATCH_DELETED) state->deleted += 1U;
}

int main(void)
{
    char temporary[UMI_PATH_CAPACITY];
    char root[UMI_PATH_CAPACITY];
    char leaf[128];
    char file[UMI_PATH_CAPACITY];
    UmiWatcherConfig config;
    UmiWatcher *watcher = NULL;
    WatchState state;

    (void)memset(&state, 0, sizeof(state));
    config = umi_watcher_config_default(".");
    config.backend = UMI_WATCHER_FAKE;
    config.sink = watch_sink;
    config.sink_user_data = &state;
    assert(umi_watcher_create(&config, &watcher) == UMI_STATUS_OK);
    assert(umi_watcher_fake_emit(watcher,
                                 UMI_WATCH_CREATED,
                                 "one.c",
                                 NULL,
                                 0) == UMI_STATUS_OK);
    assert(state.created == 1U);
    umi_watcher_destroy(watcher);

    assert(umi_fs_temp_directory(temporary, sizeof(temporary)) == UMI_STATUS_OK);
    (void)snprintf(leaf,
                   sizeof(leaf),
                   "umicom-watcher-%llu",
                   (unsigned long long)umi_thread_current_id());
    assert(umi_path_join(temporary, leaf, root, sizeof(root)) == UMI_STATUS_OK);
    assert(umi_fs_remove_tree(root) == UMI_STATUS_OK);
    assert(umi_fs_make_directories(root) == UMI_STATUS_OK);
    assert(umi_path_join(root, "watched.txt", file, sizeof(file)) == UMI_STATUS_OK);

    (void)memset(&state, 0, sizeof(state));
    config = umi_watcher_config_default(root);
    config.sink = watch_sink;
    config.sink_user_data = &state;
    assert(umi_watcher_create(&config, &watcher) == UMI_STATUS_OK);
    assert(umi_watcher_set_root(watcher, root) == UMI_STATUS_OK);
    assert(umi_watcher_scan_once(watcher) == UMI_STATUS_OK);
    assert(umi_fs_write_text(file, "first") == UMI_STATUS_OK);
    assert(umi_watcher_scan_once(watcher) == UMI_STATUS_OK);
    assert(state.created == 1U);
    umi_thread_sleep_ms(5U);
    assert(umi_fs_write_text(file, "second-value") == UMI_STATUS_OK);
    assert(umi_watcher_scan_once(watcher) == UMI_STATUS_OK);
    assert(state.modified >= 1U);
    assert(remove(file) == 0);
    assert(umi_watcher_scan_once(watcher) == UMI_STATUS_OK);
    assert(state.deleted == 1U);
    umi_watcher_destroy(watcher);
    assert(umi_fs_remove_tree(root) == UMI_STATUS_OK);
    return 0;
}
