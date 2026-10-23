/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/explorer_storage.inc
 * PURPOSE: Own compact Test Explorer records, indexes and retained presentation state.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Included only by explorer_model.c. Compact string offsets keep sparse CTest
 * metadata from allocating every public fixed-size buffer for every row.
 * Public snapshots remain full independent values at the API boundary. */
#define EXPLORER_NONE SIZE_MAX
#define EXPLORER_TEXT_FIELDS(X) \
    X(id) X(parent_id) X(suite_id) X(name) X(uri) X(source_uri) \
    X(framework) X(kind) X(labels) X(command) X(working_directory)
#define EXPLORER_TEXT_ENUM(field) EXPLORER_TEXT_##field,
enum { EXPLORER_TEXT_FIELDS(EXPLORER_TEXT_ENUM) EXPLORER_TEXT_COUNT };
#undef EXPLORER_TEXT_ENUM

typedef struct ExplorerStateRecord {
    char item_id[128];
    int expanded;
    int selected;
} ExplorerStateRecord;

typedef struct ExplorerRecord {
    size_t text[EXPLORER_TEXT_COUNT];
    uint32_t struct_size, api_version, source_line, timeout_ms;
    int enabled, discovered, last_outcome;
    double last_duration_ms;
    uint64_t revision;
    size_t latest, parent, tree_parent, child_count, depth, visible_index;
    int expanded, selected, visible, direct_match, subtree_match;
} ExplorerRecord;

struct UmiTestExplorerModel {
    ExplorerRecord *records;
    size_t *order, *by_id, *visible;
    size_t row_count, visible_count;
    char *text;
    size_t text_size, text_capacity;
    UmiTestPlatformResultSnapshot *results;
    size_t result_count;
    ExplorerStateRecord *states;
    size_t state_count;
    UmiTestPlatformFilter filter;
    UmiTestExplorerSortMode sort_mode;
    UmiTestExplorerGroupMode group_mode;
    uint64_t item_revision, result_revision, revision;
};

static const char *ExplorerText(const UmiTestExplorerModel *model,
    size_t record, size_t field)
{
    return model->text + model->records[record].text[field];
}

static void ExplorerStorageFree(UmiTestExplorerModel *model)
{
    free(model->records); free(model->order); free(model->by_id);
    free(model->visible); free(model->text); free(model->results); free(model->states);
}

/* The public Framework filter validator now owns these bounds for every consumer. Retained for engineering review. */
#if 0
static int ExplorerFilterValid(const UmiTestPlatformFilter *filter)
{
    return filter != NULL && memchr(filter->text, 0, sizeof filter->text) != NULL &&
        memchr(filter->suite_id, 0, sizeof filter->suite_id) != NULL &&
        memchr(filter->label, 0, sizeof filter->label) != NULL;
}
#endif
static int ExplorerFilterValid(const UmiTestPlatformFilter *filter)
{
    return UmiTestPlatformFilterValidate(filter) == UMI_STATUS_OK;
}

static UmiStatus ExplorerAppendText(UmiTestExplorerModel *model,
    const char *text, size_t capacity, size_t *out_offset)
{
    const char *end = (const char *)memchr(text, 0, capacity);
    if (end == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (end == text) { *out_offset = 0U; return UMI_STATUS_OK; }
    const size_t length = (size_t)(end - text) + 1U;
    if (length > SIZE_MAX - model->text_size) return UMI_STATUS_CAPACITY_EXCEEDED;
    const size_t required = model->text_size + length;
    if (required > model->text_capacity) {
        size_t next = model->text_capacity;
        while (next < required) {
            if (next > SIZE_MAX / 2U) { next = required; break; }
            next *= 2U;
        }
        char *grown = (char *)realloc(model->text, next);
        if (grown == NULL) return UMI_STATUS_OUT_OF_MEMORY;
        model->text = grown;
        model->text_capacity = next;
    }
    *out_offset = model->text_size;
    memcpy(model->text + model->text_size, text, length);
    model->text_size = required;
    return UMI_STATUS_OK;
}

static UmiStatus ExplorerPack(UmiTestExplorerModel *model, size_t index,
    const UmiTestPlatformItemSnapshot *item)
{
    ExplorerRecord *record = &model->records[index];
    UmiStatus status;
#define EXPLORER_PACK_FIELD(field) \
    status = ExplorerAppendText(model, item->field, sizeof item->field, \
        &record->text[EXPLORER_TEXT_##field]); \
    if (status != UMI_STATUS_OK) return status;
    EXPLORER_TEXT_FIELDS(EXPLORER_PACK_FIELD)
#undef EXPLORER_PACK_FIELD
    record->struct_size = item->struct_size; record->api_version = item->api_version;
    record->source_line = item->source_line; record->timeout_ms = item->timeout_ms;
    record->enabled = item->enabled; record->discovered = item->discovered;
    record->last_outcome = item->last_outcome; record->last_duration_ms = item->last_duration_ms;
    record->revision = item->revision;
    record->latest = record->parent = record->tree_parent = EXPLORER_NONE;
    record->visible_index = EXPLORER_NONE;
    return UMI_STATUS_OK;
}

static void ExplorerUnpack(const UmiTestExplorerModel *model, size_t index,
    UmiTestPlatformItemSnapshot *item)
{
    const ExplorerRecord *record = &model->records[index];
    memset(item, 0, sizeof *item);
#define EXPLORER_UNPACK_FIELD(field) \
    memcpy(item->field, ExplorerText(model, index, EXPLORER_TEXT_##field), \
        strlen(ExplorerText(model, index, EXPLORER_TEXT_##field)) + 1U);
    EXPLORER_TEXT_FIELDS(EXPLORER_UNPACK_FIELD)
#undef EXPLORER_UNPACK_FIELD
    item->struct_size = record->struct_size; item->api_version = record->api_version;
    item->source_line = record->source_line; item->timeout_ms = record->timeout_ms;
    item->enabled = record->enabled; item->discovered = record->discovered;
    item->last_outcome = record->last_outcome; item->last_duration_ms = record->last_duration_ms;
    item->revision = record->revision;
}

static size_t ExplorerFind(const UmiTestExplorerModel *model, const char *id)
{
    size_t low = 0U, high = model->row_count;
    while (low < high) {
        const size_t middle = low + (high - low) / 2U;
        const size_t index = model->by_id[middle];
        const int order = strcmp(ExplorerText(model, index, EXPLORER_TEXT_id), id);
        if (order == 0) return index;
        if (order < 0) low = middle + 1U;
        else high = middle;
    }
    return EXPLORER_NONE;
}

static size_t ExplorerStateFind(const UmiTestExplorerModel *model, const char *id)
{
    size_t low = 0U, high = model->state_count;
    while (low < high) {
        const size_t middle = low + (high - low) / 2U;
        const int order = strcmp(model->states[middle].item_id, id);
        if (order == 0) return middle;
        if (order < 0) low = middle + 1U;
        else high = middle;
    }
    return EXPLORER_NONE;
}

static int ExplorerStateCompare(const void *a, const void *b)
{
    return strcmp(((const ExplorerStateRecord *)a)->item_id,
        ((const ExplorerStateRecord *)b)->item_id);
}

static int ExplorerCompareText(const char *left, const char *right)
{
    while (*left != '\0' && *right != '\0') {
        const int a = tolower((unsigned char)*left++);
        const int b = tolower((unsigned char)*right++);
        if (a != b) return a < b ? -1 : 1;
    }
    return *left == *right ? 0 : *left == '\0' ? -1 : 1;
}

static const UmiTestPlatformResultSnapshot *ExplorerResult(
    const UmiTestExplorerModel *model, size_t index)
{
    const size_t latest = model->records[index].latest;
    return latest == EXPLORER_NONE ? NULL : &model->results[latest];
}

static int ExplorerOutcomeRank(const UmiTestPlatformResultSnapshot *result)
{
    if (result == NULL) return 6;
    switch ((UmiTestPlatformOutcome)result->outcome) {
        case UMI_TEST_PLATFORM_OUTCOME_FAILED: return 0;
        case UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT: return 1;
        case UMI_TEST_PLATFORM_OUTCOME_CANCELLED: return 2;
        case UMI_TEST_PLATFORM_OUTCOME_SKIPPED: return 3;
        case UMI_TEST_PLATFORM_OUTCOME_PASSED: return 4;
        default: return 5;
    }
}

static int ExplorerCompare(const UmiTestExplorerModel *model,
    size_t left, size_t right, int by_id)
{
    if (by_id) return strcmp(ExplorerText(model, left, EXPLORER_TEXT_id),
        ExplorerText(model, right, EXPLORER_TEXT_id));
    const ExplorerRecord *a = &model->records[left], *b = &model->records[right];
    const UmiTestPlatformResultSnapshot *ar = ExplorerResult(model, left);
    const UmiTestPlatformResultSnapshot *br = ExplorerResult(model, right);
    const int ranks = ExplorerOutcomeRank(ar) - ExplorerOutcomeRank(br);
    int order = 0;
#define EXPLORER_COMPARE_FIELD(field) ExplorerCompareText( \
    ExplorerText(model, left, EXPLORER_TEXT_##field), ExplorerText(model, right, EXPLORER_TEXT_##field))
    if (model->group_mode == UMI_TEST_EXPLORER_GROUP_SUITE) order = EXPLORER_COMPARE_FIELD(suite_id);
    else if (model->group_mode == UMI_TEST_EXPLORER_GROUP_FRAMEWORK) order = EXPLORER_COMPARE_FIELD(framework);
    else if (model->group_mode == UMI_TEST_EXPLORER_GROUP_OUTCOME) order = ranks;
    if (order != 0) return order;
    switch (model->sort_mode) {
        case UMI_TEST_EXPLORER_SORT_SOURCE:
            order = EXPLORER_COMPARE_FIELD(source_uri);
            if (order == 0) order = a->source_line < b->source_line ? -1 : a->source_line > b->source_line;
            break;
        case UMI_TEST_EXPLORER_SORT_OUTCOME: order = ranks; break;
        case UMI_TEST_EXPLORER_SORT_DURATION: {
            const double ad = ar == NULL ? 0.0 : ar->duration_ms;
            const double bd = br == NULL ? 0.0 : br->duration_ms;
            order = ad > bd ? -1 : ad < bd;
            break;
        }
        case UMI_TEST_EXPLORER_SORT_FRAMEWORK: order = EXPLORER_COMPARE_FIELD(framework); break;
        default: break;
    }
    if (order == 0) order = EXPLORER_COMPARE_FIELD(name);
#undef EXPLORER_COMPARE_FIELD
    if (order == 0) order = strcmp(ExplorerText(model, left, EXPLORER_TEXT_id),
        ExplorerText(model, right, EXPLORER_TEXT_id));
    return order;
}

/* Bottom-up mergesort uses caller-owned scratch and no global comparator
 * context. Hierarchy depth cannot increase the C call stack. */
static void ExplorerSort(const UmiTestExplorerModel *model,
    size_t *indices, size_t *scratch, size_t count, int by_id)
{
    for (size_t width = 1U; width < count; width *= 2U) {
        for (size_t begin = 0U; begin < count; begin += width * 2U) {
            const size_t middle = begin + width < count ? begin + width : count;
            const size_t end = middle + width < count ? middle + width : count;
            size_t a = begin, b = middle, write = begin;
            while (a < middle || b < end) {
                if (b == end || (a < middle && ExplorerCompare(model, indices[a], indices[b], by_id) <= 0))
                    scratch[write++] = indices[a++];
                else scratch[write++] = indices[b++];
            }
        }
        memcpy(indices, scratch, count * sizeof *indices);
    }
}

static void ExplorerReadRow(const UmiTestExplorerModel *model, size_t index,
    UmiTestExplorerRow *row)
{
    const ExplorerRecord *record = &model->records[index];
    const UmiTestPlatformResultSnapshot *result = ExplorerResult(model, index);
    memset(row, 0, sizeof *row);
    row->struct_size = (uint32_t)sizeof *row;
    row->api_version = UMI_TEST_EXPLORER_MODEL_API_VERSION;
    ExplorerUnpack(model, index, &row->item);
    if (result != NULL) row->latest_result = *result;
    row->has_result = result != NULL;
    row->depth = record->depth; row->child_count = record->child_count;
    row->visible_index = record->visible_index;
    row->expanded = record->expanded; row->selected = record->selected;
    row->visible = record->visible; row->matches_filter = record->direct_match;
    row->descendant_matches_filter = record->subtree_match && !record->direct_match;
}

static void ExplorerVisibility(UmiTestExplorerModel *model)
{
    model->visible_count = 0U;
    for (size_t row = 0U; row < model->row_count; ++row) {
        const size_t index = model->order[row];
        ExplorerRecord *record = &model->records[index];
        int visible = record->subtree_match;
        if (record->tree_parent != EXPLORER_NONE) {
            const ExplorerRecord *parent = &model->records[record->tree_parent];
            visible = visible && parent->visible && parent->expanded;
        }
        record->visible = visible;
        record->visible_index = EXPLORER_NONE;
        if (visible) {
            record->visible_index = model->visible_count;
            model->visible[model->visible_count++] = index;
        }
    }
}
