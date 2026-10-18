/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/value_archive/workspace.c
 * PURPOSE: Decode with reusable caller-owned scratch through an installed SDK.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/bookmarks.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* A host can keep this workspace in a pool owned by its import worker. It is
 * separate from the destination and is never exposed as a live bookmark. */
typedef struct BookmarkImportStorage {
    UmiBookmarkSnapshot sample;
    UmiBookmarkSnapshot scratch;
    UmiBookmarkSnapshot result;
} BookmarkImportStorage;

int main(void)
{
    int exit_status = 1;
    BookmarkImportStorage *storage = calloc(1U, sizeof(*storage));
    unsigned char *bytes = NULL;
    size_t size = 0U, written = 0U;
    UmiStatus status = UMI_STATUS_OUT_OF_MEMORY;
    if (storage == NULL) goto done;
    storage->sample.struct_size = (uint32_t)sizeof(storage->sample);
    storage->sample.api_version = 1U;
    memcpy(storage->sample.id, "notes", sizeof("notes"));
    memcpy(storage->sample.label, "Notes workspace", sizeof("Notes workspace"));
    memcpy(storage->sample.uri, "file:///workspace/notes.c", sizeof("file:///workspace/notes.c"));
    status = umi_platform_bookmarks_snapshot_archive_encode(&storage->sample, NULL, 0U, &size);
    if (status != UMI_STATUS_OK) goto done;
    bytes = malloc(size);
    if (bytes == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto done; }
    status = umi_platform_bookmarks_snapshot_archive_encode(&storage->sample, bytes, size, &written);
    if (status != UMI_STATUS_OK) goto done;
    if (written != size) { status = UMI_STATUS_INTERNAL_ERROR; goto done; }
    status = umi_platform_bookmarks_snapshot_archive_decode_with_workspace(
        bytes, size, &storage->scratch, &storage->result);
    if (status != UMI_STATUS_OK) goto done;

    /* Scratch is reusable immediately. Erasing it cannot erase the destination. */
    memset(&storage->scratch, 0xa5, sizeof(storage->scratch));
    if (strcmp(storage->result.id, "notes") != 0 ||
        strcmp(storage->result.label, "Notes workspace") != 0) {
        status = UMI_STATUS_INTERNAL_ERROR;
        goto done;
    }
    printf("Decoded independent bookmark: %s\n", storage->result.label);
    /* Damage is refused before replacing the previously accepted value. */
    bytes[size - 1U] ^= 1U;
    status = umi_platform_bookmarks_snapshot_archive_decode_with_workspace(
        bytes, size, &storage->scratch, &storage->result);
    if (status != UMI_STATUS_PARSE_ERROR ||
        strcmp(storage->result.label, "Notes workspace") != 0) {
        status = UMI_STATUS_INTERNAL_ERROR;
        goto done;
    }
    exit_status = 0;
done:
    if (exit_status != 0) fprintf(stderr, "Workspace import: %s\n", umi_status_text(status));
    free(bytes);
    free(storage);
    return exit_status;
}
