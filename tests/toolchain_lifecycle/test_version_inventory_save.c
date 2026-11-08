/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_lifecycle/test_version_inventory_save.c
 *
 * PURPOSE:
 *   Verify exclusive inventory persistence and retain evidence after injected storage failures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/version_inventory.h"
#include "umicom/platform/output_file.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/clock.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static const char *mode;
static unsigned creates, writes, closes;
static UmiStatus create_file(const char *path, UmiOutputFile **out)
{
    ++creates;
    if (!strcmp(mode, "create-failure")) { *out = NULL; return UMI_STATUS_PERMISSION_DENIED; }
    return UmiOutputFileCreate(path, out);
}
static UmiStatus write_file(UmiOutputFile *file, const void *bytes, size_t length)
{
    ++writes;
    if (!strcmp(mode, "write-failure")) {
        UmiStatus result = UmiOutputFileWrite(file, bytes, length < 7U ? length : 7U);
        return result == UMI_STATUS_OK ? UMI_STATUS_IO_ERROR : result;
    }
    return UmiOutputFileWrite(file, bytes, length);
}
static UmiStatus close_file(UmiOutputFile *file)
{
    ++closes;
    UmiStatus result = UmiOutputFileClose(file);
    return !strcmp(mode, "close-failure") && result == UMI_STATUS_OK ? UMI_STATUS_IO_ERROR : result;
}
/* Exercise the real writer while replacing only its storage boundary. Partial
 * writes use a real exclusive file, so readback proves evidence was retained. */
UmiStatus FixtureFormat(const UmiToolchainDiscoveryReport *, size_t, UmiStatus, char *, size_t, size_t *);
UmiStatus FixtureSave(const UmiToolchainDiscoveryReport *, size_t, UmiStatus, const char *);
#define UmiOutputFileCreate create_file
#define UmiOutputFileWrite write_file
#define UmiOutputFileClose close_file
#define umi_toolchain_version_inventory_format_sized FixtureFormat
#define umi_toolchain_version_inventory_save_sized FixtureSave
#include "../../src/toolchain/version_inventory.c"
#undef umi_toolchain_version_inventory_save_sized
#undef umi_toolchain_version_inventory_format_sized
#undef UmiOutputFileClose
#undef UmiOutputFileWrite
#undef UmiOutputFileCreate
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); failed = 1; goto finish; } } while (0)
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    mode = argv[1];
    int failed = 0, owns = 0;
    char directory[UMI_PATH_CAPACITY] = {0}, temp[UMI_PATH_CAPACITY], file[UMI_PATH_CAPACITY];
    unsigned char *readback = NULL;
    UmiToolchainDiscoveryReport *r = calloc(1U, sizeof(*r));
    char *expected_json = malloc(UMI_TOOLCHAIN_VERSION_INVENTORY_CAPACITY);
    CHECK(r && expected_json);
    umi_toolchain_profile_init(&r->profile);
    UmiClock clock = umi_clock_system();
    char leaf[160];
    (void)snprintf(leaf, sizeof(leaf), "umicom-inventory-%s-%llu", mode,
        (unsigned long long)clock.monotonic_nanoseconds(&clock));
    CHECK(umi_fs_temp_directory(temp, sizeof(temp)) == UMI_STATUS_OK);
    CHECK(umi_path_join(temp, leaf, directory, sizeof(directory)) == UMI_STATUS_OK);
    CHECK(!umi_fs_exists(directory));
    CHECK(umi_fs_make_directories(directory) == UMI_STATUS_OK); owns = 1;
    CHECK(umi_path_join(directory, !strcmp(mode, "unicode-path") ? "\xc3\xa9.json" : "tools.json",
        file, sizeof(file)) == UMI_STATUS_OK);
    UmiStatus expected = UMI_STATUS_OK;
    const char *destination = file;
    if (!strcmp(mode, "existing")) {
        CHECK(umi_fs_write_text(file, "earlier evidence") == UMI_STATUS_OK);
        expected = UMI_STATUS_ALREADY_EXISTS;
    } else if (!strcmp(mode, "invalid-report")) {
        r->runtime_probe_passed = 1; expected = UMI_STATUS_INVALID_STATE;
    } else if (!strcmp(mode, "relative-path")) {
        destination = "relative.json"; expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (!strcmp(mode, "write-failure") || !strcmp(mode, "close-failure")) expected = UMI_STATUS_IO_ERROR;
    else if (!strcmp(mode, "create-failure")) expected = UMI_STATUS_PERMISSION_DENIED;
    else if (!strcmp(mode, "missing-parent")) {
        CHECK(umi_path_join(directory, "missing/tools.json", file, sizeof(file)) == UMI_STATUS_OK);
        expected = UMI_STATUS_NOT_FOUND;
    } else if (strcmp(mode, "new") && strcmp(mode, "unicode-path")) { failed = 2; goto finish; }
    CHECK(FixtureSave(r, sizeof(*r), UMI_STATUS_NOT_FOUND, destination) == expected);
    size_t length = 0U;
    if (expected == UMI_STATUS_OK || !strcmp(mode, "close-failure") || !strcmp(mode, "write-failure")) {
        CHECK(umi_fs_read_bytes(file, &readback, &length) == UMI_STATUS_OK);
        size_t expected_length = 0U;
        CHECK(FixtureFormat(r, sizeof(*r), UMI_STATUS_NOT_FOUND, expected_json,
            UMI_TOOLCHAIN_VERSION_INVENTORY_CAPACITY, &expected_length) == UMI_STATUS_OK);
        CHECK(length == (!strcmp(mode, "write-failure") ? 7U : expected_length));
        CHECK(!memcmp(readback, expected_json, length));
        CHECK(creates == 1U && writes == 1U && closes == 1U);
    } else if (!strcmp(mode, "existing")) {
        CHECK(umi_fs_read_bytes(file, &readback, &length) == UMI_STATUS_OK);
        CHECK(length == strlen("earlier evidence") && !memcmp(readback, "earlier evidence", length));
        CHECK(creates == 1U && writes == 0U && closes == 0U);
    } else {
        CHECK(!umi_fs_exists(file));
        CHECK(writes == 0U && closes == 0U);
        if (!strcmp(mode, "invalid-report") || !strcmp(mode, "relative-path")) CHECK(creates == 0U);
        else CHECK(creates == 1U);
    }
finish:
    umi_fs_free_bytes(readback); free(expected_json); free(r);
    if (owns && umi_fs_remove_tree(directory) != UMI_STATUS_OK) failed = 1;
    return failed;
}
