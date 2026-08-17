/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/explorer_model.c
 *
 * PURPOSE:
 *   Implement the professional Test Explorer projection. Canonical registry
 *   records remain authoritative; this module owns only reusable presentation
 *   state such as expansion, selection, filtering and deterministic ordering.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/explorer_model.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define EXPLORER_ID_CAPACITY 128U

typedef struct ExplorerStateRecord {
    char item_id[EXPLORER_ID_CAPACITY];
    int expanded;
    int selected;
} ExplorerStateRecord;

typedef struct ExplorerSourceEntry {
    UmiTestPlatformItemSnapshot item;
    UmiTestPlatformResultSnapshot latest_result;
    size_t child_count;
    int has_result;
    int direct_match;
    int subtree_match;
    int visiting;
    int visited;
} ExplorerSourceEntry;

struct UmiTestExplorerModel {
    UmiTestExplorerRow *rows;
    size_t row_count;
    size_t visible_count;
    ExplorerStateRecord *states;
    size_t state_count;
    UmiTestPlatformFilter filter;
    UmiTestExplorerSortMode sort_mode;
    UmiTestExplorerGroupMode group_mode;
    uint64_t item_revision;
    uint64_t result_revision;
    uint64_t revision;
};

static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U) return;
    if (source == NULL) source = "";
    length = strlen(source);
    if (length >= capacity) length = capacity - 1U;
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

static int compare_text_insensitive(const char *left, const char *right)
{
    size_t index = 0U;
    if (left == right) return 0;
    if (left == NULL) return -1;
    if (right == NULL) return 1;
    while (left[index] != '\0' && right[index] != '\0') {
        const int left_value = tolower((unsigned char)left[index]);
        const int right_value = tolower((unsigned char)right[index]);
        if (left_value < right_value) return -1;
        if (left_value > right_value) return 1;
        ++index;
    }
    if (left[index] == '\0' && right[index] == '\0') return 0;
    return left[index] == '\0' ? -1 : 1;
}

static ExplorerStateRecord *find_state(UmiTestExplorerModel *model,
                                       const char *item_id)
{
    size_t index;
    if (model == NULL || item_id == NULL) return NULL;
    for (index = 0U; index < model->state_count; ++index) {
        if (strcmp(model->states[index].item_id, item_id) == 0) {
            return &model->states[index];
        }
    }
    return NULL;
}

static ExplorerStateRecord *ensure_state(UmiTestExplorerModel *model,
                                         const char *item_id,
                                         int default_expanded)
{
    ExplorerStateRecord *record = find_state(model, item_id);
    if (record != NULL) return record;
    if (model == NULL || item_id == NULL || item_id[0] == '\0' ||
        model->state_count >= UMI_TEST_EXPLORER_MODEL_CAPACITY) {
        return NULL;
    }
    record = &model->states[model->state_count++];
    (void)memset(record, 0, sizeof(*record));
    copy_text(record->item_id, sizeof(record->item_id), item_id);
    record->expanded = default_expanded != 0;
    return record;
}

static UmiStatus find_latest_result(
    const UmiTestPlatformResultRegistry *results,
    const char *item_id,
    UmiTestPlatformResultSnapshot *out_result,
    int *out_found)
{
    size_t index;
    size_t count;
    uint64_t best_sequence = 0U;
    uint64_t best_revision = 0U;
    int found = 0;

    if (results == NULL || item_id == NULL || out_result == NULL ||
        out_found == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_result, 0, sizeof(*out_result));
    count = umi_test_platform_result_registry_count(results);
    for (index = 0U; index < count; ++index) {
        UmiTestPlatformResultSnapshot candidate;
        UmiStatus status = umi_test_platform_result_registry_at(
            results, index, &candidate);
        if (status != UMI_STATUS_OK) return status;
        if (strcmp(candidate.item_id, item_id) != 0) continue;
        if (!found || candidate.sequence > best_sequence ||
            (candidate.sequence == best_sequence &&
             candidate.revision > best_revision)) {
            *out_result = candidate;
            best_sequence = candidate.sequence;
            best_revision = candidate.revision;
            found = 1;
        }
    }
    *out_found = found;
    return UMI_STATUS_OK;
}

static size_t find_source_index(const ExplorerSourceEntry *entries,
                                size_t count,
                                const char *item_id)
{
    size_t index;
    if (entries == NULL || item_id == NULL || item_id[0] == '\0') {
        return (size_t)-1;
    }
    for (index = 0U; index < count; ++index) {
        if (strcmp(entries[index].item.id, item_id) == 0) return index;
    }
    return (size_t)-1;
}

static int subtree_matches(ExplorerSourceEntry *entries,
                           size_t count,
                           size_t index)
{
    size_t child;
    int match;
    if (entries[index].visited) return entries[index].subtree_match;
    if (entries[index].visiting) {
        /* A malformed provider hierarchy must not recurse forever. The cyclic
         * edge is ignored while the directly matching item remains visible. */
        return entries[index].direct_match;
    }
    entries[index].visiting = 1;
    match = entries[index].direct_match;
    for (child = 0U; child < count; ++child) {
        if (strcmp(entries[child].item.parent_id, entries[index].item.id) == 0) {
            if (subtree_matches(entries, count, child)) match = 1;
        }
    }
    entries[index].visiting = 0;
    entries[index].visited = 1;
    entries[index].subtree_match = match;
    return match;
}

static int outcome_rank(const ExplorerSourceEntry *entry)
{
    if (!entry->has_result) return 6;
    switch ((UmiTestPlatformOutcome)entry->latest_result.outcome) {
        case UMI_TEST_PLATFORM_OUTCOME_FAILED: return 0;
        case UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT: return 1;
        case UMI_TEST_PLATFORM_OUTCOME_CANCELLED: return 2;
        case UMI_TEST_PLATFORM_OUTCOME_SKIPPED: return 3;
        case UMI_TEST_PLATFORM_OUTCOME_PASSED: return 4;
        case UMI_TEST_PLATFORM_OUTCOME_NOT_RUN:
        default: return 5;
    }
}

static int compare_entries(const UmiTestExplorerModel *model,
                           const ExplorerSourceEntry *left,
                           const ExplorerSourceEntry *right)
{
    int order = 0;
    if (model->group_mode == UMI_TEST_EXPLORER_GROUP_SUITE) {
        order = compare_text_insensitive(left->item.suite_id, right->item.suite_id);
    } else if (model->group_mode == UMI_TEST_EXPLORER_GROUP_FRAMEWORK) {
        order = compare_text_insensitive(left->item.framework,
                                         right->item.framework);
    } else if (model->group_mode == UMI_TEST_EXPLORER_GROUP_OUTCOME) {
        const int left_rank = outcome_rank(left);
        const int right_rank = outcome_rank(right);
        if (left_rank < right_rank) order = -1;
        else if (left_rank > right_rank) order = 1;
    }
    if (order != 0) return order;

    switch (model->sort_mode) {
        case UMI_TEST_EXPLORER_SORT_SOURCE:
            order = compare_text_insensitive(left->item.source_uri,
                                             right->item.source_uri);
            if (order == 0) {
                if (left->item.source_line < right->item.source_line) order = -1;
                else if (left->item.source_line > right->item.source_line) order = 1;
            }
            break;
        case UMI_TEST_EXPLORER_SORT_OUTCOME: {
            const int left_rank = outcome_rank(left);
            const int right_rank = outcome_rank(right);
            if (left_rank < right_rank) order = -1;
            else if (left_rank > right_rank) order = 1;
            break;
        }
        case UMI_TEST_EXPLORER_SORT_DURATION:
            if (left->latest_result.duration_ms > right->latest_result.duration_ms) {
                order = -1;
            } else if (left->latest_result.duration_ms <
                       right->latest_result.duration_ms) {
                order = 1;
            }
            break;
        case UMI_TEST_EXPLORER_SORT_FRAMEWORK:
            order = compare_text_insensitive(left->item.framework,
                                             right->item.framework);
            break;
        case UMI_TEST_EXPLORER_SORT_NAME:
        default:
            break;
    }
    if (order == 0) order = compare_text_insensitive(left->item.name,
                                                     right->item.name);
    if (order == 0) order = strcmp(left->item.id, right->item.id);
    return order;
}

static void sort_indices(const UmiTestExplorerModel *model,
                         const ExplorerSourceEntry *entries,
                         size_t *indices,
                         size_t count)
{
    size_t index;
    for (index = 1U; index < count; ++index) {
        const size_t value = indices[index];
        size_t position = index;
        while (position > 0U &&
               compare_entries(model, &entries[value],
                               &entries[indices[position - 1U]]) < 0) {
            indices[position] = indices[position - 1U];
            --position;
        }
        indices[position] = value;
    }
}

static UmiStatus emit_entry(UmiTestExplorerModel *model,
                            ExplorerSourceEntry *entries,
                            size_t count,
                            size_t source_index,
                            size_t depth,
                            int ancestors_visible,
                            unsigned char *emitted)
{
    size_t child_indices[UMI_TEST_EXPLORER_MODEL_CAPACITY] = {0U};
    size_t child_count = 0U;
    size_t child;
    ExplorerStateRecord *state;
    UmiTestExplorerRow *row;
    int visible;

    if (source_index >= count || emitted[source_index] != 0U) {
        return UMI_STATUS_OK;
    }
    if (model->row_count >= UMI_TEST_EXPLORER_MODEL_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    emitted[source_index] = 1U;
    state = ensure_state(model, entries[source_index].item.id,
                         entries[source_index].child_count > 0U);
    if (state == NULL) return UMI_STATUS_CAPACITY_EXCEEDED;

    row = &model->rows[model->row_count++];
    (void)memset(row, 0, sizeof(*row));
    row->struct_size = (uint32_t)sizeof(*row);
    row->api_version = UMI_TEST_EXPLORER_MODEL_API_VERSION;
    row->item = entries[source_index].item;
    row->latest_result = entries[source_index].latest_result;
    row->depth = depth;
    row->child_count = entries[source_index].child_count;
    row->has_result = entries[source_index].has_result;
    row->expanded = state->expanded;
    row->selected = state->selected;
    row->matches_filter = entries[source_index].direct_match;
    row->descendant_matches_filter =
        entries[source_index].subtree_match && !entries[source_index].direct_match;
    visible = ancestors_visible && entries[source_index].subtree_match;
    row->visible = visible;
    if (visible) row->visible_index = model->visible_count++;
    else row->visible_index = (size_t)-1;

    for (child = 0U; child < count; ++child) {
        if (strcmp(entries[child].item.parent_id,
                   entries[source_index].item.id) == 0 &&
            child_count < UMI_TEST_EXPLORER_MODEL_CAPACITY) {
            child_indices[child_count++] = child;
        }
    }
    sort_indices(model, entries, child_indices, child_count);
    for (child = 0U; child < child_count; ++child) {
        UmiStatus status = emit_entry(
            model, entries, count, child_indices[child], depth + 1U,
            visible && state->expanded, emitted);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_test_explorer_model_create(UmiTestExplorerModel **out_model)
{
    UmiTestExplorerModel *model;
    if (out_model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_model = NULL;
    model = (UmiTestExplorerModel *)calloc(1U, sizeof(*model));
    if (model == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    model->rows = (UmiTestExplorerRow *)calloc(
        UMI_TEST_EXPLORER_MODEL_CAPACITY, sizeof(model->rows[0]));
    model->states = (ExplorerStateRecord *)calloc(
        UMI_TEST_EXPLORER_MODEL_CAPACITY, sizeof(model->states[0]));
    if (model->rows == NULL || model->states == NULL) {
        umi_test_explorer_model_destroy(model);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    umi_test_platform_filter_init(&model->filter);
    model->sort_mode = UMI_TEST_EXPLORER_SORT_NAME;
    model->group_mode = UMI_TEST_EXPLORER_GROUP_HIERARCHY;
    model->revision = 1U;
    *out_model = model;
    return UMI_STATUS_OK;
}

void umi_test_explorer_model_destroy(UmiTestExplorerModel *model)
{
    if (model == NULL) return;
    free(model->rows);
    free(model->states);
    free(model);
}

UmiStatus umi_test_explorer_model_set_filter(
    UmiTestExplorerModel *model,
    const UmiTestPlatformFilter *filter)
{
    if (model == NULL || filter == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    model->filter = *filter;
    ++model->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_test_explorer_model_set_sort(
    UmiTestExplorerModel *model,
    UmiTestExplorerSortMode sort_mode)
{
    if (model == NULL || sort_mode < UMI_TEST_EXPLORER_SORT_NAME ||
        sort_mode > UMI_TEST_EXPLORER_SORT_FRAMEWORK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    model->sort_mode = sort_mode;
    ++model->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_test_explorer_model_set_group(
    UmiTestExplorerModel *model,
    UmiTestExplorerGroupMode group_mode)
{
    if (model == NULL || group_mode < UMI_TEST_EXPLORER_GROUP_HIERARCHY ||
        group_mode > UMI_TEST_EXPLORER_GROUP_OUTCOME) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    model->group_mode = group_mode;
    ++model->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_test_explorer_model_rebuild(
    UmiTestExplorerModel *model,
    const UmiTestPlatformItemRegistry *items,
    const UmiTestPlatformResultRegistry *results)
{
    ExplorerSourceEntry *entries;
    unsigned char *emitted;
    size_t root_indices[UMI_TEST_EXPLORER_MODEL_CAPACITY] = {0U};
    size_t count;
    size_t index;
    size_t root_count = 0U;
    UmiStatus status = UMI_STATUS_OK;

    if (model == NULL || items == NULL || results == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    count = umi_test_platform_item_registry_count(items);
    if (count > UMI_TEST_EXPLORER_MODEL_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    entries = (ExplorerSourceEntry *)calloc(count > 0U ? count : 1U,
                                             sizeof(entries[0]));
    emitted = (unsigned char *)calloc(count > 0U ? count : 1U,
                                       sizeof(emitted[0]));
    if (entries == NULL || emitted == NULL) {
        free(entries);
        free(emitted);
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    model->row_count = 0U;
    model->visible_count = 0U;
    for (index = 0U; index < count; ++index) {
        status = umi_test_platform_item_registry_at(items, index,
                                                     &entries[index].item);
        if (status != UMI_STATUS_OK) break;
        status = find_latest_result(results, entries[index].item.id,
                                    &entries[index].latest_result,
                                    &entries[index].has_result);
        if (status != UMI_STATUS_OK) break;
        entries[index].direct_match = umi_test_platform_filter_matches(
            &entries[index].item,
            entries[index].has_result ? &entries[index].latest_result : NULL,
            &model->filter);
    }
    if (status == UMI_STATUS_OK) {
        for (index = 0U; index < count; ++index) {
            size_t child;
            for (child = 0U; child < count; ++child) {
                if (strcmp(entries[child].item.parent_id,
                           entries[index].item.id) == 0) {
                    ++entries[index].child_count;
                }
            }
            (void)subtree_matches(entries, count, index);
            if (entries[index].item.parent_id[0] == '\0' ||
                find_source_index(entries, count,
                                  entries[index].item.parent_id) == (size_t)-1) {
                root_indices[root_count++] = index;
            }
        }
        sort_indices(model, entries, root_indices, root_count);
        for (index = 0U; index < root_count && status == UMI_STATUS_OK; ++index) {
            status = emit_entry(model, entries, count, root_indices[index], 0U,
                                1, emitted);
        }
        /* Provider defects can produce cycles without roots. Emit remaining
         * records as deterministic roots so no discovered test disappears. */
        for (index = 0U; index < count && status == UMI_STATUS_OK; ++index) {
            if (emitted[index] == 0U) {
                status = emit_entry(model, entries, count, index, 0U, 1,
                                    emitted);
            }
        }
    }

    if (status == UMI_STATUS_OK) {
        model->item_revision = umi_test_platform_item_registry_revision(items);
        model->result_revision = umi_test_platform_result_registry_revision(results);
        ++model->revision;
    }
    free(entries);
    free(emitted);
    return status;
}

static void recompute_visibility(UmiTestExplorerModel *model)
{
    size_t index;
    int ancestor_visible[UMI_TEST_EXPLORER_MODEL_CAPACITY] = {0};
    int ancestor_expanded[UMI_TEST_EXPLORER_MODEL_CAPACITY] = {0};

    model->visible_count = 0U;
    for (index = 0U; index < model->row_count; ++index) {
        UmiTestExplorerRow *row = &model->rows[index];
        int visible = row->matches_filter || row->descendant_matches_filter;
        if (row->depth > 0U) {
            const size_t parent_depth = row->depth - 1U;
            visible = visible && ancestor_visible[parent_depth] &&
                      ancestor_expanded[parent_depth];
        }
        row->visible = visible;
        if (visible) row->visible_index = model->visible_count++;
        else row->visible_index = (size_t)-1;
        ancestor_visible[row->depth] = visible;
        ancestor_expanded[row->depth] = row->expanded;
    }
}

UmiStatus umi_test_explorer_model_set_expanded(
    UmiTestExplorerModel *model,
    const char *item_id,
    int expanded)
{
    size_t index;
    ExplorerStateRecord *state;
    if (model == NULL || item_id == NULL || item_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    state = ensure_state(model, item_id, expanded);
    if (state == NULL) return UMI_STATUS_CAPACITY_EXCEEDED;
    state->expanded = expanded != 0;
    for (index = 0U; index < model->row_count; ++index) {
        if (strcmp(model->rows[index].item.id, item_id) == 0) {
            model->rows[index].expanded = state->expanded;
            recompute_visibility(model);
            ++model->revision;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_test_explorer_model_expand_all(UmiTestExplorerModel *model)
{
    size_t index;
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < model->row_count; ++index) {
        ExplorerStateRecord *state = ensure_state(
            model, model->rows[index].item.id, 1);
        if (state == NULL) return UMI_STATUS_CAPACITY_EXCEEDED;
        state->expanded = 1;
        model->rows[index].expanded = 1;
    }
    recompute_visibility(model);
    ++model->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_test_explorer_model_collapse_all(UmiTestExplorerModel *model)
{
    size_t index;
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < model->row_count; ++index) {
        ExplorerStateRecord *state = ensure_state(
            model, model->rows[index].item.id, 0);
        if (state == NULL) return UMI_STATUS_CAPACITY_EXCEEDED;
        state->expanded = 0;
        model->rows[index].expanded = 0;
    }
    recompute_visibility(model);
    ++model->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_test_explorer_model_select(
    UmiTestExplorerModel *model,
    const char *item_id,
    int selected,
    int exclusive)
{
    size_t index;
    int found = 0;
    if (model == NULL || item_id == NULL || item_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (exclusive) (void)umi_test_explorer_model_clear_selection(model);
    for (index = 0U; index < model->row_count; ++index) {
        if (strcmp(model->rows[index].item.id, item_id) == 0) {
            ExplorerStateRecord *state = ensure_state(
                model, item_id, model->rows[index].expanded);
            if (state == NULL) return UMI_STATUS_CAPACITY_EXCEEDED;
            state->selected = selected != 0;
            model->rows[index].selected = state->selected;
            found = 1;
            break;
        }
    }
    if (!found) return UMI_STATUS_NOT_FOUND;
    ++model->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_test_explorer_model_clear_selection(UmiTestExplorerModel *model)
{
    size_t index;
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < model->state_count; ++index) {
        model->states[index].selected = 0;
    }
    for (index = 0U; index < model->row_count; ++index) {
        model->rows[index].selected = 0;
    }
    ++model->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_test_explorer_model_selection(
    const UmiTestExplorerModel *model,
    UmiTestPlatformSelection *out_selection)
{
    size_t index;
    if (model == NULL || out_selection == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_selection, 0, sizeof(*out_selection));
    for (index = 0U; index < model->row_count; ++index) {
        if (!model->rows[index].selected) continue;
        if (out_selection->count >= UMI_TEST_PLATFORM_SELECTION_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        copy_text(out_selection->item_ids[out_selection->count],
                  sizeof(out_selection->item_ids[out_selection->count]),
                  model->rows[index].item.id);
        ++out_selection->count;
    }
    out_selection->source_revision = model->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_test_explorer_model_row_at(
    const UmiTestExplorerModel *model,
    size_t position,
    UmiTestExplorerRow *out_row)
{
    if (model == NULL || out_row == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (position >= model->row_count) return UMI_STATUS_NOT_FOUND;
    *out_row = model->rows[position];
    return UMI_STATUS_OK;
}

UmiStatus umi_test_explorer_model_visible_row_at(
    const UmiTestExplorerModel *model,
    size_t visible_position,
    UmiTestExplorerRow *out_row)
{
    size_t index;
    if (model == NULL || out_row == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < model->row_count; ++index) {
        if (model->rows[index].visible &&
            model->rows[index].visible_index == visible_position) {
            *out_row = model->rows[index];
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_test_explorer_model_snapshot(
    const UmiTestExplorerModel *model,
    UmiTestExplorerModelSnapshot *out_snapshot)
{
    size_t index;
    if (model == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_TEST_EXPLORER_MODEL_API_VERSION;
    out_snapshot->sort_mode = model->sort_mode;
    out_snapshot->group_mode = model->group_mode;
    out_snapshot->filter = model->filter;
    out_snapshot->row_count = model->row_count;
    out_snapshot->visible_row_count = model->visible_count;
    out_snapshot->item_revision = model->item_revision;
    out_snapshot->result_revision = model->result_revision;
    out_snapshot->revision = model->revision;

    for (index = 0U; index < model->row_count; ++index) {
        const UmiTestExplorerRow *row = &model->rows[index];
        if (row->depth == 0U) ++out_snapshot->root_count;
        if (row->child_count == 0U) ++out_snapshot->leaf_count;
        if (row->selected) ++out_snapshot->selected_count;
        if (!row->has_result) {
            ++out_snapshot->not_run_count;
        } else {
            switch ((UmiTestPlatformOutcome)row->latest_result.outcome) {
                case UMI_TEST_PLATFORM_OUTCOME_PASSED:
                    ++out_snapshot->passed_count;
                    break;
                case UMI_TEST_PLATFORM_OUTCOME_FAILED:
                case UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT:
                    ++out_snapshot->failed_count;
                    break;
                case UMI_TEST_PLATFORM_OUTCOME_SKIPPED:
                    ++out_snapshot->skipped_count;
                    break;
                case UMI_TEST_PLATFORM_OUTCOME_CANCELLED:
                case UMI_TEST_PLATFORM_OUTCOME_NOT_RUN:
                default:
                    ++out_snapshot->not_run_count;
                    break;
            }
        }
    }
    return UMI_STATUS_OK;
}

size_t umi_test_explorer_model_count(const UmiTestExplorerModel *model)
{
    return model != NULL ? model->row_count : 0U;
}

size_t umi_test_explorer_model_visible_count(const UmiTestExplorerModel *model)
{
    return model != NULL ? model->visible_count : 0U;
}

uint64_t umi_test_explorer_model_revision(const UmiTestExplorerModel *model)
{
    return model != NULL ? model->revision : 0U;
}
