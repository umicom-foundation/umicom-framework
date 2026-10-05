/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform/search_reader_fixture.h
 * PURPOSE: Keep saved-file search fixtures isolated and retain failed evidence without replacing existing files.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_SEARCH_READER_FIXTURE_H
#define UMICOM_TEST_SEARCH_READER_FIXTURE_H
#include "umicom/platform/filesystem.h"
#include "umicom/platform/output_file.h"
#include "umicom/platform/search_session.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <process.h>
#define SEARCH_PROCESS_ID _getpid
#else
#include <unistd.h>
#define SEARCH_PROCESS_ID getpid
#endif
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "line %d: %s\n", __LINE__, #x);                                                  \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
typedef struct SearchFixture
{
    UmiFileIndex *index;
    UmiFileSearchSession *session;
    UmiCancellationToken *cancel;
    char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
} SearchFixture;
static int SearchFixtureOpen(SearchFixture *f, const char *kind, const char *mode)
{
    char temp[UMI_PATH_CAPACITY], leaf[200];
    CHECK(umi_fs_temp_directory(temp, sizeof(temp)) == UMI_STATUS_OK);
    int written = snprintf(leaf, sizeof(leaf), "umicom-%s-%ld-%s", kind, (long)SEARCH_PROCESS_ID(), mode);
    CHECK(written > 0 && (size_t)written < sizeof(leaf));
    CHECK(umi_path_join(temp, leaf, f->root, sizeof(f->root)) == UMI_STATUS_OK);
    CHECK(!umi_fs_exists(f->root));
    CHECK(umi_fs_make_directories(f->root) == UMI_STATUS_OK);
    CHECK(umi_cancellation_token_create(&f->cancel) == UMI_STATUS_OK);
    return 0;
}
static int SearchFixturePut(SearchFixture *f, const char *leaf, const void *bytes, size_t length)
{
    UmiOutputFile *file = NULL;
    CHECK(umi_path_join(f->root, leaf, f->path, sizeof(f->path)) == UMI_STATUS_OK);
    CHECK(UmiOutputFileCreate(f->path, &file) == UMI_STATUS_OK);
    UmiStatus status = UmiOutputFileWrite(file, bytes, length);
    UmiStatus closed = UmiOutputFileClose(file);
    UmiOutputFileDestroy(file);
    CHECK(status == UMI_STATUS_OK && closed == UMI_STATUS_OK);
    return 0;
}
static int SearchFixtureIndex(SearchFixture *f)
{
    UmiFileIndexConfig config = umi_file_index_config_default(f->root);
    CHECK(umi_file_index_create(&config, &f->index) == UMI_STATUS_OK);
    CHECK(umi_file_index_rebuild(f->index) == UMI_STATUS_OK);
    return 0;
}
static void SearchFixtureStop(SearchFixture *f)
{
    UmiFileSearchDestroy(f->session);
    umi_file_index_destroy(f->index);
    umi_cancellation_token_destroy(f->cancel);
    /* Only this process created the directory after checking it did not exist. */
    (void)umi_fs_remove_tree(f->root);
}
#endif
