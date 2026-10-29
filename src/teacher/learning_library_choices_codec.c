/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/learning_library_choices_codec.c
 *
 * PURPOSE:
 *   Encode and admit private library choices through bounded shared JSON services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "learning_library_choices_internal.h"
#include "umicom/language_runtime/json_tree.h"
#include "umicom/language_runtime/json_writer.h"
#include <stdlib.h>
#include <string.h>

#define CHOICE_TRY(call) do { UmiStatus result = (call); \
    if (result != UMI_STATUS_OK) return result; } while (0)
/* Exact object cardinality plus a lookup for every required member rejects
 * unknown and duplicate fields, including escaped duplicate names. There are
 * no ignored extension keys whose meaning might change on a future load. */
static UmiStatus ChoiceDecodeEntry(const UmiJsonTree *tree, int node,
    UmiLearningLibraryChoice *entry)
{
    if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT ||
        UmiJsonTreeCount(tree, node) != 6U) return UMI_STATUS_PARSE_ERROR;
    const char *names[] = {"root", "manifest", "id", "product", "locale"};
    char *values[] = {entry->root, entry->manifest, entry->pack_id, entry->product, entry->locale};
    const size_t capacities[] = {sizeof(entry->root), sizeof(entry->manifest),
        sizeof(entry->pack_id), sizeof(entry->product), sizeof(entry->locale)};
    for (size_t i = 0U; i < 5U; ++i) {
        int value = -1;
        CHOICE_TRY(UmiJsonTreeMember(tree, node, names[i], &value));
        CHOICE_TRY(UmiJsonTreeText(tree, value, values[i], capacities[i]));
    }
    int value = -1;
    int64_t revision = 0;
    CHOICE_TRY(UmiJsonTreeMember(tree, node, "revision", &value));
    CHOICE_TRY(UmiJsonTreeInteger(tree, value, &revision));
    if (revision < 1) return UMI_STATUS_PARSE_ERROR;
    entry->revision = (uint64_t)revision;
    return UMI_STATUS_OK;
}
static UmiStatus ChoiceDecodeRoot(const UmiJsonTree *tree, UmiLearningLibraryChoices *choices)
{
    if (UmiJsonTreeKind(tree, 0) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT ||
        UmiJsonTreeCount(tree, 0) != 3U) return UMI_STATUS_PARSE_ERROR;
    int node = -1;
    char schema[64];
    int64_t version = 0;
    CHOICE_TRY(UmiJsonTreeMember(tree, 0, "schema", &node));
    CHOICE_TRY(UmiJsonTreeText(tree, node, schema, sizeof(schema)));
    CHOICE_TRY(UmiJsonTreeMember(tree, 0, "version", &node));
    CHOICE_TRY(UmiJsonTreeInteger(tree, node, &version));
    if (strcmp(schema, "umicom.learning.libraries") != 0 || version != 1)
        return UMI_STATUS_PARSE_ERROR;
    CHOICE_TRY(UmiJsonTreeMember(tree, 0, "libraries", &node));
    if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
        return UMI_STATUS_PARSE_ERROR;
    size_t count = UmiJsonTreeCount(tree, node);
    if (count > UMI_LEARNING_LIBRARY_CHOICES_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    node = UmiJsonTreeFirst(tree, node);
    for (size_t i = 0U; i < count; ++i) {
        UmiLearningLibraryChoice entry = {0};
        CHOICE_TRY(ChoiceDecodeEntry(tree, node, &entry));
        CHOICE_TRY(LearningLibraryChoiceAdd(choices, &entry));
        node = UmiJsonTreeNext(tree, node);
    }
    return UMI_STATUS_OK;
}
#undef CHOICE_TRY
UmiStatus UmiLearningLibraryChoicesDecode(const void *bytes, size_t size,
    UmiLearningLibraryChoices **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (bytes == NULL || size == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    UmiJsonTreeLimits limits = {UMI_LEARNING_LIBRARY_CHOICES_BYTES, 1024U, 4U};
    UmiJsonTree *tree = NULL;
    UmiLearningLibraryChoices *choices = NULL;
    UmiStatus status = UmiJsonTreeCreate(bytes, size, &limits, NULL, &tree);
    if (status == UMI_STATUS_OK) status = UmiLearningLibraryChoicesCreate(&choices);
    if (status == UMI_STATUS_OK) status = ChoiceDecodeRoot(tree, choices);
    UmiJsonTreeDestroy(tree);
    if (status != UMI_STATUS_OK) { UmiLearningLibraryChoicesDestroy(choices); return status; }
    *out = choices;
    return UMI_STATUS_OK;
}
UmiStatus UmiLearningLibraryChoicesEncode(const UmiLearningLibraryChoices *choices,
    char *out, size_t capacity, size_t *out_size)
{
    if (choices == NULL || out_size == NULL || (out == NULL && capacity != 0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Stage the whole document so insufficient output space never publishes
     * an apparently valid prefix. Bounds include worst-case escaped paths. */
    char *encoded = malloc(UMI_LEARNING_LIBRARY_CHOICES_BYTES + 1U);
    if (encoded == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiLanguageRuntimeJsonWriter writer;
    umi_language_runtime_json_writer_init(&writer, encoded, UMI_LEARNING_LIBRARY_CHOICES_BYTES + 1U);
    umi_language_runtime_json_writer_raw(&writer,
        "{\"schema\":\"umicom.learning.libraries\",\"version\":1,\"libraries\":[");
    for (size_t i = 0U; i < choices->count; ++i) {
        const UmiLearningLibraryChoice *entry = &choices->entries[i];
        if (i != 0U) umi_language_runtime_json_writer_raw(&writer, ",");
        umi_language_runtime_json_writer_raw(&writer, "{\"root\":");
        umi_language_runtime_json_writer_string(&writer, entry->root);
        umi_language_runtime_json_writer_raw(&writer, ",\"manifest\":");
        umi_language_runtime_json_writer_string(&writer, entry->manifest);
        umi_language_runtime_json_writer_raw(&writer, ",\"id\":");
        umi_language_runtime_json_writer_string(&writer, entry->pack_id);
        umi_language_runtime_json_writer_raw(&writer, ",\"product\":");
        umi_language_runtime_json_writer_string(&writer, entry->product);
        umi_language_runtime_json_writer_raw(&writer, ",\"locale\":");
        umi_language_runtime_json_writer_string(&writer, entry->locale);
        umi_language_runtime_json_writer_raw(&writer, ",\"revision\":");
        umi_language_runtime_json_writer_uint64(&writer, entry->revision);
        umi_language_runtime_json_writer_raw(&writer, "}");
    }
    umi_language_runtime_json_writer_raw(&writer, "]}\n");
    UmiStatus status = writer.status;
    if (status == UMI_STATUS_OK) {
        *out_size = writer.length;
        if (capacity <= writer.length) status = UMI_STATUS_CAPACITY_EXCEEDED;
        else memcpy(out, encoded, writer.length + 1U);
    }
    free(encoded);
    return status;
}
