/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/project_files/refresh.c
 * PURPOSE:
 *   Read a Notes project's file list on a Framework worker while the calling
 *   thread polls copied progress, without changing or executing project files.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/file_index.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/threading.h"
#include <stdio.h>
#include <stdlib.h>

/* The example is a console client of the same APIs used by Studio's Explorer.
 * A GUI uses its existing timer instead of sleeping inside an event callback. */
int main(int argc, char **argv)
{
    UmiFileIndex *index = NULL;
    UmiFileIndexRefreshSnapshot refresh = {0};
    UmiFileIndexEntry first;
    UmiFileIndexPage page;
    char current[UMI_PATH_CAPACITY], root[UMI_PATH_CAPACITY];
    UmiStatus status;
    if (argc != 2) {
        fputs("Usage: umicom-notes-file-refresh <project-directory>\n", stderr);
        return EXIT_FAILURE;
    }
    status = umi_fs_current_directory(current, sizeof(current));
    if (status == UMI_STATUS_OK)
        status = umi_path_absolute(argv[1], current, root, sizeof(root));
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Project path: %s\n", umi_status_text(status));
        return EXIT_FAILURE;
    }
    /* The index copies this configuration; root is absolute before submission. */
    UmiFileIndexConfig config = umi_file_index_config_default(root);
    status = umi_file_index_create(&config, &index);
    if (status == UMI_STATUS_OK) status = UmiFileIndexRefreshStart(index, 0U);
    if (status != UMI_STATUS_OK) goto finish;
    for (unsigned attempt = 0U; attempt < 3000U; ++attempt) {
        status = UmiFileIndexRefreshRead(index, &refresh);
        if (status != UMI_STATUS_OK || !refresh.active) break;
        /* A real application can process input or other work between polls. */
        umi_thread_sleep_ms(10U);
    }
    if (status == UMI_STATUS_OK) {
        if (refresh.active) {
            (void)UmiFileIndexRefreshCancel(index);
            status = UMI_STATUS_TIMEOUT;
        } else status = refresh.status;
    }
    if (status == UMI_STATUS_OK) {
        status = UmiFileIndexReadPage(index, "", 0, 0U, 0U, &first, 1U, &page);
        if (status == UMI_STATUS_OK) {
            printf("Indexed files: %zu\n", page.stats.files);
            if (page.count != 0U) printf("First file: %s\n", first.relative_path);
        }
    }
finish:
    if (status != UMI_STATUS_OK) fprintf(stderr, "Refresh: %s\n", umi_status_text(status));
    /* Destruction requests cancellation and joins before releasing index data. */
    umi_file_index_destroy(index);
    return status == UMI_STATUS_OK ? EXIT_SUCCESS : EXIT_FAILURE;
}
