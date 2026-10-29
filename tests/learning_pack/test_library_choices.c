/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_library_choices.c
 *
 * PURPOSE:
 *   Check strict private settings admission, copied ownership and refusal without mutation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "library_choices_fixture.h"
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); result = 1; goto cleanup; \
} } while (0)
typedef struct Invalid { const char *name, *before, *after; } Invalid;
static const Invalid invalid[] = {
    {"schema", "umicom.learning.libraries", "unrecognised.libraries"},
    {"version", "\"version\":1", "\"version\":2"},
    {"fractional-version", "\"version\":1", "\"version\":1.0"},
    {"duplicate", "\"version\":1", "\"version\":1,\"version\":1"},
    {"escaped-duplicate", "\"version\":1", "\"version\":1,\"ver\\u0073ion\":1"},
    {"unknown", "\"version\":1", "\"version\":1,\"execute\":true"},
    {"missing", "\"product\":\"framework\",", ""},
    {"entry-duplicate", "\"product\":\"framework\"", "\"product\":\"framework\",\"product\":\"framework\""},
    {"entry-unknown", "\"revision\":3", "\"revision\":3,\"trusted\":true"},
    {"entry-type", "\"revision\":3", "\"revision\":\"3\""},
    {"revision", "\"revision\":3", "\"revision\":0"},
    {"revision-overflow", "\"revision\":3", "\"revision\":9223372036854775808"},
    {"traversal", "learning-pack.json", "../learning-pack.json"},
    {"absolute-manifest", "learning-pack.json", "/learning-pack.json"},
    {"device", "learning-pack.json", "CON"},
    {"control", "learning-pack.json", "learning\\npack.json"},
    {"embedded-nul", "learning-pack.json", "learning\\u0000pack.json"},
    {"locale", "\"locale\":\"en-GB\"", "\"locale\":\"en--GB\""},
    {"identifier", "\"id\":\"framework.learning\"", "\"id\":\"framework/learning\""},
    {"array-type", "\"libraries\":[", "\"libraries\":{\"invalid\":["}
};
static char *Replace(const char *text, const char *before, const char *after)
{
    const char *at = strstr(text, before);
    if (at == NULL) return NULL;
    size_t prefix = (size_t)(at - text);
    size_t length = strlen(text) - strlen(before) + strlen(after);
    char *copy = malloc(length + 1U);
    if (copy != NULL) {
        memcpy(copy, text, prefix);
        memcpy(copy + prefix, after, strlen(after));
        strcpy(copy + prefix + strlen(after), at + strlen(before));
    }
    return copy;
}
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    const char *name = argv[1], *root = argv[2];
    int result = 0;
    UmiLearningLibraryChoices *choices = NULL, *decoded = NULL, *clone = NULL;
    UmiLearningPack *pack = NULL;
    UmiLearningLibraryChoice entry, guard;
    char *bytes = malloc(UMI_LEARNING_LIBRARY_CHOICES_BYTES + 1U), *damaged = NULL;
    size_t size = 0U;
    CHECK(bytes != NULL);
    CHECK(UmiLearningLibraryChoicesCreate(&choices) == UMI_STATUS_OK);
    if (strcmp(name, "empty") == 0) {
        CHECK(UmiLearningLibraryChoicesCount(choices) == 0U);
        CHECK(UmiLearningLibraryChoicesEncode(choices, bytes, UMI_LEARNING_LIBRARY_CHOICES_BYTES + 1U, &size) == UMI_STATUS_OK);
        CHECK(strcmp(bytes, "{\"schema\":\"umicom.learning.libraries\",\"version\":1,\"libraries\":[]}\n") == 0);
        CHECK(UmiLearningLibraryChoicesDecode(bytes, size, &decoded) == UMI_STATUS_OK);
        CHECK(UmiLearningLibraryChoicesCount(decoded) == 0U);
        goto cleanup;
    }
    CHECK(ChoiceFixturePack(root, "framework.learning", "framework", "en-GB", 3U, 0, &pack) == UMI_STATUS_OK);
    CHECK(UmiLearningLibraryChoicesRemember(choices, pack, "learning-pack.json") == UMI_STATUS_OK);
    CHECK(UmiLearningLibraryChoicesAt(choices, 0U, &entry) == UMI_STATUS_OK);
    CHECK(entry.revision == 3U && strcmp(entry.pack_id, "framework.learning") == 0);
    UmiLearningPackDestroy(pack); pack = NULL;
    CHECK(UmiLearningLibraryChoicesEncode(choices, bytes, UMI_LEARNING_LIBRARY_CHOICES_BYTES + 1U, &size) == UMI_STATUS_OK);
    for (size_t i = 0U; i < sizeof(invalid) / sizeof(invalid[0]); ++i) {
        if (strcmp(name, invalid[i].name) != 0) continue;
        damaged = Replace(bytes, invalid[i].before, invalid[i].after);
        CHECK(damaged != NULL);
        CHECK(UmiLearningLibraryChoicesDecode(damaged, strlen(damaged), &decoded) != UMI_STATUS_OK);
        CHECK(decoded == NULL && UmiLearningLibraryChoicesCount(choices) == 1U);
        goto cleanup;
    }
    if (strcmp(name, "relative-root") == 0) {
        char *start = strstr(bytes, "\"root\":"), *end = strstr(bytes, ",\"manifest\":");
        CHECK(start != NULL && end != NULL);
        size_t prefix = (size_t)(start - bytes), suffix = strlen(end);
        damaged = malloc(prefix + 18U + suffix + 1U); CHECK(damaged != NULL);
        memcpy(damaged, bytes, prefix);
        strcpy(damaged + prefix, "\"root\":\"relative\"");
        strcat(damaged, end);
        CHECK(UmiLearningLibraryChoicesDecode(damaged, strlen(damaged), &decoded) != UMI_STATUS_OK);
        CHECK(decoded == NULL);
    } else if (strcmp(name, "trailing") == 0 || strcmp(name, "utf8") == 0 || strcmp(name, "nul") == 0) {
        if (strcmp(name, "trailing") == 0) { bytes[size++] = 'x'; bytes[size] = '\0'; }
        else if (strcmp(name, "utf8") == 0) *strstr(bytes, "framework.learning") = (char)0xff;
        else bytes[3] = '\0';
        CHECK(UmiLearningLibraryChoicesDecode(bytes, size, &decoded) != UMI_STATUS_OK && decoded == NULL);
    } else if (strcmp(name, "bounded") == 0) {
        CHECK(UmiLearningLibraryChoicesDecode(bytes, UMI_LEARNING_LIBRARY_CHOICES_BYTES + 1U, &decoded) != UMI_STATUS_OK);
        CHECK(decoded == NULL);
    } else if (strcmp(name, "outputs") == 0) {
        memset(&entry, 0xa5, sizeof(entry)); memcpy(&guard, &entry, sizeof(entry));
        CHECK(UmiLearningLibraryChoicesAt(choices, 1U, &entry) == UMI_STATUS_NOT_FOUND);
        CHECK(memcmp(&entry, &guard, sizeof(entry)) == 0);
        char output[2] = {'x','y'}; size_t required = 0U;
        CHECK(UmiLearningLibraryChoicesEncode(choices, output, sizeof(output), &required) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(output[0] == 'x' && output[1] == 'y' && required == size);
        CHECK(UmiLearningLibraryChoicesEncode(choices, NULL, 0U, &required) == UMI_STATUS_CAPACITY_EXCEEDED && required == size);
        CHECK(UmiLearningLibraryChoicesEncode(choices, output, sizeof(output), NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(name, "arguments") == 0) {
        CHECK(UmiLearningLibraryChoicesCreate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningLibraryChoicesClone(NULL, &clone) == UMI_STATUS_INVALID_ARGUMENT && clone == NULL);
        CHECK(UmiLearningLibraryChoicesRemember(choices, NULL, "learning-pack.json") == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningLibraryChoicesDecode(NULL, 0U, &decoded) == UMI_STATUS_INVALID_ARGUMENT && decoded == NULL);
        CHECK(UmiLearningLibraryChoicesAt(NULL, 0U, &entry) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningLibraryChoicesForget(choices, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(name, "clone") == 0 || strcmp(name, "forget") == 0) {
        CHECK(UmiLearningLibraryChoicesClone(choices, &clone) == UMI_STATUS_OK);
        CHECK(UmiLearningLibraryChoicesForget(choices, "absent") == UMI_STATUS_NOT_FOUND);
        CHECK(UmiLearningLibraryChoicesForget(choices, "framework.learning") == UMI_STATUS_OK);
        CHECK(UmiLearningLibraryChoicesCount(choices) == 0U && UmiLearningLibraryChoicesCount(clone) == 1U);
        CHECK(UmiLearningLibraryChoicesAt(clone, 0U, &entry) == UMI_STATUS_OK && entry.revision == 3U);
    } else if (strcmp(name, "capacity") == 0) {
        for (unsigned i = 1U; i <= UMI_LEARNING_LIBRARY_CHOICES_MAX; ++i) {
            char id[32]; snprintf(id, sizeof(id), "pack.%u", i);
            CHECK(ChoiceFixturePack(root, id, "framework", "en-GB", 1U, 0, &pack) == UMI_STATUS_OK);
            CHECK(UmiLearningLibraryChoicesRemember(choices, pack, "learning-pack.json") ==
                (i == UMI_LEARNING_LIBRARY_CHOICES_MAX ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK));
            UmiLearningPackDestroy(pack); pack = NULL;
        }
        CHECK(UmiLearningLibraryChoicesEncode(choices, bytes, UMI_LEARNING_LIBRARY_CHOICES_BYTES + 1U, &size) == UMI_STATUS_OK);
        CHECK(UmiLearningLibraryChoicesDecode(bytes, size, &decoded) == UMI_STATUS_OK);
        CHECK(UmiLearningLibraryChoicesCount(decoded) == UMI_LEARNING_LIBRARY_CHOICES_MAX);
    } else if (strcmp(name, "duplicate-id") == 0) {
        CHECK(ChoiceFixturePack(root, "framework.learning", "another", "fr", 4U, 0, &pack) == UMI_STATUS_OK);
        CHECK(UmiLearningLibraryChoicesRemember(choices, pack, "other.json") == UMI_STATUS_ALREADY_EXISTS);
        CHECK(UmiLearningLibraryChoicesAt(choices, 0U, &entry) == UMI_STATUS_OK && entry.revision == 3U);
    } else if (strcmp(name, "roundtrip") == 0) {
        CHECK(UmiLearningLibraryChoicesDecode(bytes, size, &decoded) == UMI_STATUS_OK);
        memset(bytes, 'x', size);
        CHECK(UmiLearningLibraryChoicesAt(decoded, 0U, &entry) == UMI_STATUS_OK);
        CHECK(strcmp(entry.pack_id, "framework.learning") == 0 && entry.revision == 3U);
    } else { fprintf(stderr, "Unknown case: %s\n", name); result = 2; }
cleanup:
    free(bytes); free(damaged);
    UmiLearningPackDestroy(pack);
    UmiLearningLibraryChoicesDestroy(choices);
    UmiLearningLibraryChoicesDestroy(decoded);
    UmiLearningLibraryChoicesDestroy(clone);
    return result;
}
