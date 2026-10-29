/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_qualification/test_source_archive.c
 *
 * PURPOSE:
 *   Verify durable source baselines retain identity and reject damaged or unrelated input.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "fixture.h"
#include "umicom/distribution/runtime/source_observation_archive.h"
#include <stdlib.h>
#include <string.h>

static bool Is(const char *a, const char *b) { return strcmp(a, b) == 0; }
/* Replace one field without changing the framing of any other row. The caller
 * separately rehashes the edited bytes to exercise parsing beyond receipt checks. */
static int ReplaceField(char *text, const char *row, size_t column, const char *value)
{
    char *begin = strstr(text, row);
    REQUIRE(begin != NULL);
    for (size_t i = 0U; i < column; ++i) {
        begin = strchr(begin, '\t'); REQUIRE(begin != NULL); ++begin;
    }
    char *end = strpbrk(begin, "\t\n"); REQUIRE(end != NULL);
    size_t n = strlen(value);
    memmove(begin + n, end, strlen(end) + 1U);
    memcpy(begin, value, n);
    return 0;
}
static int Observe(QualificationFixture *f, size_t inputs, const char *extraPath)
{
    REQUIRE(QualificationStart(f, 3U, false) == 0);
    if (inputs > 1U || extraPath != NULL) {
        for (size_t i = 1U; i < inputs; ++i) {
            char name[64];
            (void)snprintf(name, sizeof(name), "/repo/source-%06zu.c", i);
            REQUIRE(FixtureSource(&f->text, name, "library"));
        }
        if (extraPath != NULL) REQUIRE(FixtureSource(&f->text, extraPath, "library"));
        UmiReleaseQualificationDestroy(f->run); f->run = NULL;
        UmiReleaseImpactDestroy(f->plan); f->plan = NULL;
        UmiReleaseSourceObservationDestroy(f->before); f->before = NULL;
        UmiReleaseInventoryDestroy(f->inventory); f->inventory = NULL;
        REQUIRE(FixtureParse(&f->text, &f->inventory) == UMI_STATUS_OK);
        UmiReleaseSourceOptions options = {.read = QualificationRead,
            .maximumFileBytes = UMI_RELEASE_SOURCE_FILE_LIMIT, .maximumTotalBytes = UMI_RELEASE_SOURCE_TOTAL_LIMIT};
        REQUIRE(UmiReleaseSourceObserve(f->inventory, &options, &f->before) == UMI_STATUS_OK);
    }
    return 0;
}
int main(int argc, char **argv)
{
    REQUIRE(argc == 2);
    const char *mode = argv[1];
    const char *modes[] = {"roundtrip", "deterministic", "unicode", "many", "ownership",
        "version", "truncated", "trailing", "embedded-nul", "count", "count-overflow",
        "path", "path-case", "duplicate", "size", "size-overflow", "total-overflow",
        "file-digest", "fingerprint", "inventory", "receipt", "bad-receipt",
        "null-receipt", "empty", "oversized", "null-inventory", "null-bytes",
        "null-output", "occupied-output", "encode-null-source", "encode-null-output",
        "encode-null-length", "encode-null-digest", "encode-occupied", "empty-source"};
    bool known = false;
    for (size_t i = 0U; i < sizeof(modes) / sizeof(modes[0]); ++i)
        if (Is(mode, modes[i])) known = true;
    REQUIRE(known);
    QualificationFixture f;
    size_t inputs = Is(mode, "many") ? 2048U : Is(mode, "total-overflow") ? 33U : 2U;
    REQUIRE(Observe(&f, inputs, Is(mode, "unicode") ? "/repo/\xc3\xa9 space.c" : NULL) == 0);
    if (Is(mode, "empty-source")) {
        UmiReleaseSourceObservationDestroy(f.before); f.before = NULL;
        UmiReleaseSourceOptions options = {.read = QualificationRead, .context = "",
            .maximumFileBytes = UMI_RELEASE_SOURCE_FILE_LIMIT, .maximumTotalBytes = UMI_RELEASE_SOURCE_TOTAL_LIMIT};
        REQUIRE(UmiReleaseSourceObserve(f.inventory, &options, &f.before) == UMI_STATUS_OK);
    }
    char *bytes = NULL;
    size_t length = 17U;
    char digest[65]; memset(digest, 'x', sizeof(digest));
    if (strncmp(mode, "encode-", 7U) == 0) {
        char sentinel[] = "retained";
        if (Is(mode, "encode-occupied")) bytes = sentinel;
        UmiStatus status = UmiReleaseSourceObservationEncode(
            Is(mode, "encode-null-source") ? NULL : f.before,
            Is(mode, "encode-null-output") ? NULL : &bytes,
            Is(mode, "encode-null-length") ? NULL : &length,
            Is(mode, "encode-null-digest") ? NULL : digest);
        REQUIRE(status == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(bytes == (Is(mode, "encode-occupied") ? sentinel : NULL));
        REQUIRE(length == 17U);
        for (size_t i = 0U; i < sizeof(digest); ++i) REQUIRE(digest[i] == 'x');
        QualificationDestroy(&f); return 0;
    }
    REQUIRE(UmiReleaseSourceObservationEncode(f.before, &bytes, &length, digest) == UMI_STATUS_OK);
    REQUIRE(bytes[length] == '\0' && strlen(bytes) == length);
    if (Is(mode, "deterministic")) {
        char *again = NULL, secondDigest[65]; size_t secondLength = 0U;
        REQUIRE(UmiReleaseSourceObservationEncode(f.before, &again, &secondLength, secondDigest) == UMI_STATUS_OK);
        REQUIRE(secondLength == length && strcmp(digest, secondDigest) == 0 && memcmp(bytes, again, length) == 0);
        UmiReleaseSourceObservationFreeEncoded(again);
    }
    char expectedFingerprint[65];
    memcpy(expectedFingerprint, UmiReleaseSourceObservationFingerprint(f.before), sizeof(expectedFingerprint));
    char *edited = malloc(length + 4096U);
    REQUIRE(edited != NULL);
    memcpy(edited, bytes, length + 1U);
    UmiReleaseSourceObservationFreeEncoded(bytes); bytes = NULL;

    if (Is(mode, "version")) REQUIRE(ReplaceField(edited, "UMICOM-SOURCE-OBSERVATION", 1U, "2") == 0);
    if (Is(mode, "count")) REQUIRE(ReplaceField(edited, "\nfiles", 1U, "1") == 0);
    if (Is(mode, "count-overflow")) REQUIRE(ReplaceField(edited, "\nfiles", 1U, "18446744073709551616") == 0);
    if (Is(mode, "path")) REQUIRE(ReplaceField(edited, "\nfile\t", 1U, "2f756e6c69737465642e63") == 0);
    if (Is(mode, "path-case")) REQUIRE(ReplaceField(edited, "\nfile\t", 1U, "2f7265706f2f4c6962726172792e63") == 0);
    if (Is(mode, "duplicate")) {
        char *second = strstr(strstr(edited, "\nfile\t") + 1, "\nfile\t"); REQUIRE(second != NULL);
        REQUIRE(ReplaceField(second, "\nfile\t", 1U, "2f7265706f2f6c6962726172792e63") == 0);
    }
    if (Is(mode, "size")) REQUIRE(ReplaceField(edited, "\nfile\t", 2U, "7") == 0);
    if (Is(mode, "size-overflow")) REQUIRE(ReplaceField(edited, "\nfile\t", 2U, "16777217") == 0);
    if (Is(mode, "total-overflow")) {
        char *cursor = edited;
        for (size_t i = 0U; i < inputs; ++i) {
            cursor = strstr(cursor, "\nfile\t"); REQUIRE(cursor != NULL);
            REQUIRE(ReplaceField(cursor, "\nfile\t", 2U, "16777216") == 0); ++cursor;
        }
    }
    if (Is(mode, "file-digest")) REQUIRE(ReplaceField(edited, "\nfile\t", 3U, "invalid") == 0);
    if (Is(mode, "fingerprint") || Is(mode, "inventory")) {
        char other[65]; memset(other, '0', 64U); other[64] = '\0';
        REQUIRE(ReplaceField(edited, Is(mode, "fingerprint") ? "\nfingerprint" : "\ninventory", 1U, other) == 0);
    }
    if (Is(mode, "trailing")) strcat(edited, "extra\n");
    length = strlen(edited);
    if (Is(mode, "truncated")) --length;
    if (Is(mode, "embedded-nul")) edited[length / 2U] = '\0';
    if (Is(mode, "empty")) length = 0U;
    REQUIRE(UmiSha256Buffer(edited, length, digest) == UMI_STATUS_OK);
    if (Is(mode, "receipt")) digest[0] = digest[0] == '0' ? '1' : '0';
    UmiReleaseSourceObservation *reopened = NULL, *original = NULL;
    if (Is(mode, "occupied-output")) { reopened = f.before; original = reopened; }
    if (Is(mode, "ownership")) {
        UmiReleaseSourceObservationDestroy(f.before); f.before = NULL;
    }
    UmiStatus status = UmiReleaseSourceObservationDecode(
        Is(mode, "null-inventory") ? NULL : f.inventory,
        Is(mode, "null-bytes") ? NULL : edited,
        Is(mode, "oversized") ? UMI_RELEASE_SOURCE_ARCHIVE_LIMIT : length,
        Is(mode, "null-receipt") ? NULL : Is(mode, "bad-receipt") ? "bad" : digest,
        Is(mode, "null-output") ? NULL : &reopened);
    bool success = Is(mode, "roundtrip") || Is(mode, "deterministic") || Is(mode, "unicode") ||
        Is(mode, "many") || Is(mode, "ownership") || Is(mode, "empty-source");
    bool invalid = strncmp(mode, "null-", 5U) == 0 || Is(mode, "bad-receipt") || Is(mode, "occupied-output");
    UmiStatus expected = success ? UMI_STATUS_OK : invalid ? UMI_STATUS_INVALID_ARGUMENT :
        Is(mode, "receipt") || Is(mode, "inventory") ? UMI_STATUS_INVALID_STATE :
        Is(mode, "oversized") || Is(mode, "total-overflow") ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_PARSE_ERROR;
    REQUIRE(status == expected);
    if (success) {
        REQUIRE(strcmp(UmiReleaseSourceObservationFingerprint(reopened), expectedFingerprint) == 0);
        if (!Is(mode, "ownership")) {
            UmiReleaseSourceComparison comparison;
            REQUIRE(UmiReleaseSourceObservationCompare(f.before, reopened, &comparison) == UMI_STATUS_OK);
            REQUIRE(comparison.same && comparison.changedFiles == 0U);
        } else {
            free(edited); edited = NULL; QualificationDestroy(&f);
            REQUIRE(UmiReleaseSourceObservationCount(reopened) == inputs);
            REQUIRE(strcmp(UmiReleaseSourceObservationAt(reopened, 0U)->path, "/repo/library.c") == 0);
        }
    } else REQUIRE(reopened == original);
    if (reopened != original) UmiReleaseSourceObservationDestroy(reopened);
    free(edited); QualificationDestroy(&f);
    UmiReleaseSourceObservationFreeEncoded(NULL);
    return 0;
}
