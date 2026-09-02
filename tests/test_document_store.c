/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_document_store.c
 *
 * PURPOSE:
 *   Verify toolkit-neutral document ownership, edits, snapshots, atomic save,
 *   external-change state, reopen and dirty-close protection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    char temp_root[UMI_PATH_CAPACITY];
    char test_root[UMI_PATH_CAPACITY];
    char path[UMI_PATH_CAPACITY];
    UmiDocumentStore *store = NULL;
    UmiDocumentId document_id = 0U;
    UmiDocumentId reopened_id = 0U;
    UmiDocumentSnapshot snapshot;
    char *text = NULL;
    size_t length = 0U;

    assert(umi_fs_temp_directory(temp_root, sizeof(temp_root)) == UMI_STATUS_OK);
    assert(umi_fs_join(test_root,
                       sizeof(test_root),
                       temp_root,
                       "umicom-document-store-test") == UMI_STATUS_OK);
    assert(umi_fs_remove_tree(test_root) == UMI_STATUS_OK);
    assert(umi_fs_make_directories(test_root) == UMI_STATUS_OK);
    assert(umi_fs_join(path,
                       sizeof(path),
                       test_root,
                       "example.c") == UMI_STATUS_OK);

    assert(umi_document_store_create(&store) == UMI_STATUS_OK);
    assert(umi_document_store_new(store,
                                  "untitled.c",
                                  &document_id) == UMI_STATUS_OK);
    assert(umi_document_store_replace_text(store,
                                           document_id,
                                           "abc",
                                           3U) == UMI_STATUS_OK);
    assert(umi_document_store_insert(store,
                                     document_id,
                                     1U,
                                     "123",
                                     3U) == UMI_STATUS_OK);
    assert(umi_document_store_erase(store,
                                    document_id,
                                    2U,
                                    2U) == UMI_STATUS_OK);
    assert(umi_document_store_copy_text(store,
                                        document_id,
                                        &text,
                                        &length) == UMI_STATUS_OK);
    assert(length == 4U);
    assert(strcmp(text, "a1bc") == 0);
    umi_document_store_free_text(text);
    text = NULL;

    assert(umi_document_store_save_as(store,
                                      document_id,
                                      path) == UMI_STATUS_OK);
    assert(umi_document_store_snapshot(store,
                                       document_id,
                                       &snapshot) == UMI_STATUS_OK);
    assert(!snapshot.dirty);
    assert(snapshot.has_path);

    assert(umi_document_store_mark_external_change(store,
                                                   document_id,
                                                   1) == UMI_STATUS_OK);
    assert(umi_document_store_snapshot(store,
                                       document_id,
                                       &snapshot) == UMI_STATUS_OK);
    assert(snapshot.external_change);

    assert(umi_document_store_close(store,
                                    document_id,
                                    0) == UMI_STATUS_OK);
    assert(umi_document_store_open(store,
                                   path,
                                   &reopened_id) == UMI_STATUS_OK);
    assert(umi_document_store_copy_text(store,
                                        reopened_id,
                                        &text,
                                        &length) == UMI_STATUS_OK);
    assert(strcmp(text, "a1bc") == 0);
    umi_document_store_free_text(text);

    assert(umi_document_store_replace_text(store,
                                           reopened_id,
                                           "changed",
                                           7U) == UMI_STATUS_OK);
    assert(umi_document_store_close(store,
                                    reopened_id,
                                    0) == UMI_STATUS_INVALID_STATE);
    assert(umi_document_store_close(store,
                                    reopened_id,
                                    1) == UMI_STATUS_OK);

    umi_document_store_destroy(store);
    assert(umi_fs_remove_tree(test_root) == UMI_STATUS_OK);
    (void)printf("document store contract passed\n");
    return 0;
}
