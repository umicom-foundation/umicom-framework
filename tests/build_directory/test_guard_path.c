/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/build_directory/test_guard_path.c
 * Purpose: Keep guard names stable and reject ambiguous build-directory paths.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "../build_log/fixture.h"
#include "umicom/build/directory_guard.h"
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY], expected[UMI_PATH_CAPACITY];
    FixtureDirectory(root); FixturePath(expected, root, ".umicom-build.lock");
    memset(path, 'q', sizeof(path));
    if (strcmp(argv[1], "format") == 0) {
        CHECK(UmiBuildDirectoryGuardPath(root, path, sizeof(path)) == UMI_STATUS_OK);
        CHECK(strcmp(path, expected) == 0);
        char trailing[UMI_PATH_CAPACITY]; FixturePath(trailing, root, "");
        CHECK(UmiBuildDirectoryGuardPath(trailing, path, sizeof(path)) == UMI_STATUS_OK);
        CHECK(strcmp(path, expected) == 0);
    } else if (strcmp(argv[1], "invalid") == 0) {
        CHECK(UmiBuildDirectoryGuardPath(NULL, path, sizeof(path)) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBuildDirectoryGuardPath(root, NULL, sizeof(path)) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBuildDirectoryGuardPath("", path, sizeof(path)) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBuildDirectoryGuardPath("relative", path, sizeof(path)) == UMI_STATUS_INVALID_ARGUMENT);
        FixturePath(expected, root, "../other");
        CHECK(UmiBuildDirectoryGuardPath(expected, path, sizeof(path)) == UMI_STATUS_INVALID_ARGUMENT);
        for (size_t i = 0U; i < sizeof(path); ++i) CHECK(path[i] == 'q');
    } else if (strcmp(argv[1], "capacity") == 0) {
        CHECK(UmiBuildDirectoryGuardPath(root, path, strlen(expected)) == UMI_STATUS_CAPACITY_EXCEEDED);
        for (size_t i = 0U; i < sizeof(path); ++i) CHECK(path[i] == 'q');
        CHECK(UmiBuildDirectoryGuardPath(root, path, strlen(expected) + 1U) == UMI_STATUS_OK);
    } else if (strcmp(argv[1], "ownership") == 0) {
        UmiFileLease *a = NULL, *b = NULL;
        CHECK(UmiBuildDirectoryGuardAcquire(root, &a) == UMI_STATUS_OK);
        CHECK(UmiFileLeaseTryAcquire(expected, &b) == UMI_STATUS_BUSY && b == NULL);
        CHECK(UmiFileLeaseRelease(&a) == UMI_STATUS_OK);
        CHECK(UmiBuildDirectoryGuardAcquire(root, &b) == UMI_STATUS_OK);
        CHECK(UmiFileLeaseRelease(&b) == UMI_STATUS_OK);
    } else return 2;
    return 0;
}
