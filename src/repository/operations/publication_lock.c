/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/operations/publication_lock.c
 *
 * PURPOSE:
 *   Serialize overlapping automated publishers using exclusively created repository metadata markers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "publication_internal.h"
#include "umicom/platform/output_file.h"
#include <stdio.h>
#include <string.h>

/* The private metadata lock serialises overlapping publication invocations.
 * It is not index.lock and never causes an existing Git lock to be removed.
 * An interrupted process leaves its marker for explicit owner investigation. */
UmiStatus umi_publication_acquire_locks(UmiPublicationState *state)
{
    for (size_t i = 0U; i < state->report->count; ++i) {
        const char *args[] = {"rev-parse", "--absolute-git-dir"};
        char directory[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
        UmiOutputFile *file = NULL;
        if (state->report->entries[i].vendor) continue;
        UmiStatus status = umi_publication_text(state, i, args, 2U, directory, sizeof(directory));
        if (status != UMI_STATUS_OK) return status;
        status = umi_publication_safe_path(state, directory);
        if (status != UMI_STATUS_OK) return status;
        status = umi_path_join(directory, "umicom-publication.lock", path, sizeof(path));
        if (status != UMI_STATUS_OK) return status;
        int already_owned = 0;
        for (size_t k = 0U; k < state->lock_count; ++k)
            if (umi_path_equal(path, state->lock_paths[k])) already_owned = 1;
        if (already_owned) continue;
        if (state->lock_count == UMI_REPOSITORY_PUBLICATION_CAPACITY)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        status = UmiOutputFileCreate(path, &file);
        if (status != UMI_STATUS_OK) return umi_publication_fail(state, i, UMI_STATUS_BUSY,
            "Publication metadata lock exists or cannot be created. Check for another publisher before retrying.");
        /* Record ownership immediately, including partial-write failures, so
         * normal failure cleanup removes only markers this invocation created. */
        (void)snprintf(state->lock_paths[state->lock_count++], UMI_PATH_CAPACITY, "%s", path);
        const char *notice = "Umicom publication is active. Do not remove this marker while the publisher is running.\n";
        status = UmiOutputFileWrite(file, notice, strlen(notice));
        if (status == UMI_STATUS_OK) status = UmiOutputFileClose(file);
        UmiOutputFileDestroy(file);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_publication_release_locks(UmiPublicationState *state)
{
    UmiStatus first = UMI_STATUS_OK;
    /* Work backwards through only the exact files created above. No caller
     * locks, directories, source files or Git indexes are removed. */
    while (state->lock_count) {
        const char *path = state->lock_paths[--state->lock_count];
        UmiFileInfo info;
        UmiStatus status = umi_directory_stat(path, &info);
        if (status == UMI_STATUS_OK && info.kind != UMI_FILE_KIND_REGULAR)
            status = UMI_STATUS_INVALID_STATE;
        if (status == UMI_STATUS_OK) status = umi_fs_remove_tree(path);
        if (first == UMI_STATUS_OK && status != UMI_STATUS_OK) first = status;
    }
    return first;
}
