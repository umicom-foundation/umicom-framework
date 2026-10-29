/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_pack_changes.c
 *
 * PURPOSE:
 *   Verify complete resource differences, bounded snapshots and allocation rollback.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/teacher/learning_pack_changes.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static size_t allocation_count, failure_at;
static void *ChangesCalloc(size_t count, size_t size)
{
    ++allocation_count;
    return failure_at != 0U && allocation_count == failure_at ? NULL : calloc(count, size);
}
/* Exercise allocation failures in the actual implementation, without adding a
 * mutable failure switch to the public library or a production allocator API. */
#define calloc ChangesCalloc
#include "../../src/teacher/learning_pack_changes.c"
#undef calloc
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
    result = 1; goto cleanup; } } while (0)
static int Append(char *json, size_t *used, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int size = vsnprintf(json + *used, UMI_LEARNING_PACK_MANIFEST_BYTES - *used, format, args);
    va_end(args);
    if (size < 0 || (size_t)size >= UMI_LEARNING_PACK_MANIFEST_BYTES - *used) return 0;
    *used += (size_t)size;
    return 1;
}
static unsigned int RequestedFields(const char *name)
{
    if (strcmp(name, "title") == 0) return UMI_LEARNING_CHANGE_TITLE;
    if (strcmp(name, "topic") == 0) return UMI_LEARNING_CHANGE_TOPIC;
    if (strcmp(name, "kind") == 0) return UMI_LEARNING_CHANGE_KIND;
    if (strcmp(name, "path") == 0) return UMI_LEARNING_CHANGE_PATH;
    if (strcmp(name, "digest") == 0) return UMI_LEARNING_CHANGE_DIGEST;
    if (strcmp(name, "example") == 0) return UMI_LEARNING_CHANGE_EXAMPLE;
    if (strcmp(name, "mixed") == 0) return UMI_LEARNING_CHANGE_TITLE | UMI_LEARNING_CHANGE_DIGEST;
    if (strcmp(name, "all-fields") == 0) return 63U;
    return 0U;
}
static UmiStatus Fixture(const char *root, const char *name, int proposed, UmiLearningPack **out)
{
    char *json = malloc(UMI_LEARNING_PACK_MANIFEST_BYTES);
    if (json == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    char hash[UMI_SHA256_HEX_CAPACITY], different[UMI_SHA256_HEX_CAPACITY];
    UmiStatus status = UmiSha256Buffer("old", 3U, hash);
    if (status == UMI_STATUS_OK) status = UmiSha256Buffer("new", 3U, different);
    size_t used = 0U;
    int ok = status == UMI_STATUS_OK && Append(json, &used,
        "{\"schema\":\"umicom.learning.pack\",\"version\":1,\"id\":\"compare\","
        "\"title\":\"Comparison\",\"product\":\"framework\",\"locale\":\"en\",\"revision\":%u,"
        "\"compatibility\":{\"minimum\":[0,9,0],\"maximum\":[0,9,0]},\"resources\":[", proposed ? 2U : 1U);
    int maximum = strcmp(name, "maximum") == 0;
    size_t count = maximum ? UMI_LEARNING_PACK_MAX_RESOURCES :
        strcmp(name, "empty") == 0 ? 0U :
        proposed && strcmp(name, "added") == 0 ? 4U :
        proposed && strcmp(name, "removed") == 0 ? 2U : 3U;
    for (size_t i = 0U; ok && i < count; ++i) {
        size_t item = proposed && strcmp(name, "reordered") == 0 ? count - i - 1U : i;
        char id[64], filename[96];
        if (maximum) (void)snprintf(id, sizeof(id), "%s-%zu", proposed ? "new" : "old", i);
        else {
            const char *selected = item == 0U ? "guide" : item == 1U ? "source" : item == 2U ? "other" : "added";
            if (proposed && strcmp(name, "mixed") == 0 && item != 0U) selected = item == 1U ? "other" : "added";
            if (proposed && strcmp(name, "renamed") == 0 && item == 0U) selected = "renamed";
            (void)snprintf(id, sizeof(id), "%s", selected);
        }
        unsigned int fields = proposed && item == 0U ? RequestedFields(name) : 0U;
        (void)snprintf(filename, sizeof(filename), "%s%s.txt", id,
            (fields & UMI_LEARNING_CHANGE_PATH) != 0U ? "-updated" : "");
        const char *kind = item == 0U && !maximum ?
            ((fields & UMI_LEARNING_CHANGE_KIND) != 0U ? "lesson" : "guide") : "example";
        const char *example = item == 0U && !maximum &&
            (strcmp(name, "example") == 0 || strcmp(name, "all-fields") == 0) ?
            (proposed ? ",\"example\":\"other\"" : ",\"example\":\"source\"") : "";
        ok = Append(json, &used,
            "%s{\"id\":\"%s\",\"title\":\"%s\",\"topic\":\"%s\",\"kind\":\"%s\","
            "\"path\":\"%s\",\"sha256\":\"%s\"%s}", i == 0U ? "" : ",", id,
            (fields & UMI_LEARNING_CHANGE_TITLE) != 0U ? "Updated title" : "Resource title",
            (fields & UMI_LEARNING_CHANGE_TOPIC) != 0U ? "ownership" : "reading", kind, filename,
            (fields & UMI_LEARNING_CHANGE_DIGEST) != 0U ? different : hash, example);
    }
    if (ok) ok = Append(json, &used, "]}");
    status = ok ? UmiLearningPackCreate(json, used, root, (UmiVersion){0U, 9U, 0U}, out) : UMI_STATUS_CAPACITY_EXCEEDED;
    free(json);
    return status;
}
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    int result = 0;
    const char *name = argv[1];
    UmiLearningPack *previous = NULL, *proposed = NULL;
    UmiLearningPackChanges *changes = NULL;
    UmiLearningPackChangeSummary summary = {0};
    UmiLearningResourceChange change;
    const char *fixture = strcmp(name, "allocation") == 0 || strcmp(name, "independent") == 0 ? "mixed" : name;
    /* A published pack must contain at least one resource. Exercise rejection
     * at that admission boundary; identical valid packs below still exercise
     * an empty change set. Do not relax the public parser for a test fixture. */
    if (strcmp(fixture, "empty") == 0) {
        CHECK(Fixture(argv[2], fixture, 0, &previous) == UMI_STATUS_PARSE_ERROR);
        CHECK(previous == NULL);
        CHECK(Fixture(argv[2], fixture, 1, &proposed) == UMI_STATUS_PARSE_ERROR);
        CHECK(proposed == NULL && changes == NULL);
        goto cleanup;
    }
    CHECK(Fixture(argv[2], fixture, 0, &previous) == UMI_STATUS_OK);
    CHECK(Fixture(argv[2], fixture, 1, &proposed) == UMI_STATUS_OK);
    if (strcmp(name, "arguments") == 0) {
        CHECK(UmiLearningPackCompare(NULL, proposed, &changes) == UMI_STATUS_INVALID_ARGUMENT && changes == NULL);
        CHECK(UmiLearningPackCompare(previous, NULL, &changes) == UMI_STATUS_INVALID_ARGUMENT && changes == NULL);
        CHECK(UmiLearningPackCompare(previous, proposed, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningPackChangesCopySummary(NULL, &summary) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningPackChangeAt(NULL, 0U, &change) == UMI_STATUS_INVALID_ARGUMENT);
        goto cleanup;
    }
    if (strcmp(name, "allocation") == 0) {
        for (size_t i = 1U; i <= 2U; ++i) {
            allocation_count = 0U; failure_at = i;
            CHECK(UmiLearningPackCompare(previous, proposed, &changes) == UMI_STATUS_OUT_OF_MEMORY && changes == NULL);
        }
        failure_at = 0U;
    }
    CHECK(UmiLearningPackCompare(previous, proposed, &changes) == UMI_STATUS_OK);
    if (strcmp(name, "independent") == 0) {
        UmiLearningPackDestroy(previous); previous = NULL;
        UmiLearningPackDestroy(proposed); proposed = NULL;
    }
    CHECK(UmiLearningPackChangesCopySummary(changes, &summary) == UMI_STATUS_OK);
    memset(&change, 0, sizeof(change)); change.fields = 999U;
    CHECK(UmiLearningPackChangeAt(changes, summary.change_count, &change) == UMI_STATUS_NOT_FOUND && change.fields == 999U);
    CHECK(UmiLearningPackChangesCopySummary(changes, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiLearningPackChangeAt(changes, 0U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    if (strcmp(fixture, "empty") == 0 || strcmp(fixture, "identical") == 0 || strcmp(fixture, "reordered") == 0) {
        CHECK(summary.change_count == 0U && summary.unchanged == (strcmp(fixture, "empty") == 0 ? 0U : 3U));
    } else if (strcmp(fixture, "maximum") == 0) {
        CHECK(summary.removed == 256U && summary.added == 256U && summary.change_count == 512U);
        CHECK(UmiLearningPackChangeAt(changes, 255U, &change) == UMI_STATUS_OK);
        CHECK(change.kind == UMI_LEARNING_RESOURCE_REMOVED && strcmp(change.previous.id, "old-255") == 0);
        CHECK(UmiLearningPackChangeAt(changes, 511U, &change) == UMI_STATUS_OK);
        CHECK(change.kind == UMI_LEARNING_RESOURCE_ADDED && strcmp(change.proposed.id, "new-255") == 0);
    } else if (strcmp(fixture, "mixed") == 0) {
        CHECK(summary.removed == 1U && summary.added == 1U && summary.modified == 1U && summary.unchanged == 1U);
        CHECK(summary.change_count == 3U && summary.changed_digests == 1U);
        CHECK(UmiLearningPackChangeAt(changes, 0U, &change) == UMI_STATUS_OK && change.kind == UMI_LEARNING_RESOURCE_MODIFIED);
        CHECK(strcmp(change.previous.id, "guide") == 0 && change.fields == RequestedFields("mixed"));
        CHECK(UmiLearningPackChangeAt(changes, 1U, &change) == UMI_STATUS_OK && change.kind == UMI_LEARNING_RESOURCE_REMOVED);
        CHECK(strcmp(change.previous.id, "source") == 0 && change.proposed.id[0] == '\0');
        CHECK(UmiLearningPackChangeAt(changes, 2U, &change) == UMI_STATUS_OK && change.kind == UMI_LEARNING_RESOURCE_ADDED);
        CHECK(strcmp(change.proposed.id, "added") == 0 && change.previous.id[0] == '\0');
    } else if (strcmp(fixture, "added") == 0 || strcmp(fixture, "removed") == 0 || strcmp(fixture, "renamed") == 0) {
        CHECK(summary.modified == 0U && summary.changed_digests == 0U);
        CHECK(summary.added == (strcmp(fixture, "removed") == 0 ? 0U : 1U));
        CHECK(summary.removed == (strcmp(fixture, "added") == 0 ? 0U : 1U));
    } else {
        unsigned int expected = RequestedFields(fixture);
        CHECK(expected != 0U && summary.modified == 1U && summary.unchanged == 2U && summary.change_count == 1U);
        CHECK(summary.changed_digests == ((expected & UMI_LEARNING_CHANGE_DIGEST) != 0U ? 1U : 0U));
        CHECK(UmiLearningPackChangeAt(changes, 0U, &change) == UMI_STATUS_OK);
        CHECK(change.kind == UMI_LEARNING_RESOURCE_MODIFIED && change.fields == expected);
    }
cleanup:
    failure_at = 0U;
    UmiLearningPackChangesDestroy(changes);
    UmiLearningPackDestroy(previous); UmiLearningPackDestroy(proposed);
    return result;
}
