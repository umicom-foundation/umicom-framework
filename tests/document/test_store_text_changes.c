/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_store_text_changes.c
 * PURPOSE: Check all-or-nothing stored replacements, snapshot identity and explicit revision changes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/document_store_edits.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(c))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #c);                                          \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
static int Expect(UmiDocumentStore *store, UmiDocumentId id, const char *expected)
{
    char *text = NULL;
    size_t bytes = 0U;
    CHECK(umi_document_store_copy_text(store, id, &text, &bytes) == UMI_STATUS_OK);
    CHECK(bytes == strlen(expected) && strcmp(text, expected) == 0);
    umi_document_store_free_text(text);
    return 0;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *known[] = {"apply",     "empty",     "unchanged",    "forced",     "mixed",
                           "ownership", "duplicate", "stale-first",  "stale-last", "closed",
                           "saved",     "external",  "path",         "name",       "length",
                           "dirty",     "has-path",  "unterminated", "null-text",  "embedded-zero",
                           "too-large", "too-many",  "empty-set",    "arguments"};
    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    UmiDocumentStore *store = NULL;
    CHECK(umi_document_store_create(&store) == UMI_STATUS_OK);
    UmiDocumentId ids[2];
    UmiDocumentSnapshot expected[2];
    for (size_t i = 0U; i < 2U; ++i)
    {
        CHECK(umi_document_store_new(store, i == 0U ? "first.c" : "second.c", &ids[i]) == UMI_STATUS_OK);
        CHECK(umi_document_store_replace_text(store, ids[i], i == 0U ? "first" : "second",
                                              i == 0U ? 5U : 6U) == UMI_STATUS_OK);
        CHECK(umi_document_store_snapshot(store, ids[i], &expected[i]) == UMI_STATUS_OK);
    }
    char first[] = "changed first", second[] = "changed second";
    UmiDocumentStoreTextChange changes[2] = {{&expected[0], first, 13U, 0}, {&expected[1], second, 14U, 0}};
    const char *after_first = first, *after_second = second;
    UmiStatus wanted = UMI_STATUS_OK;
    size_t count = 2U;
    if (strcmp(mode, "empty") == 0)
    {
        changes[1].text = "";
        changes[1].length = 0U;
        after_second = "";
    }
    if (strcmp(mode, "unchanged") == 0 || strcmp(mode, "forced") == 0 || strcmp(mode, "mixed") == 0)
    {
        changes[0].text = "first";
        changes[0].length = 5U;
        after_first = "first";
        changes[0].force_revision = strcmp(mode, "forced") == 0;
        if (strcmp(mode, "mixed") != 0)
        {
            changes[1].text = "second";
            changes[1].length = 6U;
            after_second = "second";
        }
    }
    if (strcmp(mode, "duplicate") == 0)
    {
        changes[1].expected = &expected[0];
        wanted = UMI_STATUS_ALREADY_EXISTS;
    }
    if (strcmp(mode, "stale-first") == 0 || strcmp(mode, "stale-last") == 0)
    {
        size_t index = strcmp(mode, "stale-first") == 0 ? 0U : 1U;
        CHECK(umi_document_store_replace_text(store, ids[index], "later", 5U) == UMI_STATUS_OK);
        wanted = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "closed") == 0)
    {
        CHECK(umi_document_store_close(store, ids[1], 1) == UMI_STATUS_OK);
        wanted = UMI_STATUS_NOT_FOUND;
    }
    if (strcmp(mode, "saved") == 0)
    {
        ++expected[1].saved_revision;
        wanted = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "external") == 0)
    {
        CHECK(umi_document_store_mark_external_change(store, ids[1], 1) == UMI_STATUS_OK);
        wanted = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "path") == 0)
    {
        strcpy(expected[1].path, "different.c");
        wanted = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "name") == 0)
    {
        strcpy(expected[1].display_name, "different.c");
        wanted = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "length") == 0)
    {
        ++expected[1].length;
        wanted = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "dirty") == 0)
    {
        expected[1].dirty = 0;
        wanted = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "has-path") == 0)
    {
        expected[1].has_path = 1;
        wanted = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "unterminated") == 0)
    {
        memset(expected[1].path, 'x', sizeof(expected[1].path));
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "null-text") == 0)
    {
        changes[1].text = NULL;
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "embedded-zero") == 0)
    {
        changes[1].text = "a\0b";
        changes[1].length = 3U;
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "too-large") == 0)
    {
        changes[1].length = UMI_DOCUMENT_STORE_EDIT_MAXIMUM_BYTES + 1U;
        wanted = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (strcmp(mode, "too-many") == 0)
    {
        count = UMI_DOCUMENT_STORE_MAX + 1U;
        wanted = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (strcmp(mode, "empty-set") == 0)
    {
        count = 0U;
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    UmiDocumentSnapshot before[2];
    CHECK(umi_document_store_snapshot(store, ids[0], &before[0]) == UMI_STATUS_OK);
    if (strcmp(mode, "closed") != 0)
        CHECK(umi_document_store_snapshot(store, ids[1], &before[1]) == UMI_STATUS_OK);
    CHECK(UmiDocumentStoreReplaceTextChanges(store, changes, count) == wanted);
    if (wanted == UMI_STATUS_OK)
    {
        CHECK(Expect(store, ids[0], after_first) == 0 && Expect(store, ids[1], after_second) == 0);
        for (size_t i = 0U; i < 2U; ++i)
        {
            UmiDocumentSnapshot current;
            CHECK(umi_document_store_snapshot(store, ids[i], &current) == UMI_STATUS_OK);
            int changed = (i == 0U ? strcmp(after_first, "first") : strcmp(after_second, "second")) != 0 ||
                          changes[i].force_revision;
            CHECK(current.revision == before[i].revision + (changed ? 1U : 0U) &&
                  current.saved_revision == before[i].saved_revision);
        }
        if (strcmp(mode, "ownership") == 0)
        {
            memset(first, 'x', 13U);
            memset(second, 'y', 14U);
            CHECK(Expect(store, ids[0], "changed first") == 0 &&
                  Expect(store, ids[1], "changed second") == 0);
        }
    }
    else
    {
        CHECK(Expect(store, ids[0], strcmp(mode, "stale-first") == 0 ? "later" : "first") == 0);
        if (strcmp(mode, "closed") != 0)
            CHECK(Expect(store, ids[1], strcmp(mode, "stale-last") == 0 ? "later" : "second") == 0);
        UmiDocumentSnapshot current;
        CHECK(umi_document_store_snapshot(store, ids[0], &current) == UMI_STATUS_OK &&
              current.revision == before[0].revision);
    }
    if (strcmp(mode, "arguments") == 0)
    {
        CHECK(UmiDocumentStoreReplaceTextChanges(NULL, changes, 2U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentStoreReplaceTextChanges(store, NULL, 2U) == UMI_STATUS_INVALID_ARGUMENT);
        changes[1].force_revision = 2;
        CHECK(UmiDocumentStoreReplaceTextChanges(store, changes, 2U) == UMI_STATUS_INVALID_ARGUMENT);
        changes[1].force_revision = 0;
        changes[1].expected = NULL;
        CHECK(UmiDocumentStoreReplaceTextChanges(store, changes, 2U) == UMI_STATUS_INVALID_ARGUMENT);
    }
    umi_document_store_destroy(store);
    return 0;
}
