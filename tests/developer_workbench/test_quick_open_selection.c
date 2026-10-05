/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_workbench/test_quick_open_selection.c
 * PURPOSE: Exercise complete file identities, index changes and rejected malformed selections.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer_workbench/quick_open_selection.h"
#include <glib.h>
#include <glib/gstdio.h>
#include <stdio.h>
#include <string.h>
#define CHECK(c)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(c))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "line %d: %s\n", __LINE__, #c);                                                  \
            failed = 1;                                                                                      \
            goto cleanup;                                                                                    \
        }                                                                                                    \
    } while (0)
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *name = argv[1];
    const char *cases[] = {"valid",         "offset",           "stale-revision", "changed-root",
                           "changed-query", "changed-path",     "changed-name",   "changed-size",
                           "changed-time",  "invalid-position", "invalid-page",   "unterminated",
                           "zero-revision", "null-input"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(name, cases[i]) == 0)
            known = 1;
    if (!known)
        return 2;
    int failed = 0;
    UmiFileIndex *index = NULL;
    gchar *root = g_dir_make_tmp("umicom-file-choice-XXXXXX", NULL);
    gchar *first = NULL, *second = NULL, *other = NULL;
    UmiFileIndexEntry *entries = g_new0(UmiFileIndexEntry, 2);
    CHECK(root != NULL);
    first = g_build_filename(root, "alpha.c", NULL);
    second = g_build_filename(root, "beta.h", NULL);
    other = g_build_filename(root, "other", NULL);
    CHECK(g_file_set_contents(first, "alpha", 5, NULL));
    CHECK(g_file_set_contents(second, "beta", 4, NULL));
    CHECK(g_mkdir(other, 0700) == 0);
    UmiFileIndexConfig config = umi_file_index_config_default(root);
    CHECK(umi_file_index_create(&config, &index) == UMI_STATUS_OK);
    CHECK(umi_file_index_rebuild(index) == UMI_STATUS_OK);
    UmiFileIndexPage page;
    size_t offset = strcmp(name, "offset") == 0 ? 1U : 0U;
    CHECK(UmiFileIndexReadPage(index, "", 0, offset, 0U, entries, 2U, &page) == UMI_STATUS_OK);
    CHECK(page.count == 2U - offset);
    UmiStatus expected = UMI_STATUS_OK;
    const char *query = "";
    size_t position = 0U;
    if (strcmp(name, "stale-revision") == 0)
    {
        CHECK(g_file_set_contents(first, "changed source", 14, NULL));
        CHECK(umi_file_index_update(index, first) == UMI_STATUS_OK);
        expected = UMI_STATUS_BUSY;
    }
    if (strcmp(name, "changed-root") == 0)
    {
        CHECK(umi_file_index_set_root(index, other) == UMI_STATUS_OK);
        expected = UMI_STATUS_BUSY;
    }
    if (strcmp(name, "changed-query") == 0)
    {
        query = "alpha";
        expected = UMI_STATUS_BUSY;
    }
    if (strcmp(name, "changed-path") == 0)
    {
        entries[0].path[0] = 'Z';
        expected = UMI_STATUS_BUSY;
    }
    if (strcmp(name, "changed-name") == 0)
    {
        entries[0].name[0] = 'Z';
        expected = UMI_STATUS_BUSY;
    }
    if (strcmp(name, "changed-size") == 0)
    {
        ++entries[0].size;
        expected = UMI_STATUS_BUSY;
    }
    if (strcmp(name, "changed-time") == 0)
    {
        ++entries[0].modified_nanoseconds;
        expected = UMI_STATUS_BUSY;
    }
    if (strcmp(name, "invalid-position") == 0)
    {
        position = page.count;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(name, "invalid-page") == 0)
    {
        page.offset = SIZE_MAX;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(name, "unterminated") == 0)
    {
        memset(entries[0].path, 'x', sizeof(entries[0].path));
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(name, "zero-revision") == 0)
    {
        page.stats.revision = 0U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(name, "null-input") == 0)
    {
        CHECK(UmiQuickOpenCheckSelection(NULL, query, &page, position, entries) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiQuickOpenCheckSelection(index, NULL, &page, position, entries) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiQuickOpenCheckSelection(index, query, NULL, position, entries) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiQuickOpenCheckSelection(index, query, &page, position, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    }
    CHECK(UmiQuickOpenCheckSelection(index, query, &page, position, entries) == expected);
cleanup:
    umi_file_index_destroy(index);
    g_free(entries);
    if (first != NULL)
        (void)g_remove(first);
    if (second != NULL)
        (void)g_remove(second);
    if (other != NULL)
        (void)g_rmdir(other);
    if (root != NULL)
        (void)g_rmdir(root);
    g_free(first);
    g_free(second);
    g_free(other);
    g_free(root);
    return failed;
}
