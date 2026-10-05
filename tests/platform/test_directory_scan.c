/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform/test_directory_scan.c
 * PURPOSE: Exercise bounded streaming enumeration and explicit cancellation without recursive discovery.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../build_log/fixture.h"
#include "umicom/platform/directory_scan.h"
#include "umicom/platform/rooted_files.h"
#include "umicom/platform/filesystem.h"
typedef struct Visit
{
    size_t count;
    int stop;
    UmiCancellationToken *cancel;
    int bad_depth;
    size_t bytes;
} Visit;
static UmiStatus Seen(const UmiFileInfo *file, void *data)
{
    Visit *visit = data;
    ++visit->count;
    visit->bytes += (size_t)file->size;
    if (file->depth != 1U)
        visit->bad_depth = 1;
    if (visit->cancel != NULL)
        umi_cancellation_token_request(visit->cancel);
    return visit->stop ? UMI_STATUS_PERMISSION_DENIED : UMI_STATUS_OK;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1],
               *cases[] = {"empty",      "exact-limit",   "limit",     "cancellation", "callback-stop",
                           "relative",   "dot-parent",    "file-root", "missing",      "null-visitor",
                           "zero-limit", "maximum-limit", "shallow",   "metadata"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(mode, cases[i]) == 0)
            known = 1;
    CHECK(known);
    char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    Visit visit = {0};
    if (strcmp(mode, "empty") == 0)
    {
        CHECK(UmiDirectoryScanShallow(root, 1U, Seen, &visit, NULL) == UMI_STATUS_OK && visit.count == 0U);
        return 0;
    }
    CHECK(UmiRootedFileWrite(root, "one", "1", 1U) == UMI_STATUS_OK);
    CHECK(UmiRootedFileWrite(root, "two", "22", 2U) == UMI_STATUS_OK);
    const char *directory = root;
    size_t maximum = 2U;
    UmiStatus wanted = UMI_STATUS_OK;
    UmiCancellationToken *cancel = NULL;
    if (strcmp(mode, "limit") == 0)
    {
        maximum = 1U;
        wanted = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (strcmp(mode, "cancellation") == 0)
    {
        CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
        visit.cancel = cancel;
        wanted = UMI_STATUS_CANCELLED;
    }
    if (strcmp(mode, "callback-stop") == 0)
    {
        visit.stop = 1;
        wanted = UMI_STATUS_PERMISSION_DENIED;
    }
    if (strcmp(mode, "relative") == 0)
    {
        directory = "relative";
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "dot-parent") == 0)
    {
        FixturePath(path, root, "../outside");
        directory = path;
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "file-root") == 0)
    {
        FixturePath(path, root, "one");
        directory = path;
        wanted = UMI_STATUS_PERMISSION_DENIED;
    }
    if (strcmp(mode, "missing") == 0)
    {
        FixturePath(path, root, "absent");
        directory = path;
        wanted = UMI_STATUS_NOT_FOUND;
    }
    if (strcmp(mode, "null-visitor") == 0)
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(mode, "zero-limit") == 0)
    {
        maximum = 0U;
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "maximum-limit") == 0)
    {
        maximum = UMI_DIRECTORY_SCAN_MAXIMUM_ENTRIES + 1U;
        wanted = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (strcmp(mode, "shallow") == 0)
    {
        FixturePath(path, root, "nested");
        CHECK(umi_fs_make_directories(path) == UMI_STATUS_OK);
        CHECK(UmiRootedFileWrite(path, "hidden", "x", 1U) == UMI_STATUS_OK);
        maximum = 3U;
    }
    CHECK(UmiDirectoryScanShallow(directory, maximum, strcmp(mode, "null-visitor") == 0 ? NULL : Seen, &visit,
                                  cancel) == wanted &&
          !visit.bad_depth);
    if (wanted == UMI_STATUS_OK)
        CHECK(visit.count == maximum);
    if (strcmp(mode, "limit") == 0 || strcmp(mode, "cancellation") == 0 || strcmp(mode, "callback-stop") == 0)
        CHECK(visit.count == 1U);
    if (strcmp(mode, "metadata") == 0)
        CHECK(visit.bytes == 3U);
    umi_cancellation_token_destroy(cancel);
    return 0;
}
