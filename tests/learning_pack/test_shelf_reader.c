/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_shelf_reader.c
 *
 * PURPOSE:
 *   Verify independent catalogue readers and refusal of outdated selections.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/learning_reader.h"
#include "umicom/teacher/learning_search.h"
#include "umicom/platform/rooted_files.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

static const unsigned char lesson[] = "<h1>Read a catalogue selection</h1>\n";
static const unsigned char next_lesson[] = "<h1>Continue reading</h1>\n";
static const unsigned char binary[] = {0U, 'C', 0xffU, '\n'};
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
    result = 1; goto cleanup; } } while (0)

static UmiStatus MakePack(const char *root, uint64_t revision, const void *first,
    size_t first_size, UmiLearningPack **out)
{
    char hash[UMI_SHA256_HEX_CAPACITY], second_hash[UMI_SHA256_HEX_CAPACITY];
    UmiStatus status = UmiSha256Buffer(first, first_size, hash);
    if (status == UMI_STATUS_OK)
        status = UmiSha256Buffer(next_lesson, sizeof(next_lesson) - 1U, second_hash);
    if (status == UMI_STATUS_OK) status = UmiRootedFileWrite(root, "first.html", first, first_size);
    if (status == UMI_STATUS_OK)
        status = UmiRootedFileWrite(root, "next.html", next_lesson, sizeof(next_lesson) - 1U);
    if (status != UMI_STATUS_OK) return status;
    char json[2048];
    int n = snprintf(json, sizeof(json),
        "{\"schema\":\"umicom.learning.pack\",\"version\":1,\"id\":\"selection.fixture\","
        "\"title\":\"Learning selections\",\"product\":\"framework\",\"locale\":\"en\","
        "\"revision\":%" PRIu64 ",\"compatibility\":{\"minimum\":[0,9,0],\"maximum\":[0,9,0]},"
        "\"resources\":[{\"id\":\"first\",\"title\":\"First lesson\",\"topic\":\"reading\","
        "\"kind\":\"lesson\",\"path\":\"first.html\",\"sha256\":\"%s\"},"
        "{\"id\":\"next\",\"title\":\"Next lesson\",\"topic\":\"reading\","
        "\"kind\":\"lesson\",\"path\":\"next.html\",\"sha256\":\"%s\"}]}",
        revision, hash, second_hash);
    if (n < 0 || (size_t)n >= sizeof(json)) return UMI_STATUS_CAPACITY_EXCEEDED;
    return UmiLearningPackCreate(json, (size_t)n, root, (UmiVersion){0U, 9U, 0U}, out);
}

int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    const char *name = argv[1], *root = argv[2];
    int result = 0;
    UmiLearningPack *pack = NULL, *copy = NULL, *kept = NULL;
    UmiLearningShelf *shelf = NULL;
    UmiLearningReader *reader = NULL, *second = NULL;
    UmiLearningSearch *search = NULL;
    UmiLearningPackResource selected, resource;
    UmiLearningPackInfo info;
    UmiLearningReaderSnapshot state, other;
    const unsigned char *first = strcmp(name, "binary") == 0 ? binary : lesson;
    size_t first_size = strcmp(name, "binary") == 0 ? sizeof(binary) :
        strcmp(name, "empty") == 0 ? 0U : sizeof(lesson) - 1U;
    CHECK(MakePack(root, 1U, first, first_size, &pack) == UMI_STATUS_OK);
    CHECK(UmiLearningPackFindResource(pack, "first", &selected) == UMI_STATUS_OK);
    CHECK(UmiLearningShelfCreate(&shelf) == UMI_STATUS_OK);
    CHECK(UmiLearningShelfAdopt(shelf, &pack) == UMI_STATUS_OK && pack == NULL);

    if (strcmp(name, "clone") == 0 || strcmp(name, "clone-lifetime") == 0 ||
        strcmp(name, "clone-arguments") == 0) {
        CHECK(UmiLearningShelfCopyPack(shelf, "selection.fixture", &copy) == UMI_STATUS_OK);
        CHECK(copy != NULL && UmiLearningShelfCount(shelf) == 1U);
        if (strcmp(name, "clone-arguments") == 0) {
            kept = copy;
            CHECK(UmiLearningShelfCopyPack(NULL, "selection.fixture", &copy) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(copy == NULL);
            copy = kept;
            CHECK(UmiLearningShelfCopyPack(shelf, NULL, &copy) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(copy == NULL);
            copy = kept;
            CHECK(UmiLearningShelfCopyPack(shelf, "absent", &copy) == UMI_STATUS_NOT_FOUND);
            CHECK(copy == NULL);
            CHECK(UmiLearningShelfCopyPack(shelf, "selection.fixture", NULL) == UMI_STATUS_INVALID_ARGUMENT);
            goto cleanup;
        }
        if (strcmp(name, "clone-lifetime") == 0) {
            CHECK(UmiLearningShelfRemove(shelf, "selection.fixture") == UMI_STATUS_OK);
            UmiLearningShelfDestroy(shelf);
            shelf = NULL;
        }
        CHECK(UmiLearningPackCopyInfo(copy, &info) == UMI_STATUS_OK);
        CHECK(info.revision == 1U && info.resource_count == 2U);
        CHECK(UmiLearningPackFindResource(copy, "first", &resource) == UMI_STATUS_OK);
        CHECK(strcmp(resource.sha256, selected.sha256) == 0);
        CHECK(UmiLearningReaderCreate(&copy, 4096U, &reader) == UMI_STATUS_OK && copy == NULL);
        CHECK(UmiLearningReaderOpen(reader, "first") == UMI_STATUS_OK);
        goto verify_content;
    }
    if (strcmp(name, "changed-revision") == 0 || strcmp(name, "changed-digest") == 0) {
        CHECK(UmiLearningShelfRemove(shelf, "selection.fixture") == UMI_STATUS_OK);
        CHECK(MakePack(root, strcmp(name, "changed-revision") == 0 ? 2U : 1U,
            strcmp(name, "changed-digest") == 0 ? next_lesson : first,
            strcmp(name, "changed-digest") == 0 ? sizeof(next_lesson) - 1U : first_size,
            &pack) == UMI_STATUS_OK);
        CHECK(UmiLearningShelfAdopt(shelf, &pack) == UMI_STATUS_OK);
        CHECK(UmiLearningReaderCreateFromShelf(shelf, "selection.fixture", 1U,
            "first", selected.sha256, 4096U, &reader) == UMI_STATUS_INVALID_STATE);
        CHECK(reader == NULL && UmiLearningShelfCount(shelf) == 1U);
        goto cleanup;
    }
    if (strcmp(name, "removed") == 0 || strcmp(name, "missing-resource") == 0 ||
        strcmp(name, "changed-content") == 0 || strcmp(name, "missing-file") == 0 ||
        strcmp(name, "bounded") == 0) {
        UmiStatus expected = UMI_STATUS_NOT_FOUND;
        if (strcmp(name, "removed") == 0)
            CHECK(UmiLearningShelfRemove(shelf, "selection.fixture") == UMI_STATUS_OK);
        if (strcmp(name, "changed-content") == 0) {
            CHECK(UmiRootedFileWrite(root, "first.html", "changed", 7U) == UMI_STATUS_OK);
            expected = UMI_STATUS_INVALID_STATE;
        }
        if (strcmp(name, "missing-file") == 0)
            CHECK(UmiRootedFileRemove(root, "first.html") == UMI_STATUS_OK);
        if (strcmp(name, "bounded") == 0) expected = UMI_STATUS_CAPACITY_EXCEEDED;
        CHECK(UmiLearningReaderCreateFromShelf(shelf, "selection.fixture", 1U,
            strcmp(name, "missing-resource") == 0 ? "absent" : "first", selected.sha256,
            strcmp(name, "bounded") == 0 ? 1U : 4096U, &reader) == expected);
        CHECK(reader == NULL);
        goto cleanup;
    }
    if (strcmp(name, "short-hash") == 0 || strcmp(name, "long-hash") == 0 ||
        strcmp(name, "upper-hash") == 0 || strcmp(name, "invalid-hash") == 0) {
        char hash[UMI_SHA256_HEX_CAPACITY + 1U];
        memcpy(hash, selected.sha256, UMI_SHA256_HEX_CAPACITY);
        if (strcmp(name, "short-hash") == 0) hash[63] = '\0';
        if (strcmp(name, "long-hash") == 0) { hash[64] = '0'; hash[65] = '\0'; }
        if (strcmp(name, "upper-hash") == 0) hash[0] = 'A';
        if (strcmp(name, "invalid-hash") == 0) hash[0] = 'g';
        CHECK(UmiLearningReaderCreateFromShelf(shelf, "selection.fixture", 1U,
            "first", hash, 4096U, &reader) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(reader == NULL);
        goto cleanup;
    }
    if (strcmp(name, "arguments") == 0) {
        CHECK(UmiLearningReaderCreateFromShelf(shelf, "selection.fixture", 1U,
            "first", selected.sha256, 4096U, &second) == UMI_STATUS_OK);
        reader = second;
        CHECK(UmiLearningReaderCreateFromShelf(NULL, "selection.fixture", 1U,
            "first", selected.sha256, 4096U, &reader) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(reader == NULL);
        CHECK(UmiLearningReaderCreateFromShelf(shelf, NULL, 1U,
            "first", selected.sha256, 4096U, &reader) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningReaderCreateFromShelf(shelf, "selection.fixture", 0U,
            "first", selected.sha256, 4096U, &reader) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningReaderCreateFromShelf(shelf, "selection.fixture", 1U,
            NULL, selected.sha256, 4096U, &reader) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningReaderCreateFromShelf(shelf, "selection.fixture", 1U,
            "first", NULL, 4096U, &reader) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningReaderCreateFromShelf(shelf, "selection.fixture", 1U,
            "first", selected.sha256, 0U, &reader) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningReaderCreateFromShelf(shelf, "selection.fixture", 1U,
            "first", selected.sha256, UMI_LEARNING_READER_MAX_BYTES + 1U, &reader) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningReaderCreateFromShelf(shelf, "selection.fixture", 1U,
            "first", selected.sha256, 4096U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(reader == NULL && UmiLearningShelfCount(shelf) == 1U);
        goto cleanup;
    }
    if (strncmp(name, "reader-selection", strlen("reader-selection")) == 0) {
        CHECK(UmiLearningReaderCreateFromShelf(shelf, "selection.fixture", 1U,
            "first", selected.sha256, 4096U, &reader) == UMI_STATUS_OK);
        CHECK(UmiLearningShelfFindResource(shelf, "selection.fixture", "next", &resource) == UMI_STATUS_OK);
        UmiStatus expected = UMI_STATUS_OK;
        const char *pack_id = "selection.fixture", *resource_id = "next", *digest = resource.sha256;
        uint64_t revision = 1U;
        if (strcmp(name, "reader-selection-pack") == 0) {
            pack_id = "another.pack"; expected = UMI_STATUS_INVALID_STATE;
        } else if (strcmp(name, "reader-selection-revision") == 0) {
            revision = 2U; expected = UMI_STATUS_INVALID_STATE;
        } else if (strcmp(name, "reader-selection-digest") == 0) {
            digest = selected.sha256; expected = UMI_STATUS_INVALID_STATE;
        } else if (strcmp(name, "reader-selection-missing") == 0) {
            resource_id = "absent"; expected = UMI_STATUS_NOT_FOUND;
        } else if (strcmp(name, "reader-selection-bytes") == 0) {
            CHECK(UmiRootedFileWrite(root, "next.html", "changed", 7U) == UMI_STATUS_OK);
            expected = UMI_STATUS_INVALID_STATE;
        } else if (strcmp(name, "reader-selection-arguments") == 0) {
            CHECK(UmiLearningReaderOpenSelection(NULL, pack_id, revision, resource_id, digest) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiLearningReaderOpenSelection(reader, NULL, revision, resource_id, digest) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiLearningReaderOpenSelection(reader, pack_id, 0U, resource_id, digest) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiLearningReaderOpenSelection(reader, pack_id, revision, NULL, digest) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiLearningReaderOpenSelection(reader, pack_id, revision, resource_id, NULL) == UMI_STATUS_INVALID_ARGUMENT);
            goto verify_content;
        } else CHECK(strcmp(name, "reader-selection") == 0);
        CHECK(UmiLearningReaderOpenSelection(reader, pack_id, revision, resource_id, digest) == expected);
        CHECK(UmiLearningReaderCopySnapshot(reader, &state) == UMI_STATUS_OK);
        if (expected == UMI_STATUS_OK) {
            CHECK(state.history_count == 2U && state.can_go_back &&
                strcmp(state.selection.resource.id, "next") == 0);
            CHECK(UmiLearningReaderBack(reader) == UMI_STATUS_OK);
            CHECK(UmiLearningReaderForward(reader) == UMI_STATUS_OK);
        } else {
            CHECK(state.history_count == 1U && state.revision == 1U &&
                strcmp(state.selection.resource.id, "first") == 0);
            unsigned char content[sizeof(lesson)];
            size_t length = 0U;
            CHECK(UmiLearningReaderCopyContent(reader, content, sizeof(content), &length) == UMI_STATUS_OK);
            CHECK(length == sizeof(lesson) - 1U && memcmp(content, lesson, length) == 0);
        }
        goto cleanup;
    }
    if (strcmp(name, "search-open") == 0) {
        UmiLearningSearchQuery query = {0};
        memcpy(query.text, "First", sizeof("First"));
        CHECK(UmiLearningSearchCreate(shelf, &query, 8U, &search) == UMI_STATUS_OK);
        UmiLearningSearchHit hit;
        CHECK(UmiLearningSearchResultAt(search, 0U, &hit) == UMI_STATUS_OK);
        UmiLearningSearchDestroy(search);
        search = NULL;
        CHECK(UmiLearningReaderCreateFromShelf(shelf, hit.pack.id, hit.pack.revision,
            hit.resource.id, hit.resource.sha256, 4096U, &reader) == UMI_STATUS_OK);
    } else {
        CHECK(strcmp(name, "open") == 0 || strcmp(name, "reader-lifetime") == 0 ||
            strcmp(name, "two-readers") == 0 || strcmp(name, "binary") == 0 || strcmp(name, "empty") == 0);
        CHECK(UmiLearningReaderCreateFromShelf(shelf, "selection.fixture", 1U,
            "first", selected.sha256, 4096U, &reader) == UMI_STATUS_OK);
    }
    CHECK(UmiLearningShelfCount(shelf) == 1U);
    if (strcmp(name, "two-readers") == 0) {
        CHECK(UmiLearningReaderCreateFromShelf(shelf, "selection.fixture", 1U,
            "first", selected.sha256, 4096U, &second) == UMI_STATUS_OK);
        CHECK(UmiLearningReaderFollow(second, "next.html#example") == UMI_STATUS_OK);
        CHECK(UmiLearningReaderCopySnapshot(second, &other) == UMI_STATUS_OK);
        CHECK(other.history_count == 2U && strcmp(other.selection.resource.id, "next") == 0);
    }
    if (strcmp(name, "reader-lifetime") == 0 || strcmp(name, "two-readers") == 0) {
        UmiLearningShelfDestroy(shelf);
        shelf = NULL;
    }
verify_content:
    CHECK(UmiLearningReaderCopySnapshot(reader, &state) == UMI_STATUS_OK);
    CHECK(state.has_document && state.history_count == 1U && state.history_index == 0U);
    CHECK(strcmp(state.selection.resource.id, "first") == 0 && state.pack.revision == 1U);
    {
        unsigned char bytes[sizeof(lesson) + 1U];
        size_t count = 0U;
        CHECK(UmiLearningReaderCopyContent(reader, bytes, sizeof(bytes), &count) == UMI_STATUS_OK);
        CHECK(count == first_size && memcmp(bytes, first, count) == 0 && bytes[count] == 0U);
    }
    CHECK(UmiLearningReaderFollow(reader, "next.html") == UMI_STATUS_OK);
    CHECK(UmiLearningReaderBack(reader) == UMI_STATUS_OK);
cleanup:
    /* Negative output tests temporarily alias owned sentinels; a failed
     * assertion must still release each allocation only once. */
    if (reader == second) reader = NULL;
    if (copy == kept) copy = NULL;
    UmiLearningSearchDestroy(search);
    UmiLearningReaderDestroy(reader);
    UmiLearningReaderDestroy(second);
    UmiLearningPackDestroy(pack);
    UmiLearningPackDestroy(copy);
    UmiLearningPackDestroy(kept);
    UmiLearningShelfDestroy(shelf);
    return result;
}
