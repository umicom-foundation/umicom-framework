/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/operational_state/test_directory_empty.c
 * PURPOSE: Traverse empty project folders and retain deterministic file ordering.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/platform/clock.h"
#include "umicom/platform/directory.h"
#include "umicom/platform/filesystem.h"
#define CHECK(test) do { if (!(test)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #test); return EXIT_FAILURE; } } while (0)
typedef struct Visits { unsigned count; int ordered; } Visits;
/* A visitor receives a.txt before b.txt regardless of file creation order. */
static UmiStatus Visit(const UmiFileInfo *info, void *context)
{
    Visits *visits = context;
    const char *expected = visits->count == 0U ? "a.txt" : "b.txt";
    if (visits->count >= 2U || strcmp(info->name, expected) != 0) visits->ordered = 0;
    ++visits->count;
    return UMI_STATUS_OK;
}
int main(void)
{
    char root[256], path[UMI_PATH_CAPACITY];
    UmiClock clock = umi_clock_system();
    UmiDirectoryWalkOptions options = umi_directory_walk_options_default();
    size_t count = 99U;
    Visits visits = {0U, 1};
    (void)snprintf(root, sizeof(root), "empty-project-%" PRIu64, clock.wall_nanoseconds(&clock));
    CHECK(!umi_fs_exists(root));
    CHECK(umi_fs_make_directories(root) == UMI_STATUS_OK);
    CHECK(umi_directory_count(root, &options, &count) == UMI_STATUS_OK && count == 0U);
    CHECK(umi_fs_join(path, sizeof(path), root, "b.txt") == UMI_STATUS_OK);
    CHECK(umi_fs_write_text(path, "Umicom note") == UMI_STATUS_OK);
    CHECK(umi_directory_count(root, &options, &count) == UMI_STATUS_OK && count == 1U);
    CHECK(umi_fs_join(path, sizeof(path), root, "a.txt") == UMI_STATUS_OK);
    CHECK(umi_fs_write_text(path, "Umicom note") == UMI_STATUS_OK);
    CHECK(umi_directory_walk(root, &options, Visit, &visits) == UMI_STATUS_OK);
    CHECK(visits.count == 2U && visits.ordered);
    CHECK(umi_fs_remove_tree(root) == UMI_STATUS_OK);
    return EXIT_SUCCESS;
}
