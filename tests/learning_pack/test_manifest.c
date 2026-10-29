/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_manifest.c
 *
 * PURPOSE:
 *   Verify pack admission, compatibility, references and shelf ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/learning_pack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char manifest[] =
    "{\"schema\":\"umicom.learning.pack\",\"version\":1,"
    "\"id\":\"framework.learning\",\"title\":\"Learning Framework\",\"product\":\"framework\","
    "\"locale\":\"en-GB\",\"revision\":3,"
    "\"compatibility\":{\"minimum\":[0,9,0],\"maximum\":[0,9,9]},"
    "\"resources\":["
    "{\"id\":\"guide\",\"title\":\"Understand services\",\"topic\":\"services\",\"kind\":\"guide\","
    "\"path\":\"guide.html\",\"sha256\":\"0000000000000000000000000000000000000000000000000000000000000000\","
    "\"example\":\"example\"},"
    "{\"id\":\"example\",\"title\":\"Read a service\",\"topic\":\"services\",\"kind\":\"example\","
    "\"path\":\"example.c\",\"sha256\":\"0000000000000000000000000000000000000000000000000000000000000000\"}]}";

typedef struct InvalidCase { const char *name, *before, *after; } InvalidCase;
static const InvalidCase invalid_cases[] = {
    {"schema", "umicom.learning.pack", "other.learning.pack"},
    {"version", "\"version\":1", "\"version\":2"},
    {"duplicate-key", "\"version\":1", "\"version\":1,\"version\":1"},
    {"escaped-duplicate", "\"version\":1", "\"version\":1,\"ver\\u0073ion\":1"},
    {"unknown-field", "\"revision\":3", "\"revision\":3,\"private_notes\":\"ignored?\""},
    {"unknown-resource", "\"example\":\"example\"", "\"example\":\"example\",\"execute\":true"},
    {"duplicate-resource", "\"id\":\"example\"", "\"id\":\"guide\""},
    {"missing-example", "\"example\":\"example\"", "\"example\":\"absent\""},
    {"wrong-example-kind", "\"kind\":\"example\"", "\"kind\":\"lesson\""},
    {"self-reference", "\"example\":\"example\"", "\"example\":\"guide\""},
    {"traversal", "\"path\":\"guide.html\"", "\"path\":\"../guide.html\""},
    {"absolute", "\"path\":\"guide.html\"", "\"path\":\"C:/private/guide.html\""},
    {"backslash-traversal", "\"path\":\"guide.html\"", "\"path\":\"lessons\\\\..\\\\guide.html\""},
    {"device", "\"path\":\"guide.html\"", "\"path\":\"CON\""},
    {"invalid-hash", "\"sha256\":\"0", "\"sha256\":\"x"},
    {"negative-revision", "\"revision\":3", "\"revision\":-1"},
    {"fractional-version", "\"version\":1", "\"version\":1.0"},
    {"reversed-range", "\"maximum\":[0,9,9]", "\"maximum\":[0,8,9]"},
    {"overflow-version", "\"maximum\":[0,9,9]", "\"maximum\":[0,9,65536]"},
    {"locale", "\"locale\":\"en-GB\"", "\"locale\":\"en--GB\""},
    {"title-control", "\"title\":\"Learning Framework\"", "\"title\":\"Learning\\nFramework\""},
    {"title-type", "\"title\":\"Learning Framework\"", "\"title\":42"},
    {"invalid-id", "\"id\":\"framework.learning\"", "\"id\":\"../framework\""},
    {"resources-type", "\"resources\":[", "\"resources\":{"},
    {"trailing-json", "}]}","}]} true"}
};
/* Mutate exactly one field in a known fixture. Refuse a missing pattern rather
 * than accidentally testing the original valid document after an edit. */
static char *Replace(const char *input, const char *before, const char *after)
{
    const char *hit = strstr(input, before);
    if (hit == NULL) return NULL;
    size_t prefix = (size_t)(hit - input);
    size_t size = strlen(input) - strlen(before) + strlen(after);
    char *result = malloc(size + 1U);
    if (result == NULL) return NULL;
    memcpy(result, input, prefix);
    memcpy(result + prefix, after, strlen(after));
    memcpy(result + prefix + strlen(after), hit + strlen(before),
           strlen(hit + strlen(before)) + 1U);
    return result;
}
#define CHECK(condition) do { \
    if (!(condition)) { \
        (void)fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
        result = 1; goto cleanup; \
    } \
} while (0)

static int Check(const char *name, const char *root)
{
    int result = 0;
    UmiLearningPack *pack = NULL, *other = NULL;
    UmiLearningShelf *shelf = NULL;
    char *input = NULL;
    UmiVersion host = {0U, 9U, 1U};
    UmiStatus status;
    for (size_t i = 0U; i < sizeof(invalid_cases) / sizeof(invalid_cases[0]); ++i)
        if (strcmp(name, invalid_cases[i].name) == 0) {
            input = Replace(manifest, invalid_cases[i].before, invalid_cases[i].after);
            CHECK(input != NULL);
            status = UmiLearningPackCreate(input, strlen(input), root, host, &pack);
            CHECK(status != UMI_STATUS_OK && pack == NULL);
            goto cleanup;
        }
    if (strcmp(name, "incompatible-low") == 0 || strcmp(name, "incompatible-high") == 0) {
        host = strcmp(name, "incompatible-low") == 0
            ? (UmiVersion){0U, 8U, 9U} : (UmiVersion){0U, 9U, 10U};
        status = UmiLearningPackCreate(manifest, sizeof(manifest) - 1U, root, host, &pack);
        CHECK(status == UMI_STATUS_UNAVAILABLE && pack == NULL);
        goto cleanup;
    }
    if (strcmp(name, "invalid-arguments") == 0) {
        status = UmiLearningPackCreate(NULL, 1U, root, host, &pack);
        CHECK(status == UMI_STATUS_INVALID_ARGUMENT && pack == NULL);
        status = UmiLearningPackCreate(manifest, 0U, root, host, &pack);
        CHECK(status == UMI_STATUS_INVALID_ARGUMENT && pack == NULL);
        status = UmiLearningPackCreate(manifest, sizeof(manifest), "relative", host, &pack);
        CHECK(status != UMI_STATUS_OK && pack == NULL);
        status = UmiLearningPackCreate(manifest, UMI_LEARNING_PACK_MANIFEST_BYTES + 1U, root, host, &pack);
        CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED && pack == NULL);
        status = UmiLearningPackCreate(manifest, sizeof(manifest), root, host, NULL);
        CHECK(status == UMI_STATUS_INVALID_ARGUMENT);
        goto cleanup;
    }

    if (strcmp(name, "maximum-version") == 0) host = (UmiVersion){0U, 9U, 9U};
    if (strcmp(name, "minimum-version") == 0) host = (UmiVersion){0U, 9U, 0U};
    if (strcmp(name, "embedded-nul") == 0) {
        input = Replace(manifest, "Learning Framework", "Learning Framework");
        CHECK(input != NULL);
        input[20] = '\0';
        status = UmiLearningPackCreate(input, sizeof(manifest) - 1U, root, host, &pack);
        CHECK(status != UMI_STATUS_OK && pack == NULL);
        goto cleanup;
    }

    /* Mutable input proves that accepted metadata belongs to the pack. */
    input = Replace(manifest, "Learning Framework", "Learning Framework");
    CHECK(input != NULL);
    status = UmiLearningPackCreate(input, strlen(input), root, host, &pack);
    CHECK(status == UMI_STATUS_OK);
    memset(input, '?', strlen(input));
    free(input);
    input = NULL;
    UmiLearningPackInfo info;
    UmiLearningPackResource resource, saved;
    status = UmiLearningPackCopyInfo(pack, &info);
    CHECK(status == UMI_STATUS_OK && info.resource_count == 2U && info.revision == 3U);
    CHECK(strcmp(info.title, "Learning Framework") == 0);
    status = UmiLearningPackFindResource(pack, "guide", &resource);
    CHECK(status == UMI_STATUS_OK && strcmp(resource.example_id, "example") == 0);
    memcpy(&saved, &resource, sizeof(saved));
    status = UmiLearningPackResourceAt(pack, 2U, &resource);
    CHECK(status == UMI_STATUS_NOT_FOUND && memcmp(&saved, &resource, sizeof(saved)) == 0);
    status = UmiLearningPackFindResource(pack, "missing", &resource);
    CHECK(status == UMI_STATUS_NOT_FOUND && memcmp(&saved, &resource, sizeof(saved)) == 0);
    if (strcmp(name, "owned-metadata") == 0 || strcmp(name, "minimum-version") == 0 ||
        strcmp(name, "maximum-version") == 0) {
        UmiLearningPackDestroy(pack);
        pack = NULL;
        CHECK(strcmp(resource.title, "Understand services") == 0);
        goto cleanup;
    }
    if (strcmp(name, "shelf") == 0) {
        status = UmiLearningShelfCreate(&shelf);
        CHECK(status == UMI_STATUS_OK);
        status = UmiLearningShelfAdopt(shelf, &pack);
        CHECK(status == UMI_STATUS_OK && pack == NULL && UmiLearningShelfCount(shelf) == 1U);
        status = UmiLearningPackCreate(manifest, sizeof(manifest) - 1U, root, host, &other);
        CHECK(status == UMI_STATUS_OK);
        status = UmiLearningShelfAdopt(shelf, &other);
        CHECK(status == UMI_STATUS_ALREADY_EXISTS && other != NULL && UmiLearningShelfCount(shelf) == 1U);
        UmiLearningPackDestroy(other);
        other = NULL;
        input = Replace(manifest, "\"id\":\"framework.learning\"", "\"id\":\"studio.learning\"");
        CHECK(input != NULL);
        status = UmiLearningPackCreate(input, strlen(input), root, host, &other);
        CHECK(status == UMI_STATUS_OK);
        status = UmiLearningShelfAdopt(shelf, &other);
        CHECK(status == UMI_STATUS_OK && other == NULL && UmiLearningShelfCount(shelf) == 2U);
        status = UmiLearningShelfRemove(shelf, "framework.learning");
        CHECK(status == UMI_STATUS_OK && UmiLearningShelfCount(shelf) == 1U);
        status = UmiLearningShelfPackAt(shelf, 0U, &info);
        CHECK(status == UMI_STATUS_OK && strcmp(info.id, "studio.learning") == 0);
        status = UmiLearningShelfFindResource(shelf, "studio.learning", "guide", &resource);
        CHECK(status == UMI_STATUS_OK);
        status = UmiLearningShelfFindResource(shelf, "framework.learning", "guide", &resource);
        CHECK(status == UMI_STATUS_NOT_FOUND);
        goto cleanup;
    }
    CHECK(!"Unknown learning pack case");
cleanup:
    free(input);
    UmiLearningPackDestroy(pack);
    UmiLearningPackDestroy(other);
    UmiLearningShelfDestroy(shelf);
    return result;
}
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    return Check(argv[1], argv[2]);
}
