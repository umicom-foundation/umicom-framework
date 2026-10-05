/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/value_archive/main.c
 * PURPOSE: Save and recover a reviewed bookmark collection through the existing Data Server.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/bookmarks.h"
#include "umicom/data/blob_store.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* No direct file writes belong in a value codec. This lesson passes the
 * encoded bytes to the existing storage authority and restores only after
 * checking the destination's current observation revision. */
static UmiStatus Run(UmiDataServer *server)
{
    UmiBookmarkRegistry *source = NULL, *reopened = NULL;
    unsigned char *encoded = NULL, *saved = NULL;
    size_t size = 0U, saved_size = 0U;
    UmiStore store; UmiBlobStore blobs; UmiBookmarkSnapshot item = {0}, restored;
    UmiStatus status = umi_store_from_data_server(server, &store);
    if (status == UMI_STATUS_OK) status = umi_blob_store_init(&blobs, &store, "portable-state");
    if (status == UMI_STATUS_OK) status = umi_platform_bookmarks_registry_create(&source);
    if (status == UMI_STATUS_OK) status = umi_platform_bookmarks_registry_create(&reopened);
    if (status != UMI_STATUS_OK) goto done;
    memcpy(item.id, "notes", sizeof("notes"));
    memcpy(item.uri, "file:///workspace/notes.c", sizeof("file:///workspace/notes.c"));
    memcpy(item.label, "Notes project", sizeof("Notes project"));
    status = umi_platform_bookmarks_registry_upsert(source, &item);
    if (status != UMI_STATUS_OK) goto done;
    uint64_t observation = umi_platform_bookmarks_registry_revision(source);
    status = umi_platform_bookmarks_registry_archive_encode(source, observation, NULL, 0U, &size);
    if (status != UMI_STATUS_OK) goto done;
    encoded = malloc(size);
    if (encoded == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto done; }
    status = umi_platform_bookmarks_registry_archive_encode(source, observation, encoded, size, &size);
    if (status != UMI_STATUS_OK) goto done;
    /* Keep the complete saved value and commit within one Data Server
     * transaction. A failed write rolls back instead of clearing old data. */
    status = umi_data_server_begin(server);
    if (status != UMI_STATUS_OK) goto done;
    status = umi_blob_store_put(&blobs, "bookmarks", encoded, size);
    if (status == UMI_STATUS_OK) status = umi_data_server_commit(server);
    if (status != UMI_STATUS_OK) {
        UmiStatus recovery = umi_data_server_rollback(server);
        if (recovery != UMI_STATUS_OK)
            fprintf(stderr, "Storage rollback: %s\n", umi_status_text(recovery));
        goto done;
    }
    status = umi_blob_store_get(&blobs, "bookmarks", &saved, &saved_size);
    if (status != UMI_STATUS_OK) goto done;
    observation = umi_platform_bookmarks_registry_revision(reopened);
    status = umi_platform_bookmarks_registry_archive_restore(reopened, observation, saved, saved_size, NULL);
    if (status != UMI_STATUS_OK) goto done;
    status = umi_platform_bookmarks_registry_find(reopened, "notes", &restored);
    if (status == UMI_STATUS_OK && (strcmp(restored.uri, item.uri) != 0 || strcmp(restored.label, item.label) != 0))
        status = UMI_STATUS_INTERNAL_ERROR;
    if (status == UMI_STATUS_OK) printf("Recovered bookmark: %s (%s)\n", restored.label, restored.uri);
done:
    umi_blob_store_free(saved);
    free(encoded);
    umi_platform_bookmarks_registry_destroy(reopened);
    umi_platform_bookmarks_registry_destroy(source);
    return status;
}
int main(int argc, char **argv)
{
    if (argc != 1) { fprintf(stderr, "This lesson uses an isolated in-memory Data Server.\n"); return 2; }
    (void)argv;
    UmiDataServer *server = NULL;
    UmiStatus status = umi_data_server_create_memory(&server);
    if (status == UMI_STATUS_OK) status = Run(server);
    umi_data_server_destroy(server);
    if (status != UMI_STATUS_OK) fprintf(stderr, "Recovery lesson: %s\n", umi_status_text(status));
    return status == UMI_STATUS_OK ? 0 : 1;
}
