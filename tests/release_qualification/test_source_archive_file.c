/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_qualification/test_source_archive_file.c
 *
 * PURPOSE:
 *   Verify explicit baseline persistence preserves existing evidence and refuses invalid loads.
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
#include "fixture.h"
#include "../build_log/fixture.h"
#include "umicom/distribution/runtime/source_observation_archive.h"
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    REQUIRE(argc == 2);
    const char *mode = argv[1];
    const char *modes[] = {"reopen", "existing", "relative-save", "relative-load",
        "wrong-receipt", "missing", "malformed", "null-receipt", "null-observation",
        "null-save-output", "null-load-output", "occupied-output"};
    bool known = false;
    for (size_t i = 0U; i < sizeof(modes) / sizeof(modes[0]); ++i)
        if (strcmp(mode, modes[i]) == 0) known = true;
    REQUIRE(known);
    QualificationFixture f;
    REQUIRE(QualificationStart(&f, 2U, false) == 0);
    char directory[UMI_PATH_CAPACITY], filename[UMI_PATH_CAPACITY];
    FixtureDirectory(directory);
    FixturePath(filename, directory, "source-\xc3\xa9.tsv");
    UmiSourceObservationReceipt receipt, sentinel;
    memset(&receipt, 0x35, sizeof(receipt)); memcpy(&sentinel, &receipt, sizeof(receipt));
    if (strcmp(mode, "relative-save") == 0 || strcmp(mode, "null-observation") == 0 ||
        strcmp(mode, "null-save-output") == 0) {
        REQUIRE(UmiReleaseSourceObservationSave(strcmp(mode, "null-observation") == 0 ? NULL : f.before,
            strcmp(mode, "relative-save") == 0 ? "source.tsv" : filename,
            strcmp(mode, "null-save-output") == 0 ? NULL : &receipt) != UMI_STATUS_OK);
        REQUIRE(memcmp(&receipt, &sentinel, sizeof(receipt)) == 0);
        QualificationDestroy(&f); return 0;
    }
    char digest[65];
    memset(digest, '0', 64U); digest[64] = '\0';
    bool malformed = strcmp(mode, "malformed") == 0, missing = strcmp(mode, "missing") == 0;
    if (malformed) {
        UmiOutputFile *file = NULL;
        REQUIRE(UmiOutputFileCreate(filename, &file) == UMI_STATUS_OK);
        REQUIRE(UmiOutputFileWrite(file, "partial", 7U) == UMI_STATUS_OK);
        REQUIRE(UmiOutputFileClose(file) == UMI_STATUS_OK);
        UmiOutputFileDestroy(file);
        REQUIRE(UmiSha256Buffer("partial", 7U, digest) == UMI_STATUS_OK);
    } else if (!missing) {
        REQUIRE(UmiReleaseSourceObservationSave(f.before, filename, &receipt) == UMI_STATUS_OK);
        memcpy(digest, receipt.archiveSha256, sizeof(digest));
    }
    UmiReleaseSourceObservation *loaded = NULL, *original = NULL;
    if (strcmp(mode, "occupied-output") == 0) { loaded = f.before; original = loaded; }
    else { UmiReleaseSourceObservationDestroy(f.before); f.before = NULL; }
    if (strcmp(mode, "wrong-receipt") == 0) digest[0] = digest[0] == '0' ? '1' : '0';
    UmiStatus status = UmiReleaseSourceObservationLoad(f.inventory,
        strcmp(mode, "relative-load") == 0 ? "source.tsv" : filename,
        strcmp(mode, "null-receipt") == 0 ? NULL : digest,
        strcmp(mode, "null-load-output") == 0 ? NULL : &loaded);
    if (strcmp(mode, "reopen") != 0 && strcmp(mode, "existing") != 0) {
        REQUIRE(status != UMI_STATUS_OK && loaded == original);
    } else {
        REQUIRE(status == UMI_STATUS_OK);
        REQUIRE(strcmp(UmiReleaseSourceObservationFingerprint(loaded), receipt.sourceSha256) == 0);
        if (strcmp(mode, "existing") == 0) {
            size_t beforeSize = 0U, afterSize = 0U;
            unsigned char *before = FixtureRead(filename, &beforeSize);
            REQUIRE(beforeSize == receipt.bytes);
            UmiReleaseSourceOptions options = {.read = QualificationRead, .context = "new bytes",
                .maximumFileBytes = UMI_RELEASE_SOURCE_FILE_LIMIT, .maximumTotalBytes = UMI_RELEASE_SOURCE_TOTAL_LIMIT};
            REQUIRE(UmiReleaseSourceObserve(f.inventory, &options, &f.before) == UMI_STATUS_OK);
            memcpy(&receipt, &sentinel, sizeof(receipt));
            REQUIRE(UmiReleaseSourceObservationSave(f.before, filename, &receipt) == UMI_STATUS_ALREADY_EXISTS);
            REQUIRE(memcmp(&receipt, &sentinel, sizeof(receipt)) == 0);
            unsigned char *after = FixtureRead(filename, &afterSize);
            REQUIRE(beforeSize == afterSize && memcmp(before, after, beforeSize) == 0);
            free(before); free(after);
        }
    }
    if (loaded != original) UmiReleaseSourceObservationDestroy(loaded);
    QualificationDestroy(&f);
    return 0;
}
