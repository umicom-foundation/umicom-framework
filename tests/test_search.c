/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_search.c
 *
 * PURPOSE:
 *   Verify indexed text search, line and column locations, previews and
 *   cancellation without depending on an external search executable.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/search.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/platform/filesystem.h"
#include "umicom/platform/threading.h"

typedef struct SearchState {
    size_t count;
    UmiSearchMatch first;
} SearchState;

static UmiStatus match_sink(const UmiSearchMatch *match, void *user_data)
{
    SearchState *state = (SearchState *)user_data;
    if (state->count == 0U) state->first = *match;
    state->count += 1U;
    return UMI_STATUS_OK;
}

int main(void)
{
    char temporary[UMI_PATH_CAPACITY];
    char root[UMI_PATH_CAPACITY];
    char leaf[128];
    char file[UMI_PATH_CAPACITY];
    UmiFileIndexConfig config;
    UmiFileIndex *index = NULL;
    UmiSearchRequest request;
    UmiSearchStats stats;
    SearchState state;
    UmiCancellationToken *token = NULL;

    assert(umi_fs_temp_directory(temporary, sizeof(temporary)) == UMI_STATUS_OK);
    (void)snprintf(leaf,
                   sizeof(leaf),
                   "umicom-search-%llu",
                   (unsigned long long)umi_thread_current_id());
    assert(umi_path_join(temporary, leaf, root, sizeof(root)) == UMI_STATUS_OK);
    assert(umi_fs_remove_tree(root) == UMI_STATUS_OK);
    assert(umi_fs_make_directories(root) == UMI_STATUS_OK);
    assert(umi_path_join(root, "source.c", file, sizeof(file)) == UMI_STATUS_OK);
    assert(umi_fs_write_text(file,
                             "first line\nUmicom Framework search\nlast line\n") ==
           UMI_STATUS_OK);
    config = umi_file_index_config_default(root);
    assert(umi_file_index_create(&config, &index) == UMI_STATUS_OK);
    assert(umi_file_index_rebuild(index) == UMI_STATUS_OK);
    (void)memset(&state, 0, sizeof(state));
    request = umi_search_request_default("framework");
    assert(umi_search_file_index(index,
                                 &request,
                                 match_sink,
                                 &state,
                                 &stats) == UMI_STATUS_OK);
    assert(state.count == 1U);
    assert(state.first.line == 2U);
    assert(state.first.column == 8U);
    assert(strstr(state.first.preview, "Umicom Framework") != NULL);
    assert(stats.files_searched == 1U);

    assert(umi_cancellation_token_create(&token) == UMI_STATUS_OK);
    umi_cancellation_token_request(token);
    request.cancellation = token;
    assert(umi_search_file_index(index,
                                 &request,
                                 match_sink,
                                 &state,
                                 &stats) == UMI_STATUS_CANCELLED);
    umi_cancellation_token_destroy(token);
    umi_file_index_destroy(index);
    assert(umi_fs_remove_tree(root) == UMI_STATUS_OK);
    return 0;
}
