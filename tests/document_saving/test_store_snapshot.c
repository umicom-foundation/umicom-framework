/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document_saving/test_store_snapshot.c
 * PURPOSE: Check immutable save inputs and conditional store acknowledgement.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/document_store.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)

#ifdef UMI_TEST_ALLOCATION_FAILURE
static int failNextAllocation;
void *__real_malloc(size_t size);
void *__wrap_malloc(size_t size)
{
    if (failNextAllocation) { failNextAllocation = 0; return NULL; }
    return __real_malloc(size);
}
#endif

static int Same(const UmiDocumentSnapshot *left, const UmiDocumentSnapshot *right)
{
    return left->document_id == right->document_id && left->revision == right->revision &&
        left->saved_revision == right->saved_revision && left->length == right->length &&
        left->external_change == right->external_change && left->dirty == right->dirty &&
        left->has_path == right->has_path && strcmp(left->path, right->path) == 0 &&
        strcmp(left->display_name, right->display_name) == 0;
}

/* These paths are identifiers only. This fixture never writes a real file. */
static int Run(UmiDocumentStore *store, const char *name)
{
    UmiDocumentId id = 0U, other = 0U;
    UmiDocumentSnapshot captured, before, after;
    char *text = NULL;
    CHECK(umi_document_store_new(store, "untitled.txt", &id) == UMI_STATUS_OK);
    if (strcmp(name, "empty") != 0)
        CHECK(umi_document_store_replace_text(store, id, "saved input", 11U) == UMI_STATUS_OK);
    if (strcmp(name, "invalid") == 0 || strcmp(name, "allocation") == 0) {
        memset(&captured, 0x5a, sizeof(captured));
        unsigned char unchanged[sizeof(captured)];
        memcpy(unchanged, &captured, sizeof(captured));
#ifdef UMI_TEST_ALLOCATION_FAILURE
        if (strcmp(name, "allocation") == 0) {
            failNextAllocation = 1;
            CHECK(UmiDocumentStoreCopySnapshot(store, id, &captured, &text) == UMI_STATUS_OUT_OF_MEMORY);
            CHECK(!failNextAllocation && text == NULL && memcmp(unchanged, &captured, sizeof(captured)) == 0);
            CHECK(UmiDocumentStoreCopySnapshot(store, id, &captured, &text) == UMI_STATUS_OK);
            CHECK(strcmp(text, "saved input") == 0);
            umi_document_store_free_text(text);
            return 0;
        }
#endif
        CHECK(strcmp(name, "invalid") == 0);
        CHECK(UmiDocumentStoreCopySnapshot(NULL, id, &captured, &text) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentStoreCopySnapshot(store, 0U, &captured, &text) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentStoreCopySnapshot(store, id, NULL, &text) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentStoreCopySnapshot(store, id, &captured, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentStoreCopySnapshot(store, UINT64_MAX, &captured, &text) == UMI_STATUS_NOT_FOUND);
        CHECK(text == NULL && memcmp(unchanged, &captured, sizeof(captured)) == 0);
        char sentinel[] = "owned elsewhere";
        text = sentinel;
        CHECK(UmiDocumentStoreCopySnapshot(store, id, &captured, &text) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(text == sentinel && memcmp(unchanged, &captured, sizeof(captured)) == 0);
        text = NULL;
        CHECK(UmiDocumentStoreCopySnapshot(store, id, &captured, &text) == UMI_STATUS_OK);
        before = captured;
        CHECK(UmiDocumentStoreMarkSavedSnapshot(NULL, &captured, "notes.txt") == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentStoreMarkSavedSnapshot(store, NULL, "notes.txt") == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentStoreMarkSavedSnapshot(store, &captured, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentStoreMarkSavedSnapshot(store, &captured, "") == UMI_STATUS_INVALID_ARGUMENT);
        captured.saved_revision = captured.revision + 1U;
        CHECK(UmiDocumentStoreMarkSavedSnapshot(store, &captured, "notes.txt") == UMI_STATUS_INVALID_ARGUMENT);
        captured = before; memset(captured.path, 'x', sizeof(captured.path));
        CHECK(UmiDocumentStoreMarkSavedSnapshot(store, &captured, "notes.txt") == UMI_STATUS_INVALID_ARGUMENT);
        captured = before; captured.external_change = 2;
        CHECK(UmiDocumentStoreMarkSavedSnapshot(store, &captured, "notes.txt") == UMI_STATUS_INVALID_ARGUMENT);
        char tooLong[UMI_PATH_CAPACITY + 1U];
        memset(tooLong, 'x', sizeof(tooLong) - 1U); tooLong[sizeof(tooLong) - 1U] = '\0';
        CHECK(UmiDocumentStoreMarkSavedSnapshot(store, &before, tooLong) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(umi_document_store_snapshot(store, id, &after) == UMI_STATUS_OK && Same(&before, &after));
        umi_document_store_free_text(text);
        return 0;
    }
    CHECK(UmiDocumentStoreCopySnapshot(store, id, &captured, &text) == UMI_STATUS_OK);
    CHECK(captured.document_id == id && captured.length == strlen(text));
    if (strcmp(name, "success") == 0 || strcmp(name, "empty") == 0) {
        CHECK(strcmp(text, strcmp(name, "empty") == 0 ? "" : "saved input") == 0);
        CHECK(UmiDocumentStoreMarkSavedSnapshot(store, &captured, "./notes.txt") == UMI_STATUS_OK);
        CHECK(umi_document_store_snapshot(store, id, &after) == UMI_STATUS_OK);
        CHECK(!after.dirty && after.has_path && !after.external_change);
        CHECK(after.saved_revision == captured.revision && after.revision == captured.revision);
        CHECK(strcmp(after.display_name, "notes.txt") == 0 && umi_path_equal(after.path, "notes.txt"));
    } else if (strcmp(name, "closed") == 0) {
        CHECK(umi_document_store_close(store, id, 1) == UMI_STATUS_OK);
        CHECK(umi_document_store_new(store, "replacement.txt", &other) == UMI_STATUS_OK && other != id);
        CHECK(UmiDocumentStoreMarkSavedSnapshot(store, &captured, "notes.txt") == UMI_STATUS_NOT_FOUND);
        CHECK(umi_document_store_snapshot(store, other, &after) == UMI_STATUS_OK && !after.has_path);
        CHECK(strcmp(text, "saved input") == 0);
    } else {
        UmiStatus expected = UMI_STATUS_INVALID_STATE;
        if (strcmp(name, "edit") == 0) {
            CHECK(umi_document_store_replace_text(store, id, "newer edit", 10U) == UMI_STATUS_OK);
        } else if (strcmp(name, "restored-text") == 0) {
            CHECK(umi_document_store_replace_text(store, id, "changed", 7U) == UMI_STATUS_OK);
            CHECK(umi_document_store_replace_text(store, id, "saved input", 11U) == UMI_STATUS_OK);
        } else if (strcmp(name, "external") == 0) {
            CHECK(umi_document_store_mark_external_change(store, id, 1) == UMI_STATUS_OK);
        } else if (strcmp(name, "renamed") == 0 || strcmp(name, "already-saved") == 0) {
            CHECK(umi_document_store_mark_saved_as(store, id,
                strcmp(name, "renamed") == 0 ? "elsewhere.txt" : "notes.txt") == UMI_STATUS_OK);
        } else if (strcmp(name, "occupied") == 0) {
            CHECK(umi_document_store_create_loaded(store, "notes.txt", "notes.txt", "other", 5U, &other) == UMI_STATUS_OK);
            expected = UMI_STATUS_ALREADY_EXISTS;
        } else { umi_document_store_free_text(text); return 2; }
        CHECK(umi_document_store_snapshot(store, id, &before) == UMI_STATUS_OK);
        CHECK(UmiDocumentStoreMarkSavedSnapshot(store, &captured, "notes.txt") == expected);
        CHECK(umi_document_store_snapshot(store, id, &after) == UMI_STATUS_OK && Same(&before, &after));
        CHECK(strcmp(text, "saved input") == 0);
    }
    umi_document_store_free_text(text);
    return 0;
}

int main(int argc, char **argv)
{
    UmiDocumentStore *store = NULL;
    if (argc != 2) return 2;
    CHECK(umi_document_store_create(&store) == UMI_STATUS_OK);
    int result = Run(store, argv[1]);
    umi_document_store_destroy(store);
    return result;
}
