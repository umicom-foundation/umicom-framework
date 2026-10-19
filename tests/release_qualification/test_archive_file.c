/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_qualification/test_archive_file.c
 * Purpose: Verify explicit durable evidence paths, exclusive creation and expected digests.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "fixture.h"
#include "../build_log/fixture.h"
#include "umicom/distribution/runtime/qualification_archive.h"
#include <stdlib.h>
#include <string.h>
int main(int argc, char **argv)
{
    REQUIRE(argc == 2); const char *name = argv[1];
    char directory[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
    FixtureDirectory(directory); FixturePath(path, directory, "qualification-\xc3\xa9.tsv");
    QualificationFixture f; REQUIRE(QualificationStart(&f, 2U, false) == 0);
    REQUIRE(QualificationRecordAll(&f) == 0);
    bool open = strcmp(name, "open") == 0;
    if (!open) REQUIRE(UmiReleaseQualificationClose(f.run, &f.identity, f.before, f.toolchain) == UMI_STATUS_OK);
    UmiQualificationArchiveReceipt saved, loaded, sentinel;
    memset(&saved, 0xa5, sizeof(saved)); memcpy(&sentinel, &saved, sizeof(saved));
    if (open || strcmp(name, "relative") == 0) {
        REQUIRE(UmiQualificationArchiveSave(f.run, open ? path : "qualification.tsv", &saved) != UMI_STATUS_OK);
        REQUIRE(memcmp(&saved, &sentinel, sizeof(saved)) == 0);
        QualificationDestroy(&f); return 0;
    }
    bool missing = strcmp(name, "missing") == 0, malformed = strcmp(name, "malformed") == 0;
    if (malformed) {
        UmiOutputFile *file = NULL; REQUIRE(UmiOutputFileCreate(path, &file) == UMI_STATUS_OK);
        REQUIRE(UmiOutputFileWrite(file, "partial", 7U) == UMI_STATUS_OK);
        REQUIRE(UmiOutputFileClose(file) == UMI_STATUS_OK); UmiOutputFileDestroy(file);
    } else if (!missing) REQUIRE(UmiQualificationArchiveSave(f.run, path, &saved) == UMI_STATUS_OK);
    QualificationDestroy(&f);
    UmiQualificationView *view = NULL;
    memcpy(&loaded, &sentinel, sizeof(loaded));
    char expected[65]; const char *expect = NULL;
    if (strcmp(name, "receipt") == 0 || strcmp(name, "wrong-digest") == 0) {
        memcpy(expected, saved.reportSha256, sizeof(expected)); expect = expected;
        if (strcmp(name, "wrong-digest") == 0) expected[0] = expected[0] == '0' ? '1' : '0';
    }
    UmiStatus status = UmiQualificationArchiveLoad(path, expect, &view, &loaded);
    if (missing || malformed || strcmp(name, "wrong-digest") == 0) {
        REQUIRE(status != UMI_STATUS_OK && view == NULL);
        REQUIRE(memcmp(&loaded, &sentinel, sizeof(loaded)) == 0);
    } else {
        REQUIRE(strcmp(name, "reopen") == 0 || strcmp(name, "receipt") == 0 || strcmp(name, "existing") == 0);
        REQUIRE(status == UMI_STATUS_OK && view != NULL);
        REQUIRE(saved.bytes == loaded.bytes && strcmp(saved.reportSha256, loaded.reportSha256) == 0);
        UmiQualificationViewSummary summary;
        REQUIRE(UmiQualificationViewDescribe(view, &summary) == UMI_STATUS_OK);
        REQUIRE(summary.savedReport && summary.results.readyForOwnerReview);
        if (strcmp(name, "existing") == 0) {
            size_t beforeLength; unsigned char *before = FixtureRead(path, &beforeLength);
            REQUIRE(QualificationStart(&f, 1U, false) == 0);
            REQUIRE(UmiReleaseQualificationClose(f.run, &f.identity, NULL, NULL) == UMI_STATUS_OK);
            memcpy(&saved, &sentinel, sizeof(saved));
            REQUIRE(UmiQualificationArchiveSave(f.run, path, &saved) == UMI_STATUS_ALREADY_EXISTS);
            REQUIRE(memcmp(&saved, &sentinel, sizeof(saved)) == 0);
            size_t afterLength; unsigned char *after = FixtureRead(path, &afterLength);
            REQUIRE(beforeLength == afterLength && memcmp(before, after, beforeLength) == 0);
            free(before); free(after); QualificationDestroy(&f);
        }
    }
    UmiQualificationViewDestroy(view); return 0;
}
