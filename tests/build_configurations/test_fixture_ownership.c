/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/build_configurations/test_fixture_ownership.c
 * PURPOSE: Verify database fixture reservation preserves existing evidence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "../storage_fixture.h"
#include "umicom/platform/filesystem.h"
#include <string.h>

#define CHECK(value) do { if (!(value)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); return 1; } } while (0)

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    char directory[UMI_PATH_CAPACITY], base[UMI_PATH_CAPACITY];
    CHECK(umi_fs_current_directory(directory, sizeof directory) == UMI_STATUS_OK);
    CHECK(umi_path_join(directory, "storage-owner.sqlite", base, sizeof base) == UMI_STATUS_OK);
    if (strcmp(argv[1], "capacity") == 0)
    {
        char short_path[1] = {'x'};
        CHECK(UmiTestStorageReserve(base, short_path, sizeof short_path) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(short_path[0] == '\0');
        return 0;
    }
    if (strcmp(argv[1], "invalid-path") == 0)
    {
        char path[UMI_PATH_CAPACITY] = "unchanged";
        CHECK(UmiTestStorageReserve(NULL, path, sizeof path) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(path[0] == '\0');
        CHECK(UmiTestStorageReserve("relative.sqlite", path, sizeof path) != UMI_STATUS_OK);
        CHECK(path[0] == '\0');
        CHECK(UmiTestStorageReserve(base, NULL, 1U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiTestStorageReserve(base, path, 0U) == UMI_STATUS_INVALID_ARGUMENT);
        return 0;
    }
    CHECK(strcmp(argv[1], "collision") == 0);
    char first[UMI_PATH_CAPACITY], second[UMI_PATH_CAPACITY], third[UMI_PATH_CAPACITY];
    CHECK(UmiTestStorageReserve(base, first, sizeof first) == UMI_STATUS_OK);
    const char evidence[] = "Retain this failed-run evidence.";
    CHECK(umi_fs_write_bytes(first, evidence, sizeof evidence) == UMI_STATUS_OK);
    /* Reserve against the occupied leaf twice: simultaneous or repeated test
     * processes must never open that leaf for truncation or share a database. */
    CHECK(UmiTestStorageReserve(first, second, sizeof second) == UMI_STATUS_OK);
    CHECK(UmiTestStorageReserve(first, third, sizeof third) == UMI_STATUS_OK);
    CHECK(strcmp(first, second) != 0 && strcmp(first, third) != 0 && strcmp(second, third) != 0);
    unsigned char *bytes = NULL;
    size_t length = 0U;
    CHECK(umi_fs_read_bytes(first, &bytes, &length) == UMI_STATUS_OK);
    int preserved = length == sizeof evidence && memcmp(bytes, evidence, sizeof evidence) == 0;
    umi_fs_free_bytes(bytes);
    CHECK(preserved);
    /* Only successful reservations are removed. Failed assertions intentionally
     * leave their owned fixtures available to diagnose the observed state. */
    CHECK(umi_fs_remove_tree(third) == UMI_STATUS_OK);
    CHECK(umi_fs_remove_tree(second) == UMI_STATUS_OK);
    CHECK(umi_fs_remove_tree(first) == UMI_STATUS_OK);
    return 0;
}
