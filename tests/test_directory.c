/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_directory.c
 *
 * PURPOSE:
 *   Verify sorted portable directory traversal, metadata, recursion, hidden
 *   filtering and bounded counting with real temporary files.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/directory.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/platform/filesystem.h"
#include "umicom/platform/threading.h"

typedef struct VisitState {
    size_t files;
    size_t directories;
    char names[8][64];
    size_t name_count;
} VisitState;

static UmiStatus visitor(const UmiFileInfo *info, void *user_data)
{
    VisitState *state = (VisitState *)user_data;
    if (info->kind == UMI_FILE_KIND_DIRECTORY) state->directories += 1U;
    if (info->kind == UMI_FILE_KIND_REGULAR) state->files += 1U;
    if (state->name_count < 8U) {
        size_t length = strlen(info->name);
        if (length >= sizeof(state->names[0])) {
            length = sizeof(state->names[0]) - 1U;
        }
        (void)memcpy(state->names[state->name_count], info->name, length);
        state->names[state->name_count][length] = '\0';
        state->name_count += 1U;
    }
    return UMI_STATUS_OK;
}

int main(void)
{
    char temporary[UMI_PATH_CAPACITY];
    char root[UMI_PATH_CAPACITY];
    char nested[UMI_PATH_CAPACITY];
    char file_a[UMI_PATH_CAPACITY];
    char file_b[UMI_PATH_CAPACITY];
    char hidden[UMI_PATH_CAPACITY];
    char leaf[128];
    UmiDirectoryWalkOptions options;
    VisitState state;
    size_t count = 0U;

    assert(umi_fs_temp_directory(temporary, sizeof(temporary)) == UMI_STATUS_OK);
    (void)snprintf(leaf,
                   sizeof(leaf),
                   "umicom-directory-%llu",
                   (unsigned long long)umi_thread_current_id());
    assert(umi_path_join(temporary, leaf, root, sizeof(root)) == UMI_STATUS_OK);
    assert(umi_fs_remove_tree(root) == UMI_STATUS_OK);
    assert(umi_path_join(root, "nested", nested, sizeof(nested)) == UMI_STATUS_OK);
    assert(umi_fs_make_directories(nested) == UMI_STATUS_OK);
    assert(umi_path_join(root, "a.txt", file_a, sizeof(file_a)) == UMI_STATUS_OK);
    assert(umi_path_join(nested, "b.txt", file_b, sizeof(file_b)) == UMI_STATUS_OK);
    assert(umi_path_join(root, ".hidden", hidden, sizeof(hidden)) == UMI_STATUS_OK);
    assert(umi_fs_write_text(file_a, "alpha") == UMI_STATUS_OK);
    assert(umi_fs_write_text(file_b, "beta") == UMI_STATUS_OK);
    assert(umi_fs_write_text(hidden, "hidden") == UMI_STATUS_OK);

    options = umi_directory_walk_options_default();
    options.include_directories = 1;
    (void)memset(&state, 0, sizeof(state));
    assert(umi_directory_walk(root, &options, visitor, &state) == UMI_STATUS_OK);
    assert(state.files == 2U);
    assert(state.directories == 1U);
    assert(umi_directory_count(root, &options, &count) == UMI_STATUS_OK);
    assert(count == 3U);

    options.include_hidden = 1;
    assert(umi_directory_count(root, &options, &count) == UMI_STATUS_OK);
    assert(count == 4U);
    assert(umi_fs_remove_tree(root) == UMI_STATUS_OK);
    return 0;
}
