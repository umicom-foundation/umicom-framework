/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/project_files/search.c
 *
 * PURPOSE:
 *   Search saved Umicom Notes source files through the shared Framework index
 *   and background-search session, without modifying or executing project code.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/search_session.h"
#include "umicom/platform/filesystem.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    UmiFileIndex *index = NULL;
    UmiFileSearchSession *search = NULL;
    UmiFileSearchSnapshot state;
    char cwd[UMI_PATH_CAPACITY], root[UMI_PATH_CAPACITY];
    UmiStatus status;
    if (argc != 3) {
        fputs("Usage: umicom-notes-file-search <project-directory> <text>\n", stderr);
        return EXIT_FAILURE;
    }
    status = umi_fs_current_directory(cwd, sizeof cwd);
    if (status == UMI_STATUS_OK)
        status = umi_path_absolute(argv[1], cwd, root, sizeof root);
    if (status == UMI_STATUS_OK) {
        UmiFileIndexConfig config = umi_file_index_config_default(root);
        status = umi_file_index_create(&config, &index);
    }
    /* This console program can wait. A GUI uses the existing asynchronous
     * index refresh API and returns to its event loop instead. */
    if (status == UMI_STATUS_OK) status = umi_file_index_rebuild(index);
    if (status == UMI_STATUS_OK) status = UmiFileSearchCreate(index, &search);
    if (status == UMI_STATUS_OK) status = UmiFileSearchStart(search, argv[2], 0, 0U);
    if (status == UMI_STATUS_OK) status = UmiFileSearchWait(search, 0U);
    if (status == UMI_STATUS_OK) status = UmiFileSearchRead(search, &state);
    if (status == UMI_STATUS_OK && !state.ready) status = state.status;
    if (status == UMI_STATUS_OK) {
        printf("Matches: %zu\n", state.stats.matches);
        for (size_t i = 0U; i < state.stats.matches; ++i) {
            UmiSearchMatch match;
            status = UmiFileSearchMatchAt(search, state.requestId, i, &match);
            if (status != UMI_STATUS_OK) break;
            printf("%s:%zu:%zu: %s\n", match.path, match.line, match.column, match.preview);
        }
        if (state.stats.truncated) puts("Result limit reached. Use a more specific query.");
        printf("Skipped files: %zu binary, %zu oversized\n",
            state.stats.binary_files_skipped, state.stats.oversized_files_skipped);
    }
    if (status != UMI_STATUS_OK) fprintf(stderr, "Search: %s\n", umi_status_text(status));
    /* The session borrows index. Cancel/drain its worker before releasing index. */
    UmiFileSearchDestroy(search);
    umi_file_index_destroy(index);
    return status == UMI_STATUS_OK ? EXIT_SUCCESS : EXIT_FAILURE;
}
