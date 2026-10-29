/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_reader.c
 *
 * PURPOSE:
 *   Check verified document ownership, history branching and failed navigation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/learning_reader.h"
#include "umicom/platform/rooted_files.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const unsigned char first[] = "<h1>First lesson</h1>\n";
static const unsigned char second[] = "<h1>Second lesson</h1>\n";
static const unsigned char example[] = {0U, 'C', '\n', 0xffU};
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
    result = 1; goto cleanup; } } while (0)

static UmiStatus CreatePack(const char *root, UmiLearningPack **out)
{
    char hashes[3][UMI_SHA256_HEX_CAPACITY];
    const void *bytes[] = {first, second, example};
    const size_t sizes[] = {sizeof(first) - 1U, sizeof(second) - 1U, sizeof(example)};
    const char *paths[] = {"first.html", "second.html", "example.c"};
    for (size_t i = 0U; i < 3U; ++i) {
        UmiStatus status = UmiSha256Buffer(bytes[i], sizes[i], hashes[i]);
        if (status != UMI_STATUS_OK) return status;
        status = UmiRootedFileWrite(root, paths[i], bytes[i], sizes[i]);
        if (status != UMI_STATUS_OK) return status;
    }
    char json[2048];
    int size = snprintf(json, sizeof(json),
        "{\"schema\":\"umicom.learning.pack\",\"version\":1,\"id\":\"reader.fixture\","
        "\"title\":\"Learning reader\",\"product\":\"framework\",\"locale\":\"en\",\"revision\":1,"
        "\"compatibility\":{\"minimum\":[0,9,0],\"maximum\":[0,9,0]},\"resources\":["
        "{\"id\":\"first\",\"title\":\"First\",\"topic\":\"reading\",\"kind\":\"lesson\","
        "\"path\":\"first.html\",\"sha256\":\"%s\"},"
        "{\"id\":\"second\",\"title\":\"Second\",\"topic\":\"reading\",\"kind\":\"lesson\","
        "\"path\":\"second.html\",\"sha256\":\"%s\"},"
        "{\"id\":\"example\",\"title\":\"Example\",\"topic\":\"reading\",\"kind\":\"example\","
        "\"path\":\"example.c\",\"sha256\":\"%s\"}]}", hashes[0], hashes[1], hashes[2]);
    if (size < 0 || (size_t)size >= sizeof(json)) return UMI_STATUS_CAPACITY_EXCEEDED;
    return UmiLearningPackCreate(json, (size_t)size, root, (UmiVersion){0U, 9U, 0U}, out);
}
/* Compare semantic state rather than unspecified struct padding. */
static int SameSelection(const UmiLearningReaderSnapshot *a, const UmiLearningReaderSnapshot *b)
{
    return a->revision == b->revision && a->history_index == b->history_index &&
        a->history_count == b->history_count && a->content_size == b->content_size &&
        a->has_document == b->has_document && a->can_go_back == b->can_go_back &&
        a->can_go_forward == b->can_go_forward &&
        strcmp(a->selection.resource.id, b->selection.resource.id) == 0 &&
        strcmp(a->selection.fragment, b->selection.fragment) == 0;
}

/* Clones keep previously verified bytes even when the source file changes.
 * Independent navigation still reads and verifies the original admitted pack. */
static int CloneCase(const char *name, const char *root)
{
    int result=0;
    UmiLearningPack *pack=NULL,*pack_copy=NULL;
    UmiLearningReader *reader=NULL,*clone=NULL;
    UmiLearningReaderSnapshot before,after;
    unsigned char bytes[256];
    size_t size=0U;
    CHECK(CreatePack(root,&pack)==UMI_STATUS_OK);
    if(strcmp(name,"clone-pack")==0) {
        CHECK(UmiLearningPackClone(pack,&pack_copy)==UMI_STATUS_OK);
        UmiLearningPackDestroy(pack); pack=NULL;
        UmiLearningPackResource resource;
        CHECK(UmiLearningPackFindResource(pack_copy,"second",&resource)==UMI_STATUS_OK);
        CHECK(strcmp(resource.path,"second.html")==0);
        goto cleanup;
    }
    if(strcmp(name,"clone-invalid")==0) {
        CHECK(UmiLearningPackClone(NULL,&pack_copy)==UMI_STATUS_INVALID_ARGUMENT && pack_copy==NULL);
        CHECK(UmiLearningPackClone(pack,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningReaderClone(NULL,&clone)==UMI_STATUS_INVALID_ARGUMENT && clone==NULL);
        goto cleanup;
    }
    CHECK(UmiLearningReaderCreate(&pack,strcmp(name,"clone-bound")==0?4U:4096U,&reader)==UMI_STATUS_OK);
    if(strcmp(name,"clone-reader-pack")==0 || strcmp(name,"clone-reader-pack-arguments")==0) {
        if(strcmp(name,"clone-reader-pack-arguments")==0) {
            CHECK(UmiLearningReaderCopyPack(NULL,&pack_copy)==UMI_STATUS_INVALID_ARGUMENT && pack_copy==NULL);
            CHECK(UmiLearningReaderCopyPack(reader,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        } else {
            CHECK(UmiLearningReaderOpen(reader,"first")==UMI_STATUS_OK);
            CHECK(UmiLearningReaderCopyPack(reader,&pack_copy)==UMI_STATUS_OK);
            UmiLearningReaderDestroy(reader); reader=NULL;
            UmiLearningPackResource resource;
            CHECK(UmiLearningPackFindResource(pack_copy,"second",&resource)==UMI_STATUS_OK);
            CHECK(strcmp(resource.path,"second.html")==0);
            CHECK(UmiLearningReaderCreate(&pack_copy,4096U,&clone)==UMI_STATUS_OK && pack_copy==NULL);
            CHECK(UmiLearningReaderOpen(clone,"second")==UMI_STATUS_OK);
        }
        goto cleanup;
    }
    if(strcmp(name,"clone-empty")==0 || strcmp(name,"clone-bound")==0) {
        CHECK(UmiLearningReaderClone(reader,&clone)==UMI_STATUS_OK);
        CHECK(UmiLearningReaderCopySnapshot(clone,&after)==UMI_STATUS_OK && !after.has_document);
        if(strcmp(name,"clone-bound")==0) {
            CHECK(UmiLearningReaderOpen(clone,"first")==UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(UmiLearningReaderOpen(clone,"example")==UMI_STATUS_OK);
            CHECK(UmiLearningReaderCopySnapshot(reader,&after)==UMI_STATUS_OK && !after.has_document);
        }
        goto cleanup;
    }
    CHECK(UmiLearningReaderOpen(reader,strcmp(name,"clone-binary")==0?"example":"first")==UMI_STATUS_OK);
    if(strcmp(name,"clone-history")==0 || strcmp(name,"clone-independent")==0) {
        CHECK(UmiLearningReaderFollow(reader,"second.html#details")==UMI_STATUS_OK);
        CHECK(UmiLearningReaderBack(reader)==UMI_STATUS_OK);
    }
    CHECK(UmiLearningReaderCopySnapshot(reader,&before)==UMI_STATUS_OK);
    if(strcmp(name,"clone-stale-disk")==0)
        CHECK(UmiRootedFileWrite(root,"first.html","changed",7U)==UMI_STATUS_OK);
    CHECK(UmiLearningReaderClone(reader,&clone)==UMI_STATUS_OK);
    CHECK(UmiLearningReaderCopySnapshot(clone,&after)==UMI_STATUS_OK && SameSelection(&before,&after));
    CHECK(UmiLearningReaderCopyContent(clone,bytes,sizeof(bytes),&size)==UMI_STATUS_OK);
    if(strcmp(name,"clone-binary")==0) {
        CHECK(size==sizeof(example) && memcmp(bytes,example,size)==0);
    } else {
        CHECK(size==sizeof(first)-1U && memcmp(bytes,first,size)==0);
        if(strcmp(name,"clone-stale-disk")==0) {
            CHECK(UmiLearningReaderReload(clone)==UMI_STATUS_INVALID_STATE);
            CHECK(UmiLearningReaderCopySnapshot(clone,&after)==UMI_STATUS_OK && SameSelection(&before,&after));
        } else if(strcmp(name,"clone-source-gone")==0) {
            UmiLearningReaderDestroy(reader); reader=NULL;
            CHECK(UmiLearningReaderOpen(clone,"second")==UMI_STATUS_OK);
        } else if(strcmp(name,"clone-history")==0 || strcmp(name,"clone-independent")==0) {
            CHECK(UmiLearningReaderForward(clone)==UMI_STATUS_OK);
            CHECK(UmiLearningReaderCopySnapshot(clone,&after)==UMI_STATUS_OK);
            CHECK(strcmp(after.selection.fragment,"details")==0 && after.history_index==1U);
            CHECK(UmiLearningReaderCopySnapshot(reader,&after)==UMI_STATUS_OK && SameSelection(&before,&after));
            if(strcmp(name,"clone-independent")==0) {
                CHECK(UmiLearningReaderOpen(reader,"example")==UMI_STATUS_OK);
                CHECK(UmiLearningReaderCopySnapshot(clone,&after)==UMI_STATUS_OK);
                CHECK(strcmp(after.selection.resource.id,"second")==0);
            }
        } else CHECK(0 && "Unknown clone case");
    }
cleanup:
    UmiLearningReaderDestroy(reader); UmiLearningReaderDestroy(clone);
    UmiLearningPackDestroy(pack); UmiLearningPackDestroy(pack_copy);
    return result;
}

int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    if (strncmp(argv[1], "clone-", 6U) == 0) return CloneCase(argv[1], argv[2]);
    int result = 0;
    const char *name = argv[1], *root = argv[2];
    UmiLearningPack *pack = NULL;
    UmiLearningReader *reader = NULL;
    unsigned char *copy = NULL;
    size_t size = 0U;
    UmiLearningReaderSnapshot snapshot, before;
    UmiStatus status = CreatePack(root, &pack);
    CHECK(status == UMI_STATUS_OK);
    if (strcmp(name, "invalid-create") == 0) {
        CHECK(UmiLearningReaderCreate(&pack, 0U, &reader) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(pack != NULL && reader == NULL);
        CHECK(UmiLearningReaderCreate(&pack, UMI_LEARNING_READER_MAX_BYTES + 1U, &reader) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(pack != NULL && reader == NULL);
        CHECK(UmiLearningReaderCreate(NULL, 4096U, &reader) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningReaderCreate(&pack, 4096U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(pack != NULL && reader == NULL);
        goto cleanup;
    }
    status = UmiLearningReaderCreate(&pack, strcmp(name, "bounded") == 0 ? 4U : 4096U, &reader);
    CHECK(status == UMI_STATUS_OK && pack == NULL);
    CHECK(UmiLearningReaderCopySnapshot(reader, &snapshot) == UMI_STATUS_OK);
    CHECK(!snapshot.has_document && !snapshot.can_go_back && !snapshot.can_go_forward);
    CHECK(snapshot.history_count == 0U && snapshot.revision == 0U);
    if (strcmp(name, "empty") == 0) {
        CHECK(UmiLearningReaderBack(reader) == UMI_STATUS_NOT_FOUND);
        CHECK(UmiLearningReaderForward(reader) == UMI_STATUS_NOT_FOUND);
        CHECK(UmiLearningReaderFollow(reader, "#start") == UMI_STATUS_INVALID_STATE);
        CHECK(UmiLearningReaderReload(reader) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiLearningReaderCopyContent(reader, NULL, 0U, &size) == UMI_STATUS_INVALID_STATE);
        goto cleanup;
    }
    if (strcmp(name, "bounded") == 0) {
        before = snapshot;
        CHECK(UmiLearningReaderOpen(reader, "first") == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiLearningReaderCopySnapshot(reader, &snapshot) == UMI_STATUS_OK);
        CHECK(SameSelection(&before, &snapshot));
        CHECK(UmiLearningReaderOpen(reader, "example") == UMI_STATUS_OK);
        goto cleanup;
    }
    CHECK(UmiLearningReaderOpen(reader, strcmp(name, "binary") == 0 ? "example" : "first") == UMI_STATUS_OK);
    CHECK(UmiLearningReaderCopySnapshot(reader, &snapshot) == UMI_STATUS_OK);
    CHECK(snapshot.has_document && snapshot.history_count == 1U && snapshot.revision == 1U);
    if (strcmp(name, "copy") == 0 || strcmp(name, "binary") == 0) {
        const unsigned char *expected = strcmp(name, "binary") == 0 ? example : first;
        size_t expected_size = strcmp(name, "binary") == 0 ? sizeof(example) : sizeof(first) - 1U;
        CHECK(UmiLearningReaderCopyContent(reader, NULL, 0U, &size) == UMI_STATUS_OK);
        CHECK(size == expected_size);
        copy = malloc(size + 1U);
        CHECK(copy != NULL);
        memset(copy, 0x5a, size + 1U);
        CHECK(UmiLearningReaderCopyContent(reader, copy, size, &size) == UMI_STATUS_CAPACITY_EXCEEDED);
        for (size_t i = 0U; i <= size; ++i) CHECK(copy[i] == 0x5aU);
        CHECK(UmiLearningReaderCopyContent(reader, copy, size + 1U, &size) == UMI_STATUS_OK);
        UmiLearningReaderDestroy(reader);
        reader = NULL;
        CHECK(size == expected_size && memcmp(copy, expected, size) == 0 && copy[size] == 0U);
        goto cleanup;
    }
    if (strcmp(name, "invalid-link") == 0 || strcmp(name, "missing-id") == 0 ||
        strcmp(name, "changed-reload") == 0) {
        before = snapshot;
        if (strcmp(name, "invalid-link") == 0)
            status = UmiLearningReaderFollow(reader, "../private.html");
        else if (strcmp(name, "missing-id") == 0)
            status = UmiLearningReaderOpen(reader, "absent");
        else {
            CHECK(UmiRootedFileWrite(root, "first.html", "changed", 7U) == UMI_STATUS_OK);
            status = UmiLearningReaderReload(reader);
        }
        CHECK(status == (strcmp(name, "invalid-link") == 0 ? UMI_STATUS_INVALID_ARGUMENT :
            strcmp(name, "missing-id") == 0 ? UMI_STATUS_NOT_FOUND : UMI_STATUS_INVALID_STATE));
        CHECK(UmiLearningReaderCopySnapshot(reader, &snapshot) == UMI_STATUS_OK);
        CHECK(SameSelection(&before, &snapshot));
        unsigned char retained[sizeof(first)];
        CHECK(UmiLearningReaderCopyContent(reader, retained, sizeof(retained), &size) == UMI_STATUS_OK);
        CHECK(size == sizeof(first) - 1U && memcmp(retained, first, size) == 0);
        goto cleanup;
    }
    if (strcmp(name, "history-limit") == 0) {
        for (size_t i = 0U; i < UMI_LEARNING_READER_HISTORY_CAPACITY + 2U; ++i) {
            char anchor[32];
            int n = snprintf(anchor, sizeof(anchor), "#step-%zu", i);
            CHECK(n > 0 && (size_t)n < sizeof(anchor));
            CHECK(UmiLearningReaderFollow(reader, anchor) == UMI_STATUS_OK);
        }
        CHECK(UmiLearningReaderCopySnapshot(reader, &snapshot) == UMI_STATUS_OK);
        CHECK(snapshot.history_count == UMI_LEARNING_READER_HISTORY_CAPACITY);
        CHECK(snapshot.history_index == UMI_LEARNING_READER_HISTORY_CAPACITY - 1U);
        for (size_t i = 1U; i < UMI_LEARNING_READER_HISTORY_CAPACITY; ++i)
            CHECK(UmiLearningReaderBack(reader) == UMI_STATUS_OK);
        CHECK(UmiLearningReaderBack(reader) == UMI_STATUS_NOT_FOUND);
        CHECK(UmiLearningReaderCopySnapshot(reader, &snapshot) == UMI_STATUS_OK);
        CHECK(strcmp(snapshot.selection.fragment, "step-2") == 0);
        goto cleanup;
    }
    CHECK(UmiLearningReaderFollow(reader, "second.html#code") == UMI_STATUS_OK);
    CHECK(UmiLearningReaderCopySnapshot(reader, &snapshot) == UMI_STATUS_OK);
    CHECK(strcmp(snapshot.selection.resource.id, "second") == 0 &&
        strcmp(snapshot.selection.fragment, "code") == 0 && snapshot.can_go_back);
    if (strcmp(name, "changed-back") == 0 || strcmp(name, "missing-back") == 0) {
        before = snapshot;
        if (strcmp(name, "changed-back") == 0)
            CHECK(UmiRootedFileWrite(root, "first.html", "changed", 7U) == UMI_STATUS_OK);
        else CHECK(UmiRootedFileRemove(root, "first.html") == UMI_STATUS_OK);
        CHECK(UmiLearningReaderBack(reader) ==
            (strcmp(name, "changed-back") == 0 ? UMI_STATUS_INVALID_STATE : UMI_STATUS_NOT_FOUND));
        CHECK(UmiLearningReaderCopySnapshot(reader, &snapshot) == UMI_STATUS_OK);
        CHECK(SameSelection(&before, &snapshot));
        goto cleanup;
    }
    CHECK(UmiLearningReaderBack(reader) == UMI_STATUS_OK);
    CHECK(UmiLearningReaderCopySnapshot(reader, &snapshot) == UMI_STATUS_OK);
    CHECK(snapshot.can_go_forward && !snapshot.can_go_back &&
        strcmp(snapshot.selection.resource.id, "first") == 0);
    if (strcmp(name, "branch") == 0) {
        CHECK(UmiLearningReaderOpen(reader, "example") == UMI_STATUS_OK);
        CHECK(UmiLearningReaderForward(reader) == UMI_STATUS_NOT_FOUND);
        CHECK(UmiLearningReaderCopySnapshot(reader, &snapshot) == UMI_STATUS_OK);
        CHECK(snapshot.history_count == 2U && strcmp(snapshot.selection.resource.id, "example") == 0);
        goto cleanup;
    }
    CHECK(strcmp(name, "history") == 0 || strcmp(name, "same-selection") == 0);
    if (strcmp(name, "same-selection") == 0) {
        CHECK(UmiLearningReaderOpen(reader, "first") == UMI_STATUS_OK);
        CHECK(UmiLearningReaderReload(reader) == UMI_STATUS_OK);
        CHECK(UmiLearningReaderCopySnapshot(reader, &snapshot) == UMI_STATUS_OK);
        CHECK(snapshot.history_count == 2U && snapshot.history_index == 0U && snapshot.can_go_forward);
    }
    CHECK(UmiLearningReaderForward(reader) == UMI_STATUS_OK);
    CHECK(UmiLearningReaderForward(reader) == UMI_STATUS_NOT_FOUND);
    CHECK(UmiLearningReaderCopySnapshot(reader, &snapshot) == UMI_STATUS_OK);
    CHECK(strcmp(snapshot.selection.resource.id, "second") == 0 &&
        strcmp(snapshot.selection.fragment, "code") == 0);
cleanup:
    free(copy);
    UmiLearningPackDestroy(pack);
    UmiLearningReaderDestroy(reader);
    return result;
}
