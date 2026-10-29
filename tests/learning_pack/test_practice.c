/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_practice.c
 *
 * PURPOSE:
 *   Verify example selection, source integrity and preservation of practice files.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../build_log/fixture.h"
#include "umicom/teacher/learning_practice.h"
#include "umicom/platform/filesystem.h"

static UmiStatus MakePack(const char *root, const void *bytes, size_t size, UmiLearningPack **out)
{
    char hash[UMI_SHA256_HEX_CAPACITY];
    UmiStatus status = UmiSha256Buffer(bytes, size, hash);
    if (status != UMI_STATUS_OK) return status;
    status = UmiRootedFileWrite(root, "source.c", bytes, size);
    if (status != UMI_STATUS_OK) return status;
    char json[3000];
    int length = snprintf(json, sizeof(json),
        "{\"schema\":\"umicom.learning.pack\",\"version\":1,\"id\":\"practice.pack\","
        "\"title\":\"Practice\",\"product\":\"framework\",\"locale\":\"en\",\"revision\":1,"
        "\"compatibility\":{\"minimum\":[0,9,0],\"maximum\":[0,9,0]},\"resources\":["
        "{\"id\":\"source\",\"title\":\"Example\",\"topic\":\"ownership\",\"kind\":\"example\","
        "\"path\":\"source.c\",\"sha256\":\"%s\"},"
        "{\"id\":\"lesson\",\"title\":\"Lesson\",\"topic\":\"ownership\",\"kind\":\"lesson\","
        "\"path\":\"lesson.html\",\"sha256\":\"%s\",\"example\":\"source\"},"
        "{\"id\":\"plain\",\"title\":\"Reference\",\"topic\":\"ownership\",\"kind\":\"guide\","
        "\"path\":\"reference.html\",\"sha256\":\"%s\"}]}", hash, hash, hash);
    if (length < 0 || (size_t)length >= sizeof(json)) return UMI_STATUS_CAPACITY_EXCEEDED;
    return UmiLearningPackCreate(json, (size_t)length, root, (UmiVersion){0U,9U,0U}, out);
}
static void CheckBytes(const char *root, const char *name, const void *expected, size_t count)
{
    char file[UMI_PATH_CAPACITY];
    FixturePath(file, root, name);
    size_t size = 0U;
    unsigned char *bytes = FixtureRead(file, &size);
    CHECK(size == count && (size == 0U || memcmp(bytes, expected, size) == 0));
    free(bytes);
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *mode = argv[1];
    char root[UMI_PATH_CAPACITY], destination[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    FixturePath(destination, root, "practice");
    CHECK(umi_fs_make_directories(destination) == UMI_STATUS_OK);
    const unsigned char source[] = {0U, 'C', '\r', '\n', 0xffU, 0U};
    size_t size = strcmp(mode, "empty") == 0 ? 0U : sizeof(source);
    UmiLearningPack *pack = NULL;
    CHECK(MakePack(root, source, size, &pack) == UMI_STATUS_OK);
    UmiLearningPracticeResult report;
    UmiLearningPackResource resource, before;
    memset(&resource, 0x5a, sizeof(resource));
    before = resource;
    if (strcmp(mode, "resolve") == 0) {
        CHECK(UmiLearningPracticeResolveExample(pack, "lesson", &resource) == UMI_STATUS_OK);
        CHECK(strcmp(resource.id, "source") == 0 && resource.kind == UMI_LEARNING_RESOURCE_EXAMPLE);
        CHECK(UmiLearningPracticeResolveExample(pack, "source", &resource) == UMI_STATUS_OK);
        resource = before;
        CHECK(UmiLearningPracticeResolveExample(pack, "plain", &resource) == UMI_STATUS_NOT_FOUND);
        CHECK(memcmp(&resource, &before, sizeof(resource)) == 0);
        CHECK(UmiLearningPracticeResolveExample(pack, "missing", &resource) == UMI_STATUS_NOT_FOUND);
        CHECK(memcmp(&resource, &before, sizeof(resource)) == 0);
        CHECK(UmiLearningPracticeResolveExample(NULL, "source", &resource) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningPracticeResolveExample(pack, NULL, &resource) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningPracticeResolveExample(pack, "source", NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(mode, "copy") == 0 || strcmp(mode, "lesson") == 0 || strcmp(mode, "empty") == 0) {
        CHECK(UmiLearningPracticeCopy(pack, strcmp(mode, "lesson") == 0 ? "lesson" : "source",
            destination, "my-example.c", 4096U, &report) == UMI_STATUS_OK);
        CHECK(report.status == UMI_STATUS_OK && report.source_verified && report.destination_attempted);
        CHECK(report.destination.created && report.destination.complete && report.source_size == size);
        CHECK(report.destination.bytes_written == size && strcmp(report.example.id, "source") == 0);
        CheckBytes(destination, "my-example.c", source, size);
        CheckBytes(root, "source.c", source, size);
        /* Editing the copied file must not modify the learning pack. */
        CHECK(UmiRootedFileWrite(destination, "my-example.c", "edited", 6U) == UMI_STATUS_OK);
        CheckBytes(root, "source.c", source, size);
    } else if (strcmp(mode, "existing") == 0 || strcmp(mode, "same-source") == 0) {
        const char *target_root = strcmp(mode, "same-source") == 0 ? root : destination;
        const char *target_name = strcmp(mode, "same-source") == 0 ? "source.c" : "keep.c";
        if (strcmp(mode, "existing") == 0)
            CHECK(UmiRootedFileWrite(destination, target_name, "keep", 4U) == UMI_STATUS_OK);
        CHECK(UmiLearningPracticeCopy(pack, "source", target_root, target_name, 4096U, &report) == UMI_STATUS_ALREADY_EXISTS);
        CHECK(report.source_verified && report.destination_attempted && !report.destination.created && !report.destination.complete);
        CHECK(report.destination.bytes_written == 0U);
        if (strcmp(mode, "existing") == 0) CheckBytes(destination, "keep.c", "keep", 4U);
        CheckBytes(root, "source.c", source, size);
    } else if (strcmp(mode, "hash-mismatch") == 0 || strcmp(mode, "missing-source") == 0 ||
               strcmp(mode, "limit") == 0 || strcmp(mode, "missing-id") == 0 || strcmp(mode, "no-example") == 0) {
        if (strcmp(mode, "hash-mismatch") == 0)
            CHECK(UmiRootedFileWrite(root, "source.c", "changed", 7U) == UMI_STATUS_OK);
        if (strcmp(mode, "missing-source") == 0)
            CHECK(UmiRootedFileRemove(root, "source.c") == UMI_STATUS_OK);
        const char *selection = strcmp(mode, "missing-id") == 0 ? "missing" :
            strcmp(mode, "no-example") == 0 ? "plain" : "source";
        UmiStatus expected = strcmp(mode, "hash-mismatch") == 0 ? UMI_STATUS_INVALID_STATE :
            strcmp(mode, "limit") == 0 ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_NOT_FOUND;
        CHECK(UmiLearningPracticeCopy(pack, selection, destination, "new.c",
            strcmp(mode, "limit") == 0 ? 2U : 4096U, &report) == expected);
        CHECK(report.status == expected && !report.source_verified && !report.destination_attempted && !report.destination.created);
        int exists = 1;
        CHECK(UmiRootedFileExists(destination, "new.c", &exists) == UMI_STATUS_OK && !exists);
    } else if (strcmp(mode, "path-refusal") == 0) {
        CHECK(UmiLearningPracticeCopy(pack, "source", destination, "../escape.c", 4096U, &report) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!report.source_verified && !report.destination_attempted && !report.destination.created);
        CHECK(UmiLearningPracticeCopy(pack, "source", destination, ".git/config", 4096U, &report) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(!report.source_verified && !report.destination_attempted);
    } else if (strcmp(mode, "missing-parent") == 0) {
        CHECK(UmiLearningPracticeCopy(pack, "source", destination, "absent/new.c", 4096U, &report) == UMI_STATUS_NOT_FOUND);
        CHECK(report.source_verified && report.destination_attempted && !report.destination.created && !report.destination.complete);
    } else if (strcmp(mode, "arguments") == 0) {
        CHECK(UmiLearningPracticeCopy(NULL, "source", destination, "new.c", 4096U, &report) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningPracticeCopy(pack, NULL, destination, "new.c", 4096U, &report) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningPracticeCopy(pack, "source", NULL, "new.c", 4096U, &report) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningPracticeCopy(pack, "source", destination, NULL, 4096U, &report) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningPracticeCopy(pack, "source", destination, "new.c", 0U, &report) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningPracticeCopy(pack, "source", destination, "new.c", UMI_LEARNING_PRACTICE_MAX_BYTES + 1U, &report) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!report.source_verified && !report.destination_attempted && !report.destination.created && report.source_size == 0U);
        CHECK(UmiLearningPracticeCopy(pack, "source", destination, "new.c", 4096U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else { UmiLearningPackDestroy(pack); return 2; }
    UmiLearningPackDestroy(pack);
    return 0;
}
