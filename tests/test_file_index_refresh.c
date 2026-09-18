/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_file_index_refresh.c
 * PURPOSE:
 *   Exercise cancelled and stale scans, repeated background requests and
 *   teardown using the real file index, task queue and owned directories.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/file_index.h"
#include "umicom/platform/directory.h"
#include "umicom/platform/filesystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef UMI_TEST_DIRECTORY_FAULTS
#include <dirent.h>
#include <errno.h>
static int enumerationFault;
static unsigned enumerationCalls;
struct dirent *__real_readdir(DIR *stream);
/* Link-time injection affects only this Linux regression executable. It models
 * a read error after enumeration has begun, not an end-of-directory result. */
struct dirent *__wrap_readdir(DIR *stream)
{
    if (enumerationFault && ++enumerationCalls == 4U) {
        errno = EIO;
        return NULL;
    }
    return __real_readdir(stream);
}
#endif
/* Keep every check enabled in Release builds. */
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); return EXIT_FAILURE; } } while (0)
typedef struct Progress {
    UmiCancellationToken *token;
    UmiFileIndex *index;
    const char *otherRoot;
    size_t threshold, calls, last;
    int action, failed;
} Progress;
/* An unlocked checkpoint lets the test reproduce a cancellation or root change
 * exactly before publication, without substituting a filesystem or worker. */
static void Observe(size_t count, void *data)
{
    Progress *progress = data;
    ++progress->calls;
    if (count < progress->last) progress->failed = 1;
    progress->last = count;
    if (count < progress->threshold || progress->action == 0) return;
    if (progress->action == 1) umi_cancellation_token_request(progress->token);
    else if (progress->action == 2) {
        if (umi_file_index_set_root(progress->index, progress->otherRoot) != UMI_STATUS_OK)
            progress->failed = 1;
    } else if (progress->action == 3) {
        if (umi_file_index_clear(progress->index) != UMI_STATUS_OK) progress->failed = 1;
    }
    progress->action = 0;
}
static int AddFiles(const char *root, unsigned first, unsigned count)
{
    char path[UMI_PATH_CAPACITY], name[64];
    for (unsigned n = first; n < first + count; ++n) {
        (void)snprintf(name,sizeof(name),"notes-%04u.c",n);
        CHECK(umi_path_join(root,name,path,sizeof(path)) == UMI_STATUS_OK);
        CHECK(umi_fs_write_text(path,"/* Umicom Notes source. */\n") == UMI_STATUS_OK);
    }
    return EXIT_SUCCESS;
}
typedef struct Visit { UmiCancellationToken *token; size_t count; } Visit;
static UmiStatus CancelVisitor(const UmiFileInfo *info, void *data)
{
    Visit *visit = data; (void)info;
    ++visit->count; umi_cancellation_token_request(visit->token);
    return UMI_STATUS_OK;
}
static int Run(const char *name, const char *root, const char *other)
{
    UmiFileIndex *index = NULL;
    UmiCancellationToken *token = NULL;
    UmiFileIndexConfig config = umi_file_index_config_default(root);
    UmiFileIndexScanOptions options = {0};
    UmiFileIndexRefreshSnapshot state;
    Progress progress = {0};
    UmiFileIndexStats before;
    UmiFileIndexEntry row;
    if (strcmp(name,"failure") == 0) config.maximum_files = 2U;
    CHECK(umi_file_index_create(&config,&index) == UMI_STATUS_OK);
    CHECK(umi_cancellation_token_create(&token) == UMI_STATUS_OK);
    CHECK(AddFiles(root,0U,2U) == EXIT_SUCCESS);
    CHECK(umi_file_index_rebuild(index) == UMI_STATUS_OK);
    before = umi_file_index_stats(index);
    progress.index = index; progress.token = token; progress.otherRoot = other;
    options.cancellation = token; options.progress = Observe; options.userData = &progress;
#ifdef UMI_TEST_DIRECTORY_FAULTS
    if (strcmp(name,"enumeration-error") == 0) {
        enumerationCalls = 0U; enumerationFault = 1;
        UmiStatus failure = umi_file_index_rebuild(index);
        enumerationFault = 0;
        CHECK(failure == UMI_STATUS_IO_ERROR);
        CHECK(umi_file_index_stats(index).revision == before.revision);
        CHECK(umi_file_index_stats(index).files == before.files);
    } else
#endif
    if (strcmp(name,"invalid") == 0) {
        CHECK(UmiFileIndexRefreshStart(NULL,0U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiFileIndexRefreshCancel(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiFileIndexRefreshWait(NULL,0U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiFileIndexRefreshRead(NULL,&state) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiFileIndexRefreshRead(index,NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiFileIndexRebuildWithOptions(NULL,&options) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDirectoryWalkCancellable(NULL,NULL,CancelVisitor,NULL,token) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDirectoryWalkCancellable(root,NULL,NULL,NULL,token) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiFileIndexRefreshRead(index,&state) == UMI_STATUS_OK && state.requestId == 0U && !state.active);
        CHECK(UmiFileIndexRefreshCancel(index) == UMI_STATUS_OK);
        CHECK(UmiFileIndexRefreshWait(index,1U) == UMI_STATUS_OK);
    } else if (strcmp(name,"pre-cancel") == 0) {
        umi_cancellation_token_request(token);
        CHECK(UmiFileIndexRebuildWithOptions(index,&options) == UMI_STATUS_CANCELLED);
        CHECK(progress.calls == 0U && umi_file_index_stats(index).revision == before.revision);
    } else if (strcmp(name,"mid-cancel") == 0 || strcmp(name,"late-cancel") == 0) {
        const int middle = strcmp(name,"mid-cancel") == 0;
        if (middle) CHECK(AddFiles(root,2U,128U) == EXIT_SUCCESS);
        progress.action = 1; progress.threshold = middle ? 64U : 2U;
        CHECK(UmiFileIndexRebuildWithOptions(index,&options) == UMI_STATUS_CANCELLED);
        CHECK(progress.last == progress.threshold && !progress.failed);
        CHECK(umi_file_index_stats(index).revision == before.revision);
        CHECK(umi_file_index_stats(index).files == 2U && umi_file_index_stats(index).rebuilds == before.rebuilds);
        CHECK(umi_file_index_at(index,1U,&row) == UMI_STATUS_OK && strcmp(row.name,"notes-0001.c") == 0);
    } else if (strcmp(name,"stale-request") == 0) {
        options.expectedRevision = before.revision + 1U;
        CHECK(UmiFileIndexRebuildWithOptions(index,&options) == UMI_STATUS_BUSY && progress.calls == 0U);
        CHECK(UmiFileIndexRefreshStart(index,before.revision + 1U) == UMI_STATUS_BUSY);
        CHECK(UmiFileIndexRefreshRead(index,&state) == UMI_STATUS_OK && state.requestId == 0U);
    } else if (strcmp(name,"root-during-scan") == 0 || strcmp(name,"clear-during-scan") == 0) {
        progress.action = strcmp(name,"root-during-scan") == 0 ? 2 : 3; progress.threshold = 2U;
        CHECK(UmiFileIndexRebuildWithOptions(index,&options) == UMI_STATUS_BUSY);
        CHECK(!progress.failed && umi_file_index_stats(index).files == 0U);
        CHECK(umi_path_equal(umi_file_index_stats(index).root, strcmp(name,"root-during-scan") == 0 ? other : root));
    } else if (strcmp(name,"visitor-cancel") == 0 || strcmp(name,"empty-cancel") == 0) {
        Visit visit = {token,0U}; const int empty = strcmp(name,"empty-cancel") == 0;
        if (empty) umi_cancellation_token_request(token);
        CHECK(UmiDirectoryWalkCancellable(empty ? other : root,NULL,CancelVisitor,&visit,token) == UMI_STATUS_CANCELLED);
        CHECK(visit.count == (empty ? 0U : 1U));
    } else if (strcmp(name,"success") == 0 || strcmp(name,"repeat") == 0) {
        CHECK(AddFiles(root,2U,128U) == EXIT_SUCCESS);
        unsigned attempts = strcmp(name,"repeat") == 0 ? 72U : 1U;
        for (unsigned n = 0U; n < attempts; ++n) {
            CHECK(UmiFileIndexRefreshStart(index,0U) == UMI_STATUS_OK);
            CHECK(UmiFileIndexRefreshWait(index,10000U) == UMI_STATUS_OK);
            CHECK(UmiFileIndexRefreshRead(index,&state) == UMI_STATUS_OK);
            CHECK(!state.active && state.status == UMI_STATUS_OK && state.filesScanned == 130U);
            CHECK(state.requestId == (uint64_t)n + 1U);
            CHECK(umi_file_index_stats(index).revision == before.revision + 1U);
        }
        CHECK(UmiFileIndexRefreshCancel(index) == UMI_STATUS_OK);
        CHECK(UmiFileIndexRefreshRead(index,&state) == UMI_STATUS_OK && state.status == UMI_STATUS_OK);
    } else if (strcmp(name,"failure") == 0) {
        CHECK(AddFiles(root,2U,1U) == EXIT_SUCCESS);
        CHECK(UmiFileIndexRefreshStart(index,0U) == UMI_STATUS_OK);
        CHECK(UmiFileIndexRefreshWait(index,10000U) == UMI_STATUS_OK);
        CHECK(UmiFileIndexRefreshRead(index,&state) == UMI_STATUS_OK);
        CHECK(!state.active && state.status == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(umi_file_index_stats(index).revision == before.revision && umi_file_index_stats(index).files == 2U);
    } else if (strcmp(name,"cancel-background") == 0 || strcmp(name,"switch-background") == 0 || strcmp(name,"destroy-background") == 0) {
        CHECK(AddFiles(root,2U,1024U) == EXIT_SUCCESS);
        CHECK(UmiFileIndexRefreshStart(index,0U) == UMI_STATUS_OK);
        if (strcmp(name,"destroy-background") == 0) {
            umi_file_index_destroy(index); index = NULL;
        } else if (strcmp(name,"switch-background") == 0) {
            CHECK(umi_file_index_set_root(index,other) == UMI_STATUS_OK);
            CHECK(UmiFileIndexRefreshWait(index,10000U) == UMI_STATUS_OK);
            CHECK(umi_file_index_stats(index).files == 0U && umi_path_equal(umi_file_index_stats(index).root,other));
            CHECK(UmiFileIndexRefreshStart(index,0U) == UMI_STATUS_OK);
            CHECK(UmiFileIndexRefreshWait(index,10000U) == UMI_STATUS_OK);
            CHECK(UmiFileIndexRefreshRead(index,&state) == UMI_STATUS_OK && state.status == UMI_STATUS_OK && state.filesScanned == 0U);
        } else {
            CHECK(UmiFileIndexRefreshCancel(index) == UMI_STATUS_OK);
            CHECK(UmiFileIndexRefreshWait(index,10000U) == UMI_STATUS_OK);
            CHECK(UmiFileIndexRefreshRead(index,&state) == UMI_STATUS_OK && !state.active);
            /* A scan can publish before the cancellation request wins. */
            CHECK(state.status == UMI_STATUS_CANCELLED || state.status == UMI_STATUS_OK);
            CHECK(umi_file_index_stats(index).files == (state.status == UMI_STATUS_CANCELLED ? 2U : 1026U));
        }
    } else if (strcmp(name,"relative-root") == 0) {
        UmiFileIndex *relative = NULL;
        UmiFileIndexConfig relativeConfig = umi_file_index_config_default("project");
        CHECK(umi_file_index_create(&relativeConfig,&relative) == UMI_STATUS_OK);
        CHECK(umi_file_index_rebuild(relative) == UMI_STATUS_OK);
        CHECK(UmiFileIndexRefreshStart(relative,0U) == UMI_STATUS_INVALID_ARGUMENT);
        umi_file_index_destroy(relative);
    } else { fputs("Unknown refresh case.\n",stderr); return EXIT_FAILURE; }
    umi_file_index_destroy(index); umi_cancellation_token_destroy(token);
    return EXIT_SUCCESS;
}
int main(int argc, char **argv)
{
    char cwd[UMI_PATH_CAPACITY], root[UMI_PATH_CAPACITY], other[UMI_PATH_CAPACITY];
    CHECK(argc == 2);
    CHECK(umi_fs_current_directory(cwd,sizeof(cwd)) == UMI_STATUS_OK);
    CHECK(umi_path_join(cwd,"project",root,sizeof(root)) == UMI_STATUS_OK);
    CHECK(umi_path_join(cwd,"other",other,sizeof(other)) == UMI_STATUS_OK);
    CHECK(!umi_fs_exists(root) && !umi_fs_exists(other));
    CHECK(umi_fs_make_directories(root) == UMI_STATUS_OK && umi_fs_make_directories(other) == UMI_STATUS_OK);
    int result = Run(argv[1],root,other);
    if (result == EXIT_SUCCESS) {
        CHECK(umi_fs_remove_tree(root) == UMI_STATUS_OK && umi_fs_remove_tree(other) == UMI_STATUS_OK);
        (void)printf("File-index refresh case passed: %s\n",argv[1]);
    }
    return result;
}
