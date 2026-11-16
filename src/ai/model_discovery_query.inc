/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/model_discovery_query.inc
 *
 * PURPOSE:
 *   Select and order owned model observations while preserving complete identities and unknown metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/base/utf8.h"
static int filter_text_valid(const char *text, size_t capacity)
{
    const char *end = memchr(text, '\0', capacity);
    return end != NULL && UmiUtf8Validate((const unsigned char *)text, (size_t)(end - text), NULL);
}
/* Model and node catalogue search now share the same deterministic ASCII-fold implementation. Preserve the former private helpers while retaining all existing model filter behavior. The former implementation is retained for owner review. */
#if 0
static unsigned char ascii_fold(unsigned char value)
{ return value >= 'A' && value <= 'Z' ? (unsigned char)(value + ('a' - 'A')) : value; }
static int contains_folded(const char *text, const char *needle)
{
    if (needle[0] == '\0') return 1;
    for (size_t start = 0; text[start] != '\0'; ++start) {
        size_t i = 0;
        while (needle[i] != '\0' && text[start + i] != '\0' &&
               ascii_fold((unsigned char)needle[i]) == ascii_fold((unsigned char)text[start + i])) ++i;
        if (needle[i] == '\0') return 1;
    }
    return 0;
}

#endif
#include "../base/ascii_search_internal.h"
static int contains_folded(const char *text,const char *needle)
{ return UmiAsciiSearchContains(text,needle); }
static int query_matches(const UmiAiDiscoveredModel *model, const UmiAiDiscoveryFilter *filter)
{
    if ((model->known & filter->required_known) != filter->required_known) return 0;
    if (filter->family[0] != '\0' && strcmp(filter->family, model->family) != 0) return 0;
    if (filter->format[0] != '\0' && strcmp(filter->format, model->format) != 0) return 0;
    if (filter->quantization[0] != '\0' && strcmp(filter->quantization, model->quantization) != 0) return 0;
    if (filter->minimum_context_tokens != 0 &&
        (!(model->known & UMI_AI_DISCOVERY_CONTEXT) || model->context_tokens < filter->minimum_context_tokens)) return 0;
    if (filter->size_limit_enabled &&
        (!(model->known & UMI_AI_DISCOVERY_SIZE) || model->size_bytes > filter->maximum_size_bytes)) return 0;
    return contains_folded(model->id, filter->search) || contains_folded(model->display_name, filter->search) ||
        contains_folded(model->family, filter->search);
}
static int query_order_id(const void *left, const void *right)
{ return strcmp(((const UmiAiDiscoveredModel *)left)->id, ((const UmiAiDiscoveredModel *)right)->id); }
static int query_order_name(const void *left, const void *right)
{
    const UmiAiDiscoveredModel *a = left, *b = right;
    int order = strcmp(a->display_name, b->display_name);
    return order != 0 ? order : strcmp(a->id, b->id);
}
static int query_order_size(const void *left, const void *right)
{
    const UmiAiDiscoveredModel *a = left, *b = right;
    int ak = (a->known & UMI_AI_DISCOVERY_SIZE) != 0, bk = (b->known & UMI_AI_DISCOVERY_SIZE) != 0;
    if (ak != bk) return ak ? -1 : 1;
    if (ak && a->size_bytes != b->size_bytes) return a->size_bytes < b->size_bytes ? -1 : 1;
    return strcmp(a->id, b->id);
}
static int query_order_context(const void *left, const void *right)
{
    const UmiAiDiscoveredModel *a = left, *b = right;
    int ak = (a->known & UMI_AI_DISCOVERY_CONTEXT) != 0, bk = (b->known & UMI_AI_DISCOVERY_CONTEXT) != 0;
    if (ak != bk) return ak ? -1 : 1;
    if (ak && a->context_tokens != b->context_tokens) return a->context_tokens > b->context_tokens ? -1 : 1;
    return strcmp(a->id, b->id);
}
UmiStatus UmiAiDiscoverySelect(const UmiAiDiscoverySnapshot *source,
    const UmiAiDiscoveryFilter *filter, const UmiCancellationToken *cancel,
    UmiAiDiscoverySnapshot **out)
{
    if (out == NULL || *out != NULL || source == NULL || filter == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    const uint32_t known = UMI_AI_DISCOVERY_SIZE | UMI_AI_DISCOVERY_VRAM | UMI_AI_DISCOVERY_CONTEXT;
    if (!filter_text_valid(filter->search, sizeof(filter->search)) ||
        !filter_text_valid(filter->family, sizeof(filter->family)) ||
        !filter_text_valid(filter->format, sizeof(filter->format)) ||
        !filter_text_valid(filter->quantization, sizeof(filter->quantization)) ||
        (filter->required_known & ~known) != 0 ||
        (filter->size_limit_enabled != 0 && filter->size_limit_enabled != 1) ||
        (!filter->size_limit_enabled && filter->maximum_size_bytes != 0) ||
        filter->order < UMI_AI_DISCOVERY_ORDER_SOURCE || filter->order > UMI_AI_DISCOVERY_ORDER_CONTEXT_DESC)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_cancellation_token_is_requested(cancel)) return UMI_STATUS_CANCELLED;
    size_t count = 0;
    for (size_t i = 0; i < source->count; ++i) {
        if (umi_cancellation_token_is_requested(cancel)) return UMI_STATUS_CANCELLED;
        if (query_matches(&source->models[i], filter)) ++count;
    }
    UmiAiDiscoverySnapshot *result = calloc(1U, sizeof(*result));
    if (result == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    result->format = source->format; result->count = count;
    UmiStatus status = UMI_STATUS_OK;
    if (count > 0) {
        result->models = calloc(count, sizeof(*result->models));
        result->sorted = calloc(count, sizeof(*result->sorted));
        if (result->models == NULL || result->sorted == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto done; }
    }
    size_t next = 0;
    for (size_t i = 0; i < source->count; ++i) {
        if (umi_cancellation_token_is_requested(cancel)) { status = UMI_STATUS_CANCELLED; goto done; }
        if (query_matches(&source->models[i], filter)) {
            if (next >= count) { status = UMI_STATUS_INVALID_STATE; goto done; }
            result->models[next++] = source->models[i];
        }
    }
    if (next != count) { status = UMI_STATUS_INVALID_STATE; goto done; }
    int (*compare)(const void *, const void *) = NULL;
    switch (filter->order) {
        case UMI_AI_DISCOVERY_ORDER_ID: compare = query_order_id; break;
        case UMI_AI_DISCOVERY_ORDER_NAME: compare = query_order_name; break;
        case UMI_AI_DISCOVERY_ORDER_SIZE_ASC: compare = query_order_size; break;
        case UMI_AI_DISCOVERY_ORDER_CONTEXT_DESC: compare = query_order_context; break;
        default: break;
    }
    if (compare != NULL && count > 1U) qsort(result->models, count, sizeof(*result->models), compare);
    /* Lookup still uses complete IDs after display ordering. Rebuild its index
     * against this independent owner's records, never against the source. */
    for (size_t i = 0; i < count; ++i) result->sorted[i] = &result->models[i];
    if (count > 1U) qsort(result->sorted, count, sizeof(*result->sorted), compare_models);
    if (umi_cancellation_token_is_requested(cancel)) { status = UMI_STATUS_CANCELLED; goto done; }
    *out = result; result = NULL;
done:
    UmiAiDiscoveryDestroy(result); return status;
}
