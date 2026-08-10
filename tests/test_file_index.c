/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_file_index.c
 *
 * PURPOSE:
 *   Verify workspace indexing, metadata queries, watcher-style updates and
 *   removals using real temporary files.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/file_index.h"

#include <assert.h>
#include <stdio.h>

#include "umicom/platform/filesystem.h"
#include "umicom/platform/threading.h"

int main(void)
{
    char temporary[UMI_PATH_CAPACITY];
    char root[UMI_PATH_CAPACITY];
    char leaf[128];
    char source[UMI_PATH_CAPACITY];
    char header[UMI_PATH_CAPACITY];
    char nested[UMI_PATH_CAPACITY];
    char nested_file[UMI_PATH_CAPACITY];
    UmiFileIndexConfig config;
    UmiFileIndex *index = NULL;
    UmiFileIndexEntry results[4];
    UmiFileIndexStats stats;
    size_t count = 0U;

    assert(umi_fs_temp_directory(temporary, sizeof(temporary)) == UMI_STATUS_OK);
    (void)snprintf(leaf,
                   sizeof(leaf),
                   "umicom-index-%llu",
                   (unsigned long long)umi_thread_current_id());
    assert(umi_path_join(temporary, leaf, root, sizeof(root)) == UMI_STATUS_OK);
    assert(umi_fs_remove_tree(root) == UMI_STATUS_OK);
    assert(umi_fs_make_directories(root) == UMI_STATUS_OK);
    assert(umi_path_join(root, "main.c", source, sizeof(source)) == UMI_STATUS_OK);
    assert(umi_path_join(root, "main.h", header, sizeof(header)) == UMI_STATUS_OK);
    assert(umi_path_join(root, "nested", nested, sizeof(nested)) == UMI_STATUS_OK);
    assert(umi_path_join(nested, "nested.c", nested_file, sizeof(nested_file)) == UMI_STATUS_OK);
    assert(umi_fs_write_text(source, "int main(void){return 0;}") == UMI_STATUS_OK);
    assert(umi_fs_write_text(header, "#pragma once") == UMI_STATUS_OK);
    assert(umi_fs_make_directories(nested) == UMI_STATUS_OK);
    assert(umi_fs_write_text(nested_file, "int nested;") == UMI_STATUS_OK);

    config = umi_file_index_config_default(root);
    assert(umi_file_index_create(&config, &index) == UMI_STATUS_OK);
    assert(umi_file_index_rebuild(index) == UMI_STATUS_OK);
    stats = umi_file_index_stats(index);
    assert(stats.files == 3U);
    assert(umi_file_index_find(index,
                               ".c",
                               0,
                               results,
                               4U,
                               &count) == UMI_STATUS_OK);
    assert(count == 2U);
    assert(remove(header) == 0);
    assert(umi_file_index_remove(index, header) == UMI_STATUS_OK);
    assert(umi_file_index_stats(index).files == 2U);
    assert(umi_fs_write_text(source, "int value = 1;") == UMI_STATUS_OK);
    assert(umi_file_index_update(index, source) == UMI_STATUS_OK);
    assert(umi_file_index_set_root(index, nested) == UMI_STATUS_OK);
    assert(umi_file_index_rebuild(index) == UMI_STATUS_OK);
    stats = umi_file_index_stats(index);
    assert(stats.files == 1U);
    umi_file_index_destroy(index);
    index = NULL;
    assert(umi_fs_remove_tree(root) == UMI_STATUS_OK);
    return 0;
}
