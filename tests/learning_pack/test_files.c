/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_files.c
 *
 * PURPOSE:
 *   Verify bounded content reads and multi-pack discovery in isolated fixtures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/learning_pack.h"
#include "umicom/platform/rooted_files.h"
#include "umicom/platform/filesystem.h"
#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { \
    if (!(condition)) { \
        (void)fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
        result = 1; goto cleanup; \
    } \
} while (0)

static UmiStatus WritePack(const char *root, const char *id, const void *bytes, size_t size)
{
    char digest[UMI_SHA256_HEX_CAPACITY];
    UmiStatus status = UmiSha256Buffer(bytes, size, digest);
    if (status != UMI_STATUS_OK) return status;
    char manifest[2048];
    /* id comes only from fixed test literals, not external text. Production
     * authoring tools must use a JSON writer to escape arbitrary metadata. */
    int written = snprintf(manifest, sizeof(manifest),
        "{\"schema\":\"umicom.learning.pack\",\"version\":1,\"id\":\"%s\","
        "\"title\":\"Fixture guide\",\"product\":\"framework\",\"locale\":\"en\","
        "\"revision\":1,\"compatibility\":{\"minimum\":[0,9,0],\"maximum\":[0,9,9]},"
        "\"resources\":[{\"id\":\"guide\",\"title\":\"Guide\",\"topic\":\"ownership\","
        "\"kind\":\"guide\",\"path\":\"guide.html\",\"sha256\":\"%s\"}]}", id, digest);
    if (written < 0 || (size_t)written >= sizeof(manifest)) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = UmiRootedFileWrite(root, "guide.html", bytes, size);
    if (status == UMI_STATUS_OK)
        status = UmiRootedFileWrite(root, "learning-pack.json", manifest, (size_t)written);
    return status;
}
static int Check(const char *name, const char *root)
{
    int result = 0;
    UmiLearningPack *pack = NULL, *second = NULL;
    UmiLearningShelf *shelf = NULL;
    unsigned char *received = NULL;
    size_t size = 0U;
    static const unsigned char guide[] = "<h1>Learning services</h1>\n";
    static const unsigned char binary[] = {0U, 1U, 0x7fU, 0xffU, 0U};
    const unsigned char *expected = strcmp(name, "binary") == 0 ? binary : guide;
    size_t expected_size = strcmp(name, "binary") == 0 ? sizeof(binary) : sizeof(guide) - 1U;
    UmiVersion host = {0U, 9U, 0U};
    UmiStatus status = WritePack(root, "framework.pack", expected, expected_size);
    CHECK(status == UMI_STATUS_OK);
    status = UmiLearningPackLoad(root, "learning-pack.json", host, &pack);
    CHECK(status == UMI_STATUS_OK);
    if (strcmp(name, "changed-content") == 0) {
        status = UmiRootedFileWrite(root, "guide.html", "changed", 7U);
        CHECK(status == UMI_STATUS_OK);
        size = 99U;
        status = UmiLearningPackReadResource(pack, "guide", 4096U, &received, &size);
        CHECK(status == UMI_STATUS_INVALID_STATE && received == NULL && size == 0U);
        goto cleanup;
    }
    if (strcmp(name, "bounded") == 0) {
        size = 99U;
        status = UmiLearningPackReadResource(pack, "guide", expected_size - 1U, &received, &size);
        CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED && received == NULL && size == 0U);
        goto cleanup;
    }
    if (strcmp(name, "missing-file") == 0) {
        status = UmiRootedFileRemove(root, "guide.html");
        CHECK(status == UMI_STATUS_OK);
        status = UmiLearningPackReadResource(pack, "guide", 4096U, &received, &size);
        CHECK(status == UMI_STATUS_NOT_FOUND && received == NULL && size == 0U);
        goto cleanup;
    }
    if (strcmp(name, "missing-id") == 0) {
        size = 99U;
        status = UmiLearningPackReadResource(pack, "absent", 4096U, &received, &size);
        CHECK(status == UMI_STATUS_NOT_FOUND && received == NULL && size == 0U);
        goto cleanup;
    }
    if (strcmp(name, "two-packs") == 0) {
        char other_root[UMI_PATH_CAPACITY];
        status = umi_path_join(root, "second", other_root, sizeof(other_root));
        CHECK(status == UMI_STATUS_OK);
        status = umi_fs_make_directories(other_root);
        CHECK(status == UMI_STATUS_OK);
        static const char other_guide[] = "<h1>Learning the editor</h1>\n";
        status = WritePack(other_root, "studio.pack", other_guide, sizeof(other_guide) - 1U);
        CHECK(status == UMI_STATUS_OK);
        status = UmiLearningPackLoad(other_root, "learning-pack.json", host, &second);
        CHECK(status == UMI_STATUS_OK);
        status = UmiLearningShelfCreate(&shelf);
        CHECK(status == UMI_STATUS_OK);
        status = UmiLearningShelfAdopt(shelf, &pack);
        CHECK(status == UMI_STATUS_OK && pack == NULL);
        status = UmiLearningShelfAdopt(shelf, &second);
        CHECK(status == UMI_STATUS_OK && second == NULL);
        CHECK(UmiLearningShelfCount(shelf) == 2U);
        status = UmiLearningShelfReadResource(shelf, "framework.pack", "guide", 4096U, &received, &size);
        CHECK(status == UMI_STATUS_OK && size == expected_size && memcmp(received, expected, size) == 0);
        UmiLearningPackFreeBytes(received);
        received = NULL;
        status = UmiLearningShelfReadResource(shelf, "studio.pack", "guide", 4096U, &received, &size);
        CHECK(status == UMI_STATUS_OK && size == sizeof(other_guide) - 1U &&
              memcmp(received, other_guide, size) == 0);
        goto cleanup;
    }
    CHECK(strcmp(name, "read") == 0 || strcmp(name, "binary") == 0);
    status = UmiLearningPackReadResource(pack, "guide", 4096U, &received, &size);
    CHECK(status == UMI_STATUS_OK && size == expected_size && memcmp(received, expected, size) == 0);
    CHECK(received[size] == 0U);
    /* The caller owns the verified bytes independently of the manifest owner. */
    UmiLearningPackDestroy(pack);
    pack = NULL;
    CHECK(memcmp(received, expected, size) == 0);
cleanup:
    UmiLearningPackFreeBytes(received);
    UmiLearningPackDestroy(pack);
    UmiLearningPackDestroy(second);
    UmiLearningShelfDestroy(shelf);
    return result;
}
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    return Check(argv[1], argv[2]);
}
