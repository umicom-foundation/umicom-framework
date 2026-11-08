/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_lifecycle/test_file_observation.c
 *
 * PURPOSE:
 *   Verify real selected-file observations preserve bytes, ownership and exclusive evidence destinations.
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
#include "umicom/toolchain/file_observation.h"
#include "umicom/platform/filesystem.h"
#include "../build_log/fixture.h"
#include <stdint.h>
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *mode = argv[1];
    char root[UMI_PATH_CAPACITY], first[UMI_PATH_CAPACITY], second[UMI_PATH_CAPACITY], report[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    FixturePath(first, root, "caf\xc3\xa9 tool.bin");
    FixturePath(second, root, "empty.bin");
    FixturePath(report, root, "observation.json");
    CHECK(umi_fs_write_bytes(first, "abc", 3U) == UMI_STATUS_OK);
    CHECK(umi_fs_write_bytes(second, "", 0U) == UMI_STATUS_OK);
    const char *paths[] = {first, second};
    UmiFileObservation *observation = NULL;
    size_t failed = 0U;
    if (!strcmp(mode, "missing")) { paths[1] = report; }
    else if (!strcmp(mode, "directory")) { paths[1] = root; }
    else if (!strcmp(mode, "duplicate")) { paths[1] = first; }
    else if (!strcmp(mode, "relative")) { paths[1] = "relative.bin"; }
    else if (!strcmp(mode, "null-path")) { paths[1] = NULL; }
    else if (!strcmp(mode, "symlink")) {
#ifdef _WIN32
        if (!FixtureSymbolicLink(report, first, 0)) return 77;
#else
        CHECK(symlink(first, report) == 0);
#endif
        paths[1] = report;
    }
    UmiStatus status = UmiFileObservationCapture(paths, 2U,
        !strcmp(mode, "small-limit") ? 2U : 16U, &observation, &failed);
    if (!strcmp(mode, "missing") || !strcmp(mode, "directory") || !strcmp(mode, "duplicate") ||
        !strcmp(mode, "relative") || !strcmp(mode, "null-path") || !strcmp(mode, "symlink") ||
        !strcmp(mode, "small-limit")) {
        CHECK(status != UMI_STATUS_OK && observation == NULL);
        CHECK(failed == (!strcmp(mode, "small-limit") ? 0U : 1U));
        return 0;
    }
    CHECK(status == UMI_STATUS_OK && observation != NULL && failed == SIZE_MAX);
    CHECK(UmiFileObservationCount(observation) == 2U && UmiFileObservationCount(NULL) == 0U);
    CHECK(UmiFileObservationAt(NULL, 0U) == NULL && UmiFileObservationAt(observation, 2U) == NULL);
    const UmiFileObservationItem *item = UmiFileObservationAt(observation, 0U);
    CHECK(item != NULL && item->bytes == 3U && !strcmp(item->path, first));
    CHECK(!strcmp(item->sha256, "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"));
    item = UmiFileObservationAt(observation, 1U);
    CHECK(item != NULL && item->bytes == 0U);
    CHECK(!strcmp(item->sha256, "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"));
    char *json = malloc(UMI_FILE_OBSERVATION_JSON_CAPACITY);
    CHECK(json != NULL);
    size_t size;
    CHECK(UmiFileObservationFormat(observation, json, UMI_FILE_OBSERVATION_JSON_CAPACITY, &size) == UMI_STATUS_OK);
    CHECK(size == strlen(json) && strstr(json, "\"atomicSnapshot\":false") != NULL);
    CHECK(strstr(json, "\"loadedModuleIdentity\":false") != NULL && strstr(json, "\"totalBytes\":3") != NULL);
    if (!strcmp(mode, "owned")) {
        CHECK(umi_fs_write_bytes(first, "changed", 7U) == UMI_STATUS_OK);
        first[0] = 'x';
        CHECK(UmiFileObservationAt(observation, 0U)->path[0] != 'x');
        CHECK(UmiFileObservationAt(observation, 0U)->bytes == 3U);
    } else if (!strcmp(mode, "format-bounds")) {
        char *exact = malloc(size + 1U); CHECK(exact != NULL);
        size_t written = SIZE_MAX;
        CHECK(UmiFileObservationFormat(observation, exact, size, &written) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(exact[0] == '\0' && written == 0U);
        CHECK(UmiFileObservationFormat(observation, exact, size + 1U, &written) == UMI_STATUS_OK);
        CHECK(written == size && !strcmp(exact, json));
        CHECK(UmiFileObservationFormat(NULL, exact, size + 1U, &written) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(exact[0] == '\0' && written == 0U);
        CHECK(UmiFileObservationFormat(observation, NULL, 0U, &written) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(written == 0U); free(exact);
    } else if (!strcmp(mode, "save") || !strcmp(mode, "save-existing")) {
        CHECK(UmiFileObservationSave(observation, report) == UMI_STATUS_OK);
        if (!strcmp(mode, "save-existing"))
            CHECK(UmiFileObservationSave(observation, report) == UMI_STATUS_ALREADY_EXISTS);
        size_t readSize; unsigned char *bytes = FixtureRead(report, &readSize);
        CHECK(readSize == size && memcmp(bytes, json, size) == 0); free(bytes);
    } else if (!strcmp(mode, "save-input")) {
        CHECK(UmiFileObservationSave(observation, first) == UMI_STATUS_ALREADY_EXISTS);
        size_t readSize; unsigned char *bytes = FixtureRead(first, &readSize);
        CHECK(readSize == 3U && memcmp(bytes, "abc", 3U) == 0); free(bytes);
    } else if (!strcmp(mode, "save-invalid")) {
        CHECK(UmiFileObservationSave(observation, "relative.json") != UMI_STATUS_OK);
        CHECK(UmiFileObservationSave(NULL, report) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!umi_fs_exists(report));
    } else CHECK(!strcmp(mode, "capture"));
    free(json);
    UmiFileObservationDestroy(observation);
    UmiFileObservationDestroy(NULL);
    return 0;
}
