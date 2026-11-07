/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_authoring.c
 *
 * PURPOSE:
 *   Verify learning revision ownership, resource rehashing, limits and complete-output publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "authoring_fixture.h"
#include <stdint.h>
#define CHECK(x) do { if (!(x)) { (void)fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); result = 1; goto cleanup; } } while (0)
static int Check(const char *name, const char *root)
{
    int result = 0;
    UmiLearningPack *source = NULL, *revised = NULL;
    unsigned char *json = NULL, *again = NULL, *received = NULL;
    size_t length = 0U, again_length = 0U, received_length = 0U;
    UmiVersion host = {0U, 9U, 0U};
    UmiLearningPackInfo before, after;
    CHECK(AuthoringFixture(root) == UMI_STATUS_OK);
    CHECK(UmiLearningPackLoad(root, "learning-pack.json", host, &source) == UMI_STATUS_OK);
    CHECK(UmiLearningPackCopyInfo(source, &before) == UMI_STATUS_OK);
    if (strcmp(name, "arguments") == 0) {
        const uint64_t bad[] = {0U, 1U, 2U, (uint64_t)INT64_MAX + 1U, UINT64_MAX};
        for (size_t i = 0U; i < sizeof(bad) / sizeof(bad[0]); ++i) {
            length = 19U;
            CHECK(UmiLearningPackPrepareRevision(source, bad[i], 1024U, &json, &length) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(json == NULL && length == 0U);
        }
        CHECK(UmiLearningPackPrepareRevision(NULL, 3U, 1024U, &json, &length) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningPackPrepareRevision(source, 3U, 0U, &json, &length) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningPackPrepareRevision(source, 3U, UMI_LEARNING_AUTHORING_RESOURCE_BYTES + 1U, &json, &length) == UMI_STATUS_INVALID_ARGUMENT);
        length = 9U;
        CHECK(UmiLearningPackPrepareRevision(source, 3U, 1024U, NULL, &length) == UMI_STATUS_INVALID_ARGUMENT && length == 0U);
        CHECK(UmiLearningPackPrepareRevision(source, 3U, 1024U, &json, NULL) == UMI_STATUS_INVALID_ARGUMENT && json == NULL);
        goto cleanup;
    }
    if (strcmp(name, "missing-late-resource") == 0) {
        CHECK(UmiRootedFileRemove(root, "image.svg") == UMI_STATUS_OK);
        length = 19U;
        CHECK(UmiLearningPackPrepareRevision(source, 3U, 1024U, &json, &length) == UMI_STATUS_NOT_FOUND);
        CHECK(json == NULL && length == 0U);
        goto unchanged;
    }
    if (strcmp(name, "bounded") == 0) {
        length = 19U;
        CHECK(UmiLearningPackPrepareRevision(source, 3U, 8U, &json, &length) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(json == NULL && length == 0U);
        CHECK(UmiLearningPackPrepareRevision(source, 3U, 9U, &json, &length) == UMI_STATUS_OK);
    } else {
        if (strcmp(name, "changed") == 0)
            CHECK(UmiRootedFileWrite(root, "lesson.md", "revised", 7U) == UMI_STATUS_OK);
        else if (strcmp(name, "binary") == 0) {
            const unsigned char binary[] = {0U, 0xffU, 7U, 0U};
            CHECK(UmiRootedFileWrite(root, "image.svg", binary, sizeof(binary)) == UMI_STATUS_OK);
        } else if (strcmp(name, "empty") == 0)
            CHECK(UmiRootedFileWrite(root, "lesson.md", "", 0U) == UMI_STATUS_OK);
        else CHECK(strcmp(name, "metadata") == 0 || strcmp(name, "maximum-revision") == 0);
        uint64_t revision = strcmp(name, "maximum-revision") == 0 ? (uint64_t)INT64_MAX : 3U;
        CHECK(UmiLearningPackPrepareRevision(source, revision, 1024U, &json, &length) == UMI_STATUS_OK);
    }
    CHECK(length > 0U && length <= UMI_LEARNING_PACK_MANIFEST_BYTES && json[length] == 0U);
    CHECK(UmiLearningPackCreate(json, length, root, host, &revised) == UMI_STATUS_OK);
    CHECK(UmiLearningPackCopyInfo(revised, &after) == UMI_STATUS_OK);
    CHECK(after.revision == (strcmp(name, "maximum-revision") == 0 ? (uint64_t)INT64_MAX : 3U));
    CHECK(strcmp(before.id, after.id) == 0 && strcmp(before.title, after.title) == 0);
    CHECK(strcmp(before.product, after.product) == 0 && strcmp(before.locale, after.locale) == 0);
    CHECK(after.minimum_version.minor == 9U && after.maximum_version.major == 1U &&
          after.maximum_version.minor == 2U && after.maximum_version.patch == 3U);
    CHECK(after.resource_count == 4U);
    for (size_t i = 0U; i < after.resource_count; ++i) {
        UmiLearningPackResource old_item, item;
        CHECK(UmiLearningPackResourceAt(source, i, &old_item) == UMI_STATUS_OK);
        CHECK(UmiLearningPackResourceAt(revised, i, &item) == UMI_STATUS_OK);
        CHECK(strcmp(old_item.id, item.id) == 0 && strcmp(old_item.path, item.path) == 0 &&
              strcmp(old_item.title, item.title) == 0 && strcmp(old_item.topic, item.topic) == 0 &&
              strcmp(old_item.example_id, item.example_id) == 0 && old_item.kind == item.kind);
        CHECK(UmiLearningPackReadResource(revised, item.id, 1024U, &received, &received_length) == UMI_STATUS_OK);
        if (strcmp(name, "changed") == 0 && i == 0U)
            CHECK(received_length == 7U && memcmp(received, "revised", 7U) == 0 && strcmp(old_item.sha256, item.sha256) != 0);
        if (strcmp(name, "empty") == 0 && i == 0U) CHECK(received_length == 0U);
        if (strcmp(name, "binary") == 0 && i == 3U)
            CHECK(received_length == 4U && received[0] == 0U && received[1] == 0xffU && received[3] == 0U);
        UmiLearningPackFreeBytes(received); received = NULL;
    }
    CHECK(UmiLearningPackPrepareRevision(source, after.revision, 1024U, &again, &again_length) == UMI_STATUS_OK);
    CHECK(length == again_length && memcmp(json, again, length) == 0);
    if (strcmp(name, "changed") == 0 || strcmp(name, "empty") == 0) {
        CHECK(UmiLearningPackReadResource(source, "lesson", 1024U, &received, &received_length) == UMI_STATUS_INVALID_STATE);
        CHECK(received == NULL && received_length == 0U);
    }
unchanged:
    CHECK(UmiLearningPackCopyInfo(source, &after) == UMI_STATUS_OK);
    CHECK(after.revision == before.revision && after.resource_count == before.resource_count);
    /* Preparing never replaces the manifest on disk. */
    UmiLearningPackDestroy(revised); revised = NULL;
    CHECK(UmiLearningPackLoad(root, "learning-pack.json", host, &revised) == UMI_STATUS_OK);
    CHECK(UmiLearningPackCopyInfo(revised, &after) == UMI_STATUS_OK && after.revision == 2U);
cleanup:
    UmiLearningPackFreeBytes(json); UmiLearningPackFreeBytes(again); UmiLearningPackFreeBytes(received);
    UmiLearningPackDestroy(source); UmiLearningPackDestroy(revised);
    return result;
}
int main(int argc, char **argv)
{
    return argc == 3 ? Check(argv[1], argv[2]) : 2;
}
