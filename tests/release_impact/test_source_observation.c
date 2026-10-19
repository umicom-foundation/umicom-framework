/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_impact/test_source_observation.c
 * Purpose: Exercise observation ownership, stale-input detection and fail-closed reads.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/distribution/runtime/source_observation.h"
#include <stdlib.h>
#include <string.h>

typedef struct Reader {
    size_t calls, cancelAfter;
    const char *changedPath;
    UmiStatus failure;
    bool malformed, oversized, empty;
    size_t diagnostics;
    UmiStatus diagnosticStatus;
    char diagnosticIdentity[128];
} Reader;
static UmiStatus Read(void *context, const char *path, size_t maximumBytes,
    size_t *outSize, char outSha256[65])
{
    Reader *reader = context;
    ++reader->calls;
    if (reader->failure != UMI_STATUS_OK) return reader->failure;
    size_t size = reader->empty ? 0U : 4U;
    if (reader->oversized) { *outSize = maximumBytes + 1U; return UmiSha256Buffer("", 0U, outSha256); }
    if (size > maximumBytes) return UMI_STATUS_CAPACITY_EXCEEDED;
    *outSize = size;
    if (reader->malformed) { memset(outSha256, 'x', 64U); outSha256[64] = '\0'; return UMI_STATUS_OK; }
    const char *bytes = reader->changedPath != NULL && strcmp(reader->changedPath, path) == 0 ? "bbbb" : "aaaa";
    return UmiSha256Buffer(bytes, size, outSha256);
}
static bool Cancelled(void *context)
{
    Reader *reader = context;
    return reader->calls >= reader->cancelAfter;
}
static void Diagnostic(void *context, UmiStatus status, const char *identity)
{
    Reader *reader = context;
    ++reader->diagnostics;
    reader->diagnosticStatus = status;
    (void)snprintf(reader->diagnosticIdentity, sizeof(reader->diagnosticIdentity), "%s", identity);
}
static UmiReleaseSourceOptions Options(Reader *reader)
{
    UmiReleaseSourceOptions options = {.maximumFileBytes = 1024U, .maximumTotalBytes = 4096U,
        .read = Read, .context = reader, .diagnostic = Diagnostic};
    return options;
}
static bool Header(Fixture *fixture, bool metadata)
{
    return FixtureRow(fixture, "header", "include/a.h", "base", "declared", metadata ?
        "impact-version=1\npath=2f7265706f2f696e636c7564652f612e68\nimpact-end\nsha256=synthetic" : "sha256=synthetic");
}
int main(int argc, char **argv)
{
    REQUIRE(argc == 2);
    const char *name = argv[1];
    Fixture fixture = {0};
    UmiReleaseInventory *inventory = NULL;
    UmiReleaseSourceObservation *before = NULL, *after = NULL;
    Reader reader = {0};
    UmiReleaseSourceOptions options = Options(&reader);
    REQUIRE(FixtureStart(&fixture));
    REQUIRE(FixtureTarget(&fixture, "base", NULL));
    const char *source = strcmp(name, "invalid-path") == 0 ? "/repo/../a.c" :
        strcmp(name, "relative-path") == 0 ? "src/a.c" :
        strcmp(name, "drive-relative") == 0 ? "C:a.c" : "/repo/a.c";
    if (strcmp(name, "empty-inventory") != 0)
        REQUIRE(FixtureRow(&fixture, "source", source, "base",
            strcmp(name, "unresolved") == 0 ? "unresolved" : "present", "sha256=synthetic"));
    if (strcmp(name, "header") == 0 || strcmp(name, "header-metadata") == 0)
        REQUIRE(Header(&fixture, strcmp(name, "header-metadata") != 0));
    if (strcmp(name, "duplicate") == 0) {
        REQUIRE(FixtureTarget(&fixture, "other", NULL));
        REQUIRE(FixtureSource(&fixture, "/repo/a.c", "other"));
    }
    if (strcmp(name, "total-limit") == 0 || strcmp(name, "cancel-between") == 0)
        REQUIRE(FixtureSource(&fixture, "/repo/z.c", "base"));
    if (strcmp(name, "separator") == 0)
        REQUIRE(FixtureSource(&fixture, "\\repo\\a.c", "base"));
    if (strcmp(name, "scale") == 0) {
        for (size_t i = 1U; i < 4096U; ++i) {
            char nameBuffer[64];
            (void)snprintf(nameBuffer, sizeof(nameBuffer), "/repo/scale-%04zu.c", i);
            REQUIRE(FixtureSource(&fixture, nameBuffer, "base"));
        }
        options.maximumTotalBytes = 32768U;
    }
    UmiStatus expected = UMI_STATUS_OK;
    bool beforeReadFailure = false;
    if (strcmp(name, "missing") == 0) expected = reader.failure = UMI_STATUS_NOT_FOUND;
    else if (strcmp(name, "busy") == 0) expected = reader.failure = UMI_STATUS_BUSY;
    else if (strcmp(name, "io-error") == 0) expected = reader.failure = UMI_STATUS_IO_ERROR;
    else if (strcmp(name, "malformed") == 0) { reader.malformed = true; expected = UMI_STATUS_PARSE_ERROR; }
    else if (strcmp(name, "oversized") == 0) { reader.oversized = true; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (strcmp(name, "file-limit") == 0) { options.maximumFileBytes = 3U; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (strcmp(name, "total-limit") == 0) { options.maximumTotalBytes = 7U; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (strcmp(name, "invalid-options") == 0) {
        options.maximumFileBytes = 0U; expected = UMI_STATUS_INVALID_ARGUMENT; beforeReadFailure = true;
    } else if (strcmp(name, "cancel-before") == 0 || strcmp(name, "cancel-between") == 0 ||
        strcmp(name, "cancel-after") == 0) {
        reader.cancelAfter = strcmp(name, "cancel-before") == 0 ? 0U : 1U;
        options.cancelled = Cancelled;
        expected = UMI_STATUS_CANCELLED;
        beforeReadFailure = reader.cancelAfter == 0U;
    } else if (strcmp(name, "unresolved") == 0 || strcmp(name, "header-metadata") == 0 ||
        strcmp(name, "empty-inventory") == 0) {
        expected = UMI_STATUS_UNAVAILABLE; beforeReadFailure = true;
    } else if (strcmp(name, "invalid-path") == 0 || strcmp(name, "relative-path") == 0 ||
        strcmp(name, "drive-relative") == 0) {
        expected = UMI_STATUS_INVALID_ARGUMENT; beforeReadFailure = true;
    } else if (strcmp(name, "empty-file") == 0) reader.empty = true;
    else REQUIRE(strcmp(name, "same") == 0 || strcmp(name, "changed") == 0 || strcmp(name, "header") == 0 ||
        strcmp(name, "duplicate") == 0 || strcmp(name, "separator") == 0 || strcmp(name, "context") == 0 ||
        strcmp(name, "ownership") == 0 || strcmp(name, "invalid-output") == 0 || strcmp(name, "scale") == 0);
    REQUIRE(FixtureParse(&fixture, &inventory) == UMI_STATUS_OK);
    REQUIRE(UmiReleaseSourceObserve(inventory, &options, &before) == expected);
    if (expected != UMI_STATUS_OK) {
        REQUIRE(before == NULL);
        bool inputProblem = expected != UMI_STATUS_CANCELLED && strcmp(name, "invalid-options") != 0 &&
            strcmp(name, "empty-inventory") != 0;
        REQUIRE(reader.diagnostics == (inputProblem ? 1U : 0U));
        if (inputProblem) {
            REQUIRE(reader.diagnosticStatus == expected);
            REQUIRE(reader.diagnosticIdentity[0] != '\0');
        }
        if (beforeReadFailure) REQUIRE(reader.calls == 0U);
        if (strcmp(name, "cancel-between") == 0 || strcmp(name, "cancel-after") == 0) REQUIRE(reader.calls == 1U);
    } else {
        size_t files = strcmp(name, "scale") == 0 ? 4096U : strcmp(name, "header") == 0 ? 2U : 1U;
        REQUIRE(UmiReleaseSourceObservationCount(before) == files);
        REQUIRE(reader.calls == files && reader.diagnostics == 0U);
        REQUIRE(UmiReleaseSourceObservationAt(before, files) == NULL);
        REQUIRE(strlen(UmiReleaseSourceObservationFingerprint(before)) == 64U);
        if (strcmp(name, "empty-file") == 0) {
            REQUIRE(UmiReleaseSourceObservationAt(before, 0U)->bytes == 0U);
            REQUIRE(strcmp(UmiReleaseSourceObservationAt(before, 0U)->sha256,
                "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855") == 0);
        }
        if (strcmp(name, "invalid-output") == 0) {
            UmiReleaseSourceObservation *sentinel = before;
            REQUIRE(UmiReleaseSourceObserve(inventory, &options, &sentinel) == UMI_STATUS_INVALID_ARGUMENT);
            REQUIRE(sentinel == before && reader.calls == files);
        }
        if (strcmp(name, "changed") == 0) reader.changedPath = "/repo/a.c";
        if (strcmp(name, "header") == 0) reader.changedPath = "/repo/include/a.h";
        if (strcmp(name, "context") == 0) {
            UmiReleaseInventoryDestroy(inventory); inventory = NULL;
            char *generation = strstr(fixture.text, "0123456789abcdef0123456789abcdef");
            REQUIRE(generation != NULL); generation[0] = 'a';
            REQUIRE(FixtureParse(&fixture, &inventory) == UMI_STATUS_OK);
        }
        REQUIRE(UmiReleaseSourceObserve(inventory, &options, &after) == UMI_STATUS_OK);
        UmiReleaseSourceComparison comparison = {99U, 98U, true};
        if (strcmp(name, "context") == 0) {
            REQUIRE(UmiReleaseSourceObservationCompare(before, after, &comparison) == UMI_STATUS_INVALID_STATE);
            REQUIRE(comparison.files == 99U && comparison.changedFiles == 98U && comparison.same);
            REQUIRE(strcmp(UmiReleaseSourceObservationFingerprint(before), UmiReleaseSourceObservationFingerprint(after)) != 0);
        } else {
            bool changed = reader.changedPath != NULL;
            REQUIRE(UmiReleaseSourceObservationCompare(before, after, &comparison) == UMI_STATUS_OK);
            REQUIRE(comparison.files == files && comparison.changedFiles == (changed ? 1U : 0U));
            REQUIRE(comparison.same == !changed);
            REQUIRE((strcmp(UmiReleaseSourceObservationFingerprint(before), UmiReleaseSourceObservationFingerprint(after)) == 0) == !changed);
        }
        if (strcmp(name, "ownership") == 0) {
            UmiReleaseInventoryDestroy(inventory); inventory = NULL;
            FixtureDestroy(&fixture);
            REQUIRE(strcmp(UmiReleaseSourceObservationAt(before, 0U)->path, "/repo/a.c") == 0);
            REQUIRE(UmiReleaseSourceObservationCompare(before, after, &comparison) == UMI_STATUS_OK && comparison.same);
        }
    }
    REQUIRE(UmiReleaseSourceObservationCount(NULL) == 0U);
    REQUIRE(UmiReleaseSourceObservationAt(NULL, 0U) == NULL);
    REQUIRE(UmiReleaseSourceObservationFingerprint(NULL) == NULL);
    UmiReleaseSourceObservationDestroy(before);
    UmiReleaseSourceObservationDestroy(after);
    UmiReleaseInventoryDestroy(inventory);
    FixtureDestroy(&fixture);
    return 0;
}
