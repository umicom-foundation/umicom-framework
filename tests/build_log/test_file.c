/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/build_log/test_file.c
 * PURPOSE: Check exclusive raw writes, Unicode names, close ownership and path refusal.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "fixture.h"

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *name = argv[1]; char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
    FixtureDirectory(root); FixturePath(path, root, strcmp(name, "unicode") == 0 ? "caf\xc3\xa9.log" : "output.log");
    UmiOutputFile *file = NULL; UmiOutputFileSnapshot state;
    if (strcmp(name, "invalid") == 0) {
        CHECK(UmiOutputFileCreate("relative.log", &file) == UMI_STATUS_INVALID_ARGUMENT && file == NULL);
        CHECK(UmiOutputFileValidatePath(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiOutputFileValidatePath("") == UMI_STATUS_INVALID_ARGUMENT);
        FixturePath(path, root, "../outside.log"); CHECK(UmiOutputFileValidatePath(path) == UMI_STATUS_INVALID_ARGUMENT);
        char oversized[UMI_PATH_CAPACITY + 1U]; memset(oversized, 'a', sizeof(oversized)); oversized[UMI_PATH_CAPACITY] = '\0';
        CHECK(UmiOutputFileValidatePath(oversized) == UMI_STATUS_CAPACITY_EXCEEDED);
#ifdef _WIN32
        const char *invalid[] = {"C:/NUL.log", "C:/CON .log", "C:/COM\xc2\xb9.log", "C:/file:stream", "\\\\.\\pipe\\capture", "C:relative", "C:/bad\xff.log"};
        for (size_t i = 0U; i < sizeof(invalid) / sizeof(invalid[0]); ++i)
            CHECK(UmiOutputFileValidatePath(invalid[i]) == UMI_STATUS_INVALID_ARGUMENT);
#endif
        CHECK(UmiOutputFileRead(NULL, &state) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiOutputFileClose(NULL) == UMI_STATUS_INVALID_ARGUMENT); UmiOutputFileDestroy(NULL); return 0;
    }
    if (strcmp(name, "missing-parent") == 0) {
        FixturePath(path, root, "absent/output.log"); CHECK(UmiOutputFileCreate(path, &file) == UMI_STATUS_NOT_FOUND); return 0;
    }
    if (strcmp(name, "symlink") == 0) {
#ifdef _WIN32
        return 77; /* Creating Windows symlinks requires a machine-specific privilege. */
#else
        CHECK(symlink("missing-target", path) == 0);
        CHECK(UmiOutputFileCreate(path, &file) == UMI_STATUS_ALREADY_EXISTS && file == NULL); return 0;
#endif
    }
    CHECK(UmiOutputFileCreate(path, &file) == UMI_STATUS_OK);
    if (strcmp(name, "collision") == 0) {
        UmiOutputFile *other = NULL;
        CHECK(UmiOutputFileWrite(file, "keep", 4U) == UMI_STATUS_OK);
        CHECK(UmiOutputFileCreate(path, &other) == UMI_STATUS_ALREADY_EXISTS && other == NULL);
    } else if (strcmp(name, "large") == 0) {
        unsigned char *bytes = malloc(180000U); CHECK(bytes != NULL); memset(bytes, 'z', 180000U); bytes[17] = 0U;
        CHECK(UmiOutputFileWrite(file, bytes, 180000U) == UMI_STATUS_OK); free(bytes);
    } else if (strcmp(name, "binary") == 0 || strcmp(name, "unicode") == 0) {
        const unsigned char bytes[] = {'a', 0U, 0xffU, '\n', 'z'};
        CHECK(UmiOutputFileWrite(file, bytes, sizeof(bytes)) == UMI_STATUS_OK);
    } else CHECK(strcmp(name, "empty") == 0 || strcmp(name, "destroy") == 0);
    CHECK(UmiOutputFileWrite(file, NULL, 0U) == UMI_STATUS_OK);
    CHECK(UmiOutputFileRead(file, &state) == UMI_STATUS_OK && !state.closed);
    CHECK(UmiOutputFileRead(file, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    if (strcmp(name, "destroy") == 0) UmiOutputFileDestroy(file);
    else {
        CHECK(UmiOutputFileClose(file) == UMI_STATUS_OK && UmiOutputFileClose(file) == UMI_STATUS_OK);
        CHECK(UmiOutputFileWrite(file, "late", 4U) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiOutputFileRead(file, &state) == UMI_STATUS_OK && state.closed);
        UmiOutputFileDestroy(file);
    }
    size_t length; unsigned char *actual = FixtureRead(path, &length);
    CHECK(length == (size_t)state.bytes_written);
    if (strcmp(name, "collision") == 0) CHECK(length == 4U && memcmp(actual, "keep", 4U) == 0);
    if (strcmp(name, "large") == 0) CHECK(length == 180000U && actual[17] == 0U && actual[length - 1U] == 'z');
    if (strcmp(name, "binary") == 0 || strcmp(name, "unicode") == 0) CHECK(length == 5U && actual[1] == 0U && actual[2] == 0xffU);
    free(actual); printf("Retained log: %s\n", path); return 0;
}
