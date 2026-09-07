/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_platform_executable_path.c
 * Purpose: Reject truncated process paths before they become installation roots.
 * Author: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/filesystem.h"

#include <stdio.h>
#include <string.h>

/* Keep the checks active in Release; no executable or filesystem is modified. */
#define CHECK(expression) do { \
    if (!(expression)) { \
        (void)fprintf(stderr, "Line %d: %s\n", __LINE__, #expression); \
        return 1; \
    } \
} while (0)

int main(void)
{
    char path[UMI_PATH_CAPACITY] = "sentinel";
    char small[2] = {'x', 'y'};
    char single = 'x';
    UmiStatus status;
    CHECK(umi_fs_executable_path(NULL, sizeof(path)) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_fs_executable_path(&single, 0U) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(single == 'x');
    CHECK(umi_fs_executable_path(&single, 1U) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(single == '\0');
    status = umi_fs_executable_path(path, sizeof(path));
#if defined(_WIN32) || defined(__linux__)
    CHECK(status == UMI_STATUS_OK);
    CHECK(umi_fs_is_absolute(path));
    CHECK(strlen(path) > 1U && strlen(path) < sizeof(path));
    CHECK(umi_fs_executable_path(small, sizeof(small)) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(small[0] == '\0');
#else
    /* Lack of a platform adapter is explicit, never a current-folder guess. */
    CHECK(status == UMI_STATUS_NOT_IMPLEMENTED && path[0] == '\0');
    CHECK(umi_fs_executable_path(small, sizeof(small)) == UMI_STATUS_NOT_IMPLEMENTED);
    CHECK(small[0] == '\0');
#endif
    return 0;
}
