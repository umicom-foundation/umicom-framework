/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/model_discovery.c
 *
 * PURPOSE:
 *   Decode provider model observations through the shared JSON owner and expose immutable, complete identities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/model_discovery.h"
#include "umicom/language_runtime/json_tree.h"
#include "model_observation_json.h"
#include <stdlib.h>
#include <string.h>

struct UmiAiDiscoverySnapshot {
    UmiAiDiscoveryFormat format;
    size_t count;
    UmiAiDiscoveredModel *models;
    const UmiAiDiscoveredModel **sorted;
};

/* Missing optional fields keep zero-initialized storage. Present values must
 * retain their exact type: null, booleans and numbers are not display strings. */
/* Model lists and per-model details now use the same private exact-field admission. Retain the original helpers for review; optional absence, type checks, integer limits and failure semantics remain unchanged. The former implementation is retained for owner review. */
#if 0
static UmiStatus text_field(const UmiJsonTree *tree, int object, const char *name,
    int required, char *out, size_t capacity)
{
    int node = -1;
    UmiStatus status = UmiJsonTreeMember(tree, object, name, &node);
    if (status == UMI_STATUS_NOT_FOUND) return required ? UMI_STATUS_PARSE_ERROR : UMI_STATUS_OK;
    if (status != UMI_STATUS_OK) return status;
    if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_STRING) return UMI_STATUS_PARSE_ERROR;
    status = UmiJsonTreeText(tree, node, out, capacity);
    if (status == UMI_STATUS_OK && required && out[0] == '\0') return UMI_STATUS_PARSE_ERROR;
    return status;
}

static UmiStatus number_field(const UmiJsonTree *tree, int object, const char *name,
    uint64_t *out, uint32_t bit, uint32_t *known)
{
    int node = -1;
    UmiStatus status = UmiJsonTreeMember(tree, object, name, &node);
    if (status == UMI_STATUS_NOT_FOUND) return UMI_STATUS_OK;
    if (status != UMI_STATUS_OK) return status;
    int64_t value = 0;
    status = UmiJsonTreeInteger(tree, node, &value);
    if (status != UMI_STATUS_OK) return status;
    if (value < 0) return UMI_STATUS_PARSE_ERROR;
    *out = (uint64_t)value;
    *known |= bit;
    return UMI_STATUS_OK;
}


#endif
static UmiStatus text_field(const UmiJsonTree *tree,int object,const char *name,
    int required,char *out,size_t capacity)
{
    return UmiAiObservationText(tree,object,name,required,out,capacity);
}
static UmiStatus number_field(const UmiJsonTree *tree,int object,const char *name,
    uint64_t *out,uint32_t bit,uint32_t *known)
{
    return UmiAiObservationUnsigned(tree,object,name,out,bit,known);
}

static UmiStatus expected_text(const UmiJsonTree *tree, int object,
    const char *name, const char *expected)
{
    char value[32] = {0};
    UmiStatus status = text_field(tree, object, name, 1, value, sizeof(value));
    if (status != UMI_STATUS_OK) return status;
    return strcmp(value, expected) == 0 ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}

static UmiStatus model_read(const UmiJsonTree *tree, int node,
    UmiAiDiscoveryFormat format, UmiAiDiscoveredModel *out)
{
    if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT) return UMI_STATUS_PARSE_ERROR;
    const int openai = format == UMI_AI_DISCOVERY_OPENAI_LIST;
    UmiStatus status = text_field(tree, node, openai ? "id" : "model", 1, out->id, sizeof(out->id));
    if (status != UMI_STATUS_OK) return status;
    if (openai) {
        status = expected_text(tree, node, "object", "model");
        if (status != UMI_STATUS_OK) return status;
        memcpy(out->display_name, out->id, strlen(out->id) + 1U);
        return UMI_STATUS_OK;
    }
    status = text_field(tree, node, "name", 0, out->display_name, sizeof(out->display_name));
    if (status != UMI_STATUS_OK) return status;
    if (out->display_name[0] == '\0') memcpy(out->display_name, out->id, strlen(out->id) + 1U);
    status = text_field(tree, node, "digest", 0, out->digest, sizeof(out->digest));
    if (status != UMI_STATUS_OK) return status;
    status = number_field(tree, node, "size", &out->size_bytes, UMI_AI_DISCOVERY_SIZE, &out->known);
    if (status != UMI_STATUS_OK) return status;
    if (format == UMI_AI_DISCOVERY_OLLAMA_LOADED) {
        status = text_field(tree, node, "expires_at", 0, out->expires_at, sizeof(out->expires_at));
        if (status != UMI_STATUS_OK) return status;
        status = number_field(tree, node, "size_vram", &out->vram_bytes, UMI_AI_DISCOVERY_VRAM, &out->known);
        if (status != UMI_STATUS_OK) return status;
        status = number_field(tree, node, "context_length", &out->context_tokens, UMI_AI_DISCOVERY_CONTEXT, &out->known);
        if (status != UMI_STATUS_OK) return status;
    } else {
        status = text_field(tree, node, "modified_at", 0, out->modified_at, sizeof(out->modified_at));
        if (status != UMI_STATUS_OK) return status;
    }
    int details = -1;
    status = UmiJsonTreeMember(tree, node, "details", &details);
    if (status == UMI_STATUS_NOT_FOUND) return UMI_STATUS_OK;
    if (status != UMI_STATUS_OK) return status;
    if (UmiJsonTreeKind(tree, details) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT) return UMI_STATUS_PARSE_ERROR;
    status = text_field(tree, details, "format", 0, out->format, sizeof(out->format));
    if (status != UMI_STATUS_OK) return status;
    status = text_field(tree, details, "family", 0, out->family, sizeof(out->family));
    if (status != UMI_STATUS_OK) return status;
    status = text_field(tree, details, "parameter_size", 0, out->parameter_size, sizeof(out->parameter_size));
    if (status != UMI_STATUS_OK) return status;
    return text_field(tree, details, "quantization_level", 0, out->quantization, sizeof(out->quantization));
}

static int compare_models(const void *left, const void *right)
{
    const UmiAiDiscoveredModel *a = *(const UmiAiDiscoveredModel *const *)left;
    const UmiAiDiscoveredModel *b = *(const UmiAiDiscoveredModel *const *)right;
    return strcmp(a->id, b->id);
}

void UmiAiDiscoveryDestroy(UmiAiDiscoverySnapshot *snapshot)
{
    if (snapshot == NULL) return;
    free(snapshot->sorted);
    free(snapshot->models);
    free(snapshot);
}

UmiStatus UmiAiDiscoveryDecode(UmiAiDiscoveryFormat format, const void *bytes,
    size_t length, const UmiCancellationToken *cancel, UmiAiDiscoverySnapshot **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (bytes == NULL || length == 0U || format < UMI_AI_DISCOVERY_OPENAI_LIST ||
        format > UMI_AI_DISCOVERY_OLLAMA_LOADED) return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_AI_DISCOVERY_MAX_BYTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    const UmiJsonTreeLimits limits = {UMI_AI_DISCOVERY_MAX_BYTES, 65536U, 32U};
    UmiJsonTree *tree = NULL;
    UmiStatus status = UmiJsonTreeCreate(bytes, length, &limits, cancel, &tree);
    if (status != UMI_STATUS_OK) return status;
    UmiAiDiscoverySnapshot *snapshot = NULL;
    int list = -1;
    if (UmiJsonTreeKind(tree, 0) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT) {
        status = UMI_STATUS_PARSE_ERROR; goto done;
    }
    status = UmiJsonTreeValidateUniqueKeys(tree, 0, cancel);
    if (status != UMI_STATUS_OK) goto done;
    if (format == UMI_AI_DISCOVERY_OPENAI_LIST) {
        status = expected_text(tree, 0, "object", "list");
        if (status != UMI_STATUS_OK) goto done;
    }
    status = UmiJsonTreeMember(tree, 0, format == UMI_AI_DISCOVERY_OPENAI_LIST ? "data" : "models", &list);
    if (status == UMI_STATUS_NOT_FOUND) status = UMI_STATUS_PARSE_ERROR;
    if (status != UMI_STATUS_OK) goto done;
    if (UmiJsonTreeKind(tree, list) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY) {
        status = UMI_STATUS_PARSE_ERROR; goto done;
    }
    size_t count = UmiJsonTreeCount(tree, list);
    if (count > UMI_AI_DISCOVERY_MAX_MODELS) { status = UMI_STATUS_CAPACITY_EXCEEDED; goto done; }
    snapshot = calloc(1, sizeof(*snapshot));
    if (snapshot == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto done; }
    snapshot->format = format;
    snapshot->count = count;
    if (count != 0U) {
        snapshot->models = calloc(count, sizeof(*snapshot->models));
        snapshot->sorted = calloc(count, sizeof(*snapshot->sorted));
        if (snapshot->models == NULL || snapshot->sorted == NULL) {
            status = UMI_STATUS_OUT_OF_MEMORY; goto done;
        }
    }
    size_t index = 0U;
    for (int node = UmiJsonTreeFirst(tree, list); node >= 0; node = UmiJsonTreeNext(tree, node)) {
        if (umi_cancellation_token_is_requested(cancel)) { status = UMI_STATUS_CANCELLED; goto done; }
        if (index >= count) { status = UMI_STATUS_INVALID_STATE; goto done; }
        status = model_read(tree, node, format, &snapshot->models[index]);
        if (status != UMI_STATUS_OK) goto done;
        snapshot->sorted[index] = &snapshot->models[index];
        ++index;
    }
    if (index != count) { status = UMI_STATUS_INVALID_STATE; goto done; }
    /* Sorting only the lookup index preserves provider order for the UI.
     * It also detects duplicate decoded IDs without quadratic scanning. */
    if (count > 1U) qsort(snapshot->sorted, count, sizeof(*snapshot->sorted), compare_models);
    for (size_t i = 1U; i < count; ++i) {
        if (umi_cancellation_token_is_requested(cancel)) { status = UMI_STATUS_CANCELLED; goto done; }
        if (strcmp(snapshot->sorted[i - 1U]->id, snapshot->sorted[i]->id) == 0) {
            status = UMI_STATUS_ALREADY_EXISTS; goto done;
        }
    }
    if (umi_cancellation_token_is_requested(cancel)) { status = UMI_STATUS_CANCELLED; goto done; }
    *out = snapshot;
    snapshot = NULL;
done:
    UmiAiDiscoveryDestroy(snapshot);
    UmiJsonTreeDestroy(tree);
    return status;
}

size_t UmiAiDiscoveryCount(const UmiAiDiscoverySnapshot *snapshot)
{
    return snapshot == NULL ? 0U : snapshot->count;
}
UmiAiDiscoveryFormat UmiAiDiscoveryFormatOf(const UmiAiDiscoverySnapshot *snapshot)
{
    return snapshot == NULL ? (UmiAiDiscoveryFormat)0 : snapshot->format;
}
UmiStatus UmiAiDiscoveryAt(const UmiAiDiscoverySnapshot *snapshot, size_t index, UmiAiDiscoveredModel *out)
{
    if (snapshot == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= snapshot->count) return UMI_STATUS_NOT_FOUND;
    *out = snapshot->models[index];
    return UMI_STATUS_OK;
}
/* Stop at the terminator: callers need not provide a capacity-sized array. */
static int bounded_id(const char *id)
{
    if (id == NULL || id[0] == '\0') return 0;
    for (size_t i = 0; i < UMI_AI_DISCOVERY_ID_CAPACITY; ++i)
        if (id[i] == '\0') return 1;
    return 0;
}
UmiStatus UmiAiDiscoveryFind(const UmiAiDiscoverySnapshot *snapshot, const char *id, UmiAiDiscoveredModel *out)
{
    if (snapshot == NULL || out == NULL || !bounded_id(id)) return UMI_STATUS_INVALID_ARGUMENT;
    size_t low = 0U, high = snapshot->count;
    while (low < high) {
        const size_t middle = low + (high - low) / 2U;
        int order = strcmp(snapshot->sorted[middle]->id, id);
        if (order == 0) { *out = *snapshot->sorted[middle]; return UMI_STATUS_OK; }
        if (order < 0) low = middle + 1U;
        else high = middle;
    }
    return UMI_STATUS_NOT_FOUND;
}

/* Keep query results inside the same snapshot lifetime and ID lookup owner. */
#include "model_discovery_query.inc"
