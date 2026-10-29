/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/learning_pack.c
 *
 * PURPOSE:
 *   Parse immutable documentation packs with shared bounded JSON and path rules.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "learning_pack_internal.h"
#include "umicom/language_runtime/json_tree.h"
#include "umicom/platform/rooted_files.h"
#include <stdlib.h>
#include <string.h>

/* A pack extends document discovery, not the existing lesson assessment model.
 * All input is staged in an unpublished owner until every reference is valid. */
static int PackAlphaNumeric(unsigned char ch)
{
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
           (ch >= '0' && ch <= '9');
}
static int PackIdentifier(const char *text)
{
    if (!PackAlphaNumeric((unsigned char)text[0])) return 0;
    for (size_t i = 1U; text[i] != '\0'; ++i)
        if (!PackAlphaNumeric((unsigned char)text[i]) && text[i] != '.' &&
            text[i] != '-' && text[i] != '_') return 0;
    return 1;
}
static int PackLabel(const char *text)
{
    int visible = 0;
    for (size_t i = 0U; text[i] != '\0'; ++i) {
        unsigned char ch = (unsigned char)text[i];
        if (ch < 0x20U || ch == 0x7fU) return 0;
        if (ch != ' ') visible = 1;
    }
    return visible;
}
static int PackLocale(const char *text)
{
    int segment = 0;
    for (size_t i = 0U; text[i] != '\0'; ++i) {
        if (PackAlphaNumeric((unsigned char)text[i])) segment = 1;
        else if (text[i] == '-' && segment) segment = 0;
        else return 0;
    }
    return segment;
}
/* Version one has a closed schema. Checking every key also rejects duplicates
 * that a selective member reader could otherwise leave uninterpreted. */
static UmiStatus PackFields(const UmiJsonTree *tree, int object,
    const char *const *allowed, size_t allowed_count)
{
    if (UmiJsonTreeKind(tree, object) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        return UMI_STATUS_PARSE_ERROR;
    for (int key = UmiJsonTreeFirst(tree, object); key >= 0;) {
        char name[64];
        UmiStatus status = UmiJsonTreeText(tree, key, name, sizeof(name));
        if (status != UMI_STATUS_OK) return status;
        int accepted = 0;
        for (size_t i = 0U; i < allowed_count; ++i)
            if (strcmp(name, allowed[i]) == 0) { accepted = 1; break; }
        if (!accepted) return UMI_STATUS_PARSE_ERROR;
        int value = -1;
        status = UmiJsonTreeMember(tree, object, name, &value);
        if (status != UMI_STATUS_OK) return status;
        key = UmiJsonTreeNext(tree, value);
    }
    return UMI_STATUS_OK;
}
static UmiStatus PackText(const UmiJsonTree *tree, int object, const char *key,
    char *out, size_t capacity, int optional)
{
    int value = -1;
    UmiStatus status = UmiJsonTreeMember(tree, object, key, &value);
    if (optional && status == UMI_STATUS_NOT_FOUND) return UMI_STATUS_OK;
    if (status != UMI_STATUS_OK) return status;
    return UmiJsonTreeText(tree, value, out, capacity);
}
static UmiStatus PackInteger(const UmiJsonTree *tree, int object, const char *key,
    int64_t *out)
{
    int value = -1;
    UmiStatus status = UmiJsonTreeMember(tree, object, key, &value);
    if (status != UMI_STATUS_OK) return status;
    return UmiJsonTreeInteger(tree, value, out);
}
static UmiStatus PackVersion(const UmiJsonTree *tree, int object, const char *key,
    UmiVersion *out)
{
    int array = -1;
    UmiStatus status = UmiJsonTreeMember(tree, object, key, &array);
    if (status != UMI_STATUS_OK) return status;
    if (UmiJsonTreeKind(tree, array) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY ||
        UmiJsonTreeCount(tree, array) != 3U) return UMI_STATUS_PARSE_ERROR;
    uint16_t parts[3] = {0};
    int node = UmiJsonTreeFirst(tree, array);
    for (size_t i = 0U; i < 3U; ++i) {
        int64_t value = -1;
        status = UmiJsonTreeInteger(tree, node, &value);
        if (status != UMI_STATUS_OK) return status;
        if (value < 0 || value > UINT16_MAX) return UMI_STATUS_PARSE_ERROR;
        parts[i] = (uint16_t)value;
        node = UmiJsonTreeNext(tree, node);
    }
    *out = (UmiVersion){parts[0], parts[1], parts[2]};
    return UMI_STATUS_OK;
}
static int PackCompareVersion(UmiVersion left, UmiVersion right)
{
    if (left.major != right.major) return left.major < right.major ? -1 : 1;
    if (left.minor != right.minor) return left.minor < right.minor ? -1 : 1;
    if (left.patch != right.patch) return left.patch < right.patch ? -1 : 1;
    return 0;
}
#define PACK_TRY(call) do { UmiStatus pack_status = (call); \
    if (pack_status != UMI_STATUS_OK) return pack_status; } while (0)

static UmiStatus PackInfo(const UmiJsonTree *tree, UmiVersion host, UmiLearningPackInfo *info)
{
    static const char *const fields[] = {
        "schema", "version", "id", "title", "product", "locale", "revision", "compatibility", "resources"
    };
    static const char *const compatibility_fields[] = {"minimum", "maximum"};
    PACK_TRY(PackFields(tree, 0, fields, sizeof(fields) / sizeof(fields[0])));
    char schema[64];
    int64_t version = 0, revision = 0;
    PACK_TRY(PackText(tree, 0, "schema", schema, sizeof(schema), 0));
    PACK_TRY(PackInteger(tree, 0, "version", &version));
    if (strcmp(schema, "umicom.learning.pack") != 0 || version != 1)
        return UMI_STATUS_PARSE_ERROR;
    PACK_TRY(PackText(tree, 0, "id", info->id, sizeof(info->id), 0));
    PACK_TRY(PackText(tree, 0, "title", info->title, sizeof(info->title), 0));
    PACK_TRY(PackText(tree, 0, "product", info->product, sizeof(info->product), 0));
    PACK_TRY(PackText(tree, 0, "locale", info->locale, sizeof(info->locale), 0));
    PACK_TRY(PackInteger(tree, 0, "revision", &revision));
    if (!PackIdentifier(info->id) || !PackIdentifier(info->product) ||
        !PackLabel(info->title) || !PackLocale(info->locale) || revision < 1)
        return UMI_STATUS_PARSE_ERROR;
    info->revision = (uint64_t)revision;
    int compatibility = -1;
    PACK_TRY(UmiJsonTreeMember(tree, 0, "compatibility", &compatibility));
    PACK_TRY(PackFields(tree, compatibility, compatibility_fields, 2U));
    PACK_TRY(PackVersion(tree, compatibility, "minimum", &info->minimum_version));
    PACK_TRY(PackVersion(tree, compatibility, "maximum", &info->maximum_version));
    if (PackCompareVersion(info->minimum_version, info->maximum_version) > 0)
        return UMI_STATUS_PARSE_ERROR;
    if (PackCompareVersion(host, info->minimum_version) < 0 ||
        PackCompareVersion(host, info->maximum_version) > 0) return UMI_STATUS_UNAVAILABLE;
    return UMI_STATUS_OK;
}
static UmiStatus PackResource(const UmiJsonTree *tree, int object, const char *root,
    UmiLearningPackResource *resource)
{
    static const char *const fields[] = {"id", "title", "topic", "kind", "path", "sha256", "example"};
    PACK_TRY(PackFields(tree, object, fields, sizeof(fields) / sizeof(fields[0])));
    PACK_TRY(PackText(tree, object, "id", resource->id, sizeof(resource->id), 0));
    PACK_TRY(PackText(tree, object, "title", resource->title, sizeof(resource->title), 0));
    PACK_TRY(PackText(tree, object, "topic", resource->topic, sizeof(resource->topic), 0));
    PACK_TRY(PackText(tree, object, "path", resource->path, sizeof(resource->path), 0));
    PACK_TRY(PackText(tree, object, "sha256", resource->sha256, sizeof(resource->sha256), 0));
    PACK_TRY(PackText(tree, object, "example", resource->example_id, sizeof(resource->example_id), 1));
    if (!PackIdentifier(resource->id) || !PackIdentifier(resource->topic) ||
        !PackLabel(resource->title) ||
        (resource->example_id[0] != '\0' && !PackIdentifier(resource->example_id)))
        return UMI_STATUS_PARSE_ERROR;
    char kind[32];
    PACK_TRY(PackText(tree, object, "kind", kind, sizeof(kind), 0));
    if (strcmp(kind, "lesson") == 0) resource->kind = UMI_LEARNING_RESOURCE_LESSON;
    else if (strcmp(kind, "guide") == 0) resource->kind = UMI_LEARNING_RESOURCE_GUIDE;
    else if (strcmp(kind, "example") == 0) resource->kind = UMI_LEARNING_RESOURCE_EXAMPLE;
    else if (strcmp(kind, "image") == 0) resource->kind = UMI_LEARNING_RESOURCE_IMAGE;
    else return UMI_STATUS_PARSE_ERROR;
    PACK_TRY(UmiRootedFileValidatePath(root, resource->path));
    if (strlen(resource->sha256) != 64U) return UMI_STATUS_PARSE_ERROR;
    for (size_t i = 0U; i < 64U; ++i) {
        char ch = resource->sha256[i];
        if (ch >= 'A' && ch <= 'F') ch = (char)(ch - 'A' + 'a');
        if (!((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f')))
            return UMI_STATUS_PARSE_ERROR;
        resource->sha256[i] = ch;
    }
    return UMI_STATUS_OK;
}
static UmiStatus PackResources(const UmiJsonTree *tree, UmiLearningPack *pack)
{
    int array = -1;
    PACK_TRY(UmiJsonTreeMember(tree, 0, "resources", &array));
    if (UmiJsonTreeKind(tree, array) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
        return UMI_STATUS_PARSE_ERROR;
    size_t count = UmiJsonTreeCount(tree, array);
    if (count == 0U) return UMI_STATUS_PARSE_ERROR;
    if (count > UMI_LEARNING_PACK_MAX_RESOURCES) return UMI_STATUS_CAPACITY_EXCEEDED;
    pack->resources = calloc(count, sizeof(*pack->resources));
    if (pack->resources == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    pack->info.resource_count = count;
    int node = UmiJsonTreeFirst(tree, array);
    for (size_t i = 0U; i < count; ++i) {
        PACK_TRY(PackResource(tree, node, pack->root, &pack->resources[i]));
        for (size_t previous = 0U; previous < i; ++previous)
            if (strcmp(pack->resources[i].id, pack->resources[previous].id) == 0)
                return UMI_STATUS_ALREADY_EXISTS;
        node = UmiJsonTreeNext(tree, node);
    }
    for (size_t i = 0U; i < count; ++i) {
        const char *example = pack->resources[i].example_id;
        if (example[0] == '\0') continue;
        int found = 0;
        for (size_t j = 0U; j < count; ++j)
            if (strcmp(example, pack->resources[j].id) == 0) {
                if (i == j || pack->resources[j].kind != UMI_LEARNING_RESOURCE_EXAMPLE)
                    return UMI_STATUS_PARSE_ERROR;
                found = 1;
                break;
            }
        if (!found) return UMI_STATUS_NOT_FOUND;
    }
    return UMI_STATUS_OK;
}
#undef PACK_TRY

UmiStatus UmiLearningPackCreate(const void *json, size_t length, const char *root,
    UmiVersion host, UmiLearningPack **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (json == NULL || root == NULL || length == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_LEARNING_PACK_MANIFEST_BYTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = UmiRootedFileValidatePath(root, "learning-pack.json");
    if (status != UMI_STATUS_OK) return status;
    UmiLearningPack *pack = calloc(1U, sizeof(*pack));
    if (pack == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiJsonTree *tree = NULL;
    UmiJsonTreeLimits limits = {UMI_LEARNING_PACK_MANIFEST_BYTES, 20000U, 8U};
    status = umi_path_normalise(root, pack->root, sizeof(pack->root));
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeCreate(json, length, &limits, NULL, &tree);
    if (status == UMI_STATUS_OK) status = PackInfo(tree, host, &pack->info);
    if (status == UMI_STATUS_OK) status = PackResources(tree, pack);
    UmiJsonTreeDestroy(tree);
    if (status != UMI_STATUS_OK) {
        UmiLearningPackDestroy(pack);
        return status;
    }
    *out = pack;
    return UMI_STATUS_OK;
}
void UmiLearningPackDestroy(UmiLearningPack *pack)
{
    if (pack == NULL) return;
    free(pack->resources);
    free(pack);
}
UmiStatus UmiLearningPackCopyInfo(const UmiLearningPack *pack, UmiLearningPackInfo *out)
{
    if (pack == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = pack->info;
    return UMI_STATUS_OK;
}
UmiStatus UmiLearningPackResourceAt(const UmiLearningPack *pack, size_t index,
    UmiLearningPackResource *out)
{
    if (pack == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= pack->info.resource_count) return UMI_STATUS_NOT_FOUND;
    *out = pack->resources[index];
    return UMI_STATUS_OK;
}
UmiStatus UmiLearningPackFindResource(const UmiLearningPack *pack, const char *id,
    UmiLearningPackResource *out)
{
    if (pack == NULL || id == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < pack->info.resource_count; ++i)
        if (strcmp(id, pack->resources[i].id) == 0) {
            *out = pack->resources[i];
            return UMI_STATUS_OK;
        }
    return UMI_STATUS_NOT_FOUND;
}
