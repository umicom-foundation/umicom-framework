/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform/test_local_replace_report.c
 *
 * PURPOSE:
 *   Verify replacement reports identify failures and reset prior caller-owned diagnostics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#include "umicom/platform/local_replace.h"

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    char root[UMI_PATH_CAPACITY], filename[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    FixturePath(filename, root, "settings");
    UmiLocalFileReplaceReport report = {
        .stage = UMI_LOCAL_REPLACE_PUBLISH,
        .status = UMI_STATUS_IO_ERROR,
        .last_publish_error = 42U,
        .publish_attempts = 42U,
        .cleanup_attempted = true,
        .cleanup_succeeded = true
    };
    UmiStatus status;
    if (strcmp(argv[1], "invalid-report") == 0)
    {
        status = UmiLocalFileReplaceDetailed("relative", "x", 1U, &report);
        CHECK(status == UMI_STATUS_INVALID_ARGUMENT && report.stage == UMI_LOCAL_REPLACE_VALIDATE);
    }
    else if (strcmp(argv[1], "invalid-bytes-report") == 0)
    {
        status = UmiLocalFileReplaceDetailed(filename, NULL, 1U, &report);
        CHECK(status == UMI_STATUS_INVALID_ARGUMENT && report.stage == UMI_LOCAL_REPLACE_VALIDATE);
    }
    else if (strcmp(argv[1], "directory-report") == 0)
    {
        status = UmiLocalFileReplaceDetailed(root, "x", 1U, &report);
        CHECK(status == UMI_STATUS_INVALID_ARGUMENT && report.stage == UMI_LOCAL_REPLACE_DESTINATION);
    }
    else if (strcmp(argv[1], "empty-report") == 0)
    {
        status = UmiLocalFileReplaceDetailed(filename, NULL, 0U, &report);
        CHECK(status == UMI_STATUS_OK && report.stage == UMI_LOCAL_REPLACE_COMPLETE);
        CHECK(report.publish_attempts >= 1U && report.publish_attempts <= 8U);
        size_t length;
        unsigned char *bytes = FixtureRead(filename, &length);
        CHECK(length == 0U);
        free(bytes);
    }
    else
        return 2;
    CHECK(report.status == status && !report.cleanup_attempted && !report.cleanup_succeeded);
    if (status != UMI_STATUS_OK)
        CHECK(report.publish_attempts == 0U && report.last_publish_error == 0U);
    return 0;
}
