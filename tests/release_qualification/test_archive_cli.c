/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_qualification/test_archive_cli.c
 * Purpose: Exercise the CLI adapter against owned historical evidence without launching build work.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "fixture.h"
#include "../build_log/fixture.h"
#include "umicom/distribution/runtime/qualification_archive.h"
int umi_cli_command_qualification_report(int argc, char **argv);
int main(int argc, char **argv)
{
    REQUIRE(argc == 2); const char *name = argv[1];
    bool blocked = strcmp(name, "blocked") == 0;
    char directory[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
    FixtureDirectory(directory); FixturePath(path, directory, "cli-record.tsv");
    QualificationFixture f; REQUIRE(QualificationStart(&f, 1U, false) == 0);
    if (!blocked) REQUIRE(QualificationRecordAll(&f) == 0);
    REQUIRE(UmiReleaseQualificationClose(f.run, &f.identity, f.before, f.toolchain) == UMI_STATUS_OK);
    UmiQualificationArchiveReceipt receipt;
    REQUIRE(UmiQualificationArchiveSave(f.run, path, &receipt) == UMI_STATUS_OK);
    QualificationDestroy(&f);
    size_t beforeLength; unsigned char *before = FixtureRead(path, &beforeLength);
    char *arguments[] = {path, NULL, NULL, NULL, NULL}; int count = 1, expected = blocked ? 1 : 0;
    char priorPath[UMI_PATH_CAPACITY];
    if (strcmp(name, "compare-same") == 0 || strcmp(name, "compare-removed") == 0 || strcmp(name, "compare-missing") == 0) {
        FixturePath(priorPath, directory, "before.tsv");
        if (strcmp(name, "compare-missing") != 0) {
            QualificationFixture prior;
            REQUIRE(QualificationStart(&prior, strcmp(name, "compare-removed") == 0 ? 2U : 1U, false) == 0);
            REQUIRE(QualificationRecordAll(&prior) == 0);
            REQUIRE(UmiReleaseQualificationClose(prior.run, &prior.identity, prior.before, prior.toolchain) == UMI_STATUS_OK);
            UmiQualificationArchiveReceipt saved;
            REQUIRE(UmiQualificationArchiveSave(prior.run, priorPath, &saved) == UMI_STATUS_OK);
            QualificationDestroy(&prior);
        }
        arguments[count++] = "--compare"; arguments[count++] = priorPath;
        expected = strcmp(name, "compare-missing") == 0 ? 2 : strcmp(name, "compare-removed") == 0 ? 1 : 0;
    }
    if (strcmp(name, "all") == 0) { arguments[count++] = "--all"; }
    else if (strcmp(name, "receipt") == 0 || strcmp(name, "wrong-receipt") == 0) {
        arguments[count++] = "--expect-report"; arguments[count++] = receipt.reportSha256;
        if (strcmp(name, "wrong-receipt") == 0) {
            receipt.reportSha256[0] = receipt.reportSha256[0] == '0' ? '1' : '0'; expected = 2;
        }
    } else if (strcmp(name, "duplicate-option") == 0) {
        arguments[count++] = "--all"; arguments[count++] = "--all"; expected = 2;
    } else if (strcmp(name, "unknown-option") == 0) {
        arguments[count++] = "--apply"; expected = 2;
    } else if (strcmp(name, "relative") == 0) {
        arguments[0] = "cli-record.tsv"; expected = 2;
    } else REQUIRE(blocked || strcmp(name, "passed") == 0 || strncmp(name, "compare-", 8U) == 0);
    REQUIRE(umi_cli_command_qualification_report(count, arguments) == expected);
    size_t afterLength; unsigned char *after = FixtureRead(path, &afterLength);
    REQUIRE(beforeLength == afterLength && memcmp(before, after, beforeLength) == 0);
    free(before); free(after); return 0;
}
