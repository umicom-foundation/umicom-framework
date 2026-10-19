/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform/test_file_lease.c
 * Purpose: Exercise cooperative ownership, content preservation and independent native processes.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#include "umicom/platform/file_lease.h"
#include "umicom/platform/process.h"
#include "umicom/platform/native_arguments.h"

static void Child(const char *program, const char *mode, const char *path, const char *root)
{
    const char *arguments[] = {mode, path};
    UmiProcessRequest request = {0};
    request.program = program; request.arguments = arguments; request.argument_count = 2U;
    request.working_directory = root; request.capture_stdout = 1; request.capture_stderr = 1;
    request.timeout_ms = 5000U; request.window_mode = UMI_PROCESS_WINDOW_HIDDEN;
    UmiProcessResult *result = calloc(1U, sizeof(*result)); CHECK(result != NULL);
    UmiStatus status = umi_process_execute(&request, result);
    if (status != UMI_STATUS_OK || !result->launched || result->exit_code != 0)
        fprintf(stderr, "Lease child: %s\n", result->output);
    CHECK(status == UMI_STATUS_OK && result->launched && result->exit_code == 0);
    free(result);
}

static int Run(int argc, char **argv)
{
    if (argc != 3) return 2;
    const char *name = argv[1];
    UmiFileLease *first = NULL, *second = NULL;
    if (strcmp(name, "child-busy") == 0) {
        CHECK(UmiFileLeaseTryAcquire(argv[2], &first) == UMI_STATUS_BUSY && first == NULL);
        return 0;
    }
    if (strcmp(name, "child-exit") == 0) {
        CHECK(UmiFileLeaseTryAcquire(argv[2], &first) == UMI_STATUS_OK);
        /* Intentionally bypass library cleanup: native process termination
         * must release ownership, leaving the persistent filename reusable. */
        _Exit(0);
    }
    char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY], other[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    FixturePath(path, root, strcmp(name, "unicode") == 0 ? "lease-\xc3\xa9.lock" : "owner.lock");
    FixturePath(other, root, "other.lock");
    if (strcmp(name, "invalid") == 0) {
        CHECK(UmiFileLeaseTryAcquire(NULL, &first) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiFileLeaseTryAcquire("relative.lock", &first) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiFileLeaseTryAcquire(path, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiFileLeaseRelease(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiFileLeaseRelease(&first) == UMI_STATUS_OK && first == NULL);
        CHECK(UmiFileLeaseTryAcquire(path, &first) == UMI_STATUS_OK);
        UmiFileLease *saved = first;
        CHECK(UmiFileLeaseTryAcquire(other, &first) == UMI_STATUS_INVALID_ARGUMENT && first == saved);
    } else if (strcmp(name, "missing-parent") == 0) {
        FixturePath(path, root, "absent/owner.lock");
        CHECK(UmiFileLeaseTryAcquire(path, &first) == UMI_STATUS_NOT_FOUND && first == NULL);
    } else if (strcmp(name, "directory") == 0) {
        CHECK(UmiFileLeaseTryAcquire(root, &first) != UMI_STATUS_OK && first == NULL);
    } else if (strcmp(name, "fifo") == 0) {
#ifndef _WIN32
        CHECK(mkfifo(path, 0600) == 0);
        CHECK(UmiFileLeaseTryAcquire(path, &first) == UMI_STATUS_INVALID_ARGUMENT && first == NULL);
#else
        return 2; /* Not registered on hosts without POSIX FIFOs. */
#endif
    } else if (strcmp(name, "symlink") == 0) {
        UmiOutputFile *file = NULL;
        CHECK(UmiOutputFileCreate(other, &file) == UMI_STATUS_OK); UmiOutputFileDestroy(file);
#ifdef _WIN32
        if (!FixtureSymbolicLink(path, other, 0)) return 77;
#else
        CHECK(symlink(other, path) == 0);
#endif
        CHECK(UmiFileLeaseTryAcquire(path, &first) != UMI_STATUS_OK && first == NULL);
    } else {
        if (strcmp(name, "contents") == 0) {
            UmiOutputFile *file = NULL;
            CHECK(UmiOutputFileCreate(path, &file) == UMI_STATUS_OK);
            CHECK(UmiOutputFileWrite(file, "keep\0bytes", 10U) == UMI_STATUS_OK);
            UmiOutputFileDestroy(file);
        }
        CHECK(UmiFileLeaseTryAcquire(path, &first) == UMI_STATUS_OK);
        CHECK(UmiFileLeaseTryAcquire(path, &second) == UMI_STATUS_BUSY && second == NULL);
        if (strcmp(name, "different") == 0) {
            CHECK(UmiFileLeaseTryAcquire(other, &second) == UMI_STATUS_OK);
            CHECK(UmiFileLeaseRelease(&second) == UMI_STATUS_OK);
        }
        if (strcmp(name, "cross-process") == 0) Child(argv[2], "child-busy", path, root);
        CHECK(UmiFileLeaseRelease(&first) == UMI_STATUS_OK && first == NULL);
        if (strcmp(name, "cross-process") == 0) Child(argv[2], "child-exit", path, root);
        CHECK(UmiFileLeaseTryAcquire(path, &second) == UMI_STATUS_OK);
        CHECK(UmiFileLeaseRelease(&second) == UMI_STATUS_OK);
        size_t length; unsigned char *bytes = FixtureRead(path, &length);
        if (strcmp(name, "contents") == 0) CHECK(length == 10U && memcmp(bytes, "keep\0bytes", 10U) == 0);
        else CHECK(length == 0U);
        free(bytes);
    }
    CHECK(UmiFileLeaseRelease(&first) == UMI_STATUS_OK);
    CHECK(UmiFileLeaseRelease(&first) == UMI_STATUS_OK);
    return 0;
}
int main(int argc, char **argv)
{
    int result = 2;
    return UmiNativeArgumentsDispatch(argc, argv, Run, &result) == UMI_STATUS_OK ? result : 2;
}
