/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostics/problem_model.c
 *
 * PURPOSE:
 *   Implement the canonical toolkit-neutral Problems model, deterministic
 *   provider-run reconciliation, selection and bounded filtered queries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostics/problem_model.h"

#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#define PROBLEM_INITIAL_CAPACITY 64U
#define PROBLEM_MAX_CAPACITY 65536U
#define GROUP_HASH_OFFSET UINT64_C(14695981039346656037)
#define GROUP_HASH_PRIME UINT64_C(1099511628211)

struct UmiDiagnosticProblemModel {
    UmiDiagnosticProblem *items;
    size_t count;
    size_t capacity;
    uint64_t revision;
};

typedef struct ProblemSortEntry {
    UmiDiagnosticProblem problem;
    UmiDiagnosticProblemSort sort;
} ProblemSortEntry;

static uint64_t next_revision(uint64_t value)
{
    return value == UINT64_MAX ? 1U : value + 1U;
}

static size_t find_fingerprint(const UmiDiagnosticProblemModel *model,
                               uint64_t fingerprint)
{
    size_t position;
    if (model == NULL || fingerprint == 0U) return SIZE_MAX;
    for (position = 0U; position < model->count; ++position) {
        if (model->items[position].finding.fingerprint == fingerprint) {
            return position;
        }
    }
    return SIZE_MAX;
}

static UmiStatus reserve_problems(UmiDiagnosticProblemModel *model,
                                  size_t required)
{
    size_t capacity;
    UmiDiagnosticProblem *replacement;
    if (required <= model->capacity) return UMI_STATUS_OK;
    capacity = model->capacity > 0U ? model->capacity : PROBLEM_INITIAL_CAPACITY;
    while (capacity < required) {
        if (capacity >= PROBLEM_MAX_CAPACITY || capacity > SIZE_MAX / 2U) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        capacity *= 2U;
        if (capacity > PROBLEM_MAX_CAPACITY) capacity = PROBLEM_MAX_CAPACITY;
    }
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiDiagnosticProblem *)realloc(
        model->items, capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    model->items = replacement;
    model->capacity = capacity;
    return UMI_STATUS_OK;
}

static UmiStatus validate_problem(const UmiDiagnosticProblem *problem)
{
    if (problem == NULL ||
        problem->struct_size != (uint32_t)sizeof(*problem) ||
        problem->api_version != UMI_DIAGNOSTIC_PROBLEM_MODEL_API_VERSION ||
        umi_diagnostic_provider_finding_validate(&problem->finding) !=
            UMI_STATUS_OK ||
        problem->original_severity < UMI_DIAGNOSTIC_TRACE ||
        problem->original_severity > UMI_DIAGNOSTIC_FATAL ||
        problem->effective_severity < UMI_DIAGNOSTIC_TRACE ||
        problem->effective_severity > UMI_DIAGNOSTIC_FATAL ||
        problem->baseline_state < UMI_DIAGNOSTIC_BASELINE_NONE ||
        problem->baseline_state > UMI_DIAGNOSTIC_BASELINE_ABSENT ||
        memchr(problem->severity_rule_id, '\0',
               sizeof(problem->severity_rule_id)) == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

static int text_contains_case_insensitive(const char *haystack,
                                          const char *needle)
{
    size_t haystack_length;
    size_t needle_length;
    size_t start;
    size_t offset;
    if (needle == NULL || needle[0] == '\0') return 1;
    if (haystack == NULL) return 0;
    haystack_length = strlen(haystack);
    needle_length = strlen(needle);
    if (needle_length > haystack_length) return 0;
    for (start = 0U; start + needle_length <= haystack_length; ++start) {
        int match = 1;
        for (offset = 0U; offset < needle_length; ++offset) {
            unsigned char left = (unsigned char)haystack[start + offset];
            unsigned char right = (unsigned char)needle[offset];
            if (tolower(left) != tolower(right)) {
                match = 0;
                break;
            }
        }
        if (match) return 1;
    }
    return 0;
}

static int problem_matches(const UmiDiagnosticProblem *problem,
                           const UmiDiagnosticProblemQuery *query)
{
    uint32_t severity_bit = UINT32_C(1) <<
        (uint32_t)problem->effective_severity;
    uint32_t kind_bit = UINT32_C(1) <<
        (uint32_t)problem->finding.diagnostic.kind;
    uint32_t baseline_bit = UINT32_C(1) <<
        (uint32_t)problem->baseline_state;
    if (query->severity_mask != 0U &&
        (query->severity_mask & severity_bit) == 0U) return 0;
    if (query->kind_mask != 0U && (query->kind_mask & kind_bit) == 0U) return 0;
    if (query->baseline_mask != 0U &&
        (query->baseline_mask & baseline_bit) == 0U) return 0;
    if (!query->include_suppressed && problem->suppression.suppressed) return 0;
    if (!query->include_resolved && !problem->active) return 0;
    if (!query->include_disabled && problem->policy_disabled) return 0;
    if (query->only_fixable && !problem->fixable) return 0;
    if (query->only_selected && !problem->selected) return 0;
    if (query->provider[0] != '\0' &&
        strcmp(query->provider, problem->finding.provider_id) != 0) return 0;
    if (query->source[0] != '\0' &&
        strcmp(query->source, problem->finding.diagnostic.source) != 0) return 0;
    if (query->code[0] != '\0' &&
        strcmp(query->code, problem->finding.diagnostic.code) != 0) return 0;
    if (query->uri[0] != '\0' &&
        strcmp(query->uri, problem->finding.diagnostic.uri) != 0) return 0;
    if (query->text[0] != '\0' &&
        !text_contains_case_insensitive(problem->finding.diagnostic.message,
                                        query->text) &&
        !text_contains_case_insensitive(problem->finding.diagnostic.detail,
                                        query->text) &&
        !text_contains_case_insensitive(problem->finding.diagnostic.code,
                                        query->text) &&
        !text_contains_case_insensitive(problem->finding.diagnostic.source,
                                        query->text) &&
        !text_contains_case_insensitive(problem->finding.provider_id,
                                        query->text)) {
        return 0;
    }
    return 1;
}

static int compare_location(const UmiDiagnosticProblem *left,
                            const UmiDiagnosticProblem *right)
{
    int order = strcmp(left->finding.diagnostic.uri,
                       right->finding.diagnostic.uri);
    if (order != 0) return order;
    if (left->finding.diagnostic.line < right->finding.diagnostic.line) return -1;
    if (left->finding.diagnostic.line > right->finding.diagnostic.line) return 1;
    if (left->finding.diagnostic.column < right->finding.diagnostic.column) return -1;
    if (left->finding.diagnostic.column > right->finding.diagnostic.column) return 1;
    return strcmp(left->finding.diagnostic.id, right->finding.diagnostic.id);
}

static int baseline_rank(UmiDiagnosticBaselineState state)
{
    switch (state) {
        case UMI_DIAGNOSTIC_BASELINE_NEW: return 0;
        case UMI_DIAGNOSTIC_BASELINE_UPDATED: return 1;
        case UMI_DIAGNOSTIC_BASELINE_UNCHANGED: return 2;
        case UMI_DIAGNOSTIC_BASELINE_ABSENT: return 3;
        default: return 4;
    }
}

static int compare_sort_entries(const void *left_pointer,
                                const void *right_pointer)
{
    const ProblemSortEntry *left = (const ProblemSortEntry *)left_pointer;
    const ProblemSortEntry *right = (const ProblemSortEntry *)right_pointer;
    int order;
    switch (left->sort) {
        case UMI_DIAGNOSTIC_PROBLEM_SORT_SEVERITY:
            if (left->problem.effective_severity !=
                right->problem.effective_severity) {
                return left->problem.effective_severity >
                       right->problem.effective_severity ? -1 : 1;
            }
            break;
        case UMI_DIAGNOSTIC_PROBLEM_SORT_LOCATION:
            return compare_location(&left->problem, &right->problem);
        case UMI_DIAGNOSTIC_PROBLEM_SORT_PROVIDER:
            order = strcmp(left->problem.finding.provider_id,
                           right->problem.finding.provider_id);
            if (order != 0) return order;
            break;
        case UMI_DIAGNOSTIC_PROBLEM_SORT_BASELINE:
            order = baseline_rank(left->problem.baseline_state) -
                    baseline_rank(right->problem.baseline_state);
            if (order != 0) return order;
            break;
        case UMI_DIAGNOSTIC_PROBLEM_SORT_NEWEST:
            if (left->problem.last_seen_run_id !=
                right->problem.last_seen_run_id) {
                return left->problem.last_seen_run_id >
                       right->problem.last_seen_run_id ? -1 : 1;
            }
            break;
        default:
            break;
    }
    order = compare_location(&left->problem, &right->problem);
    if (order != 0) return order;
    return strcmp(left->problem.finding.provider_id,
                  right->problem.finding.provider_id);
}

static uint64_t hash_text(uint64_t hash, const char *text)
{
    const unsigned char *cursor = (const unsigned char *)(text != NULL ? text : "");
    while (*cursor != 0U) {
        hash ^= (uint64_t)*cursor++;
        hash *= GROUP_HASH_PRIME;
    }
    hash ^= 0U;
    hash *= GROUP_HASH_PRIME;
    return hash;
}

static uint64_t group_hash(const UmiDiagnosticProblem *problem,
                           UmiDiagnosticProblemGroup group)
{
    uint64_t hash = GROUP_HASH_OFFSET;
    switch (group) {
        case UMI_DIAGNOSTIC_PROBLEM_GROUP_FILE:
            return hash_text(hash, problem->finding.diagnostic.uri);
        case UMI_DIAGNOSTIC_PROBLEM_GROUP_PROVIDER:
            return hash_text(hash, problem->finding.provider_id);
        case UMI_DIAGNOSTIC_PROBLEM_GROUP_SOURCE:
            return hash_text(hash, problem->finding.diagnostic.source);
        case UMI_DIAGNOSTIC_PROBLEM_GROUP_CODE:
            return hash_text(hash, problem->finding.diagnostic.code);
        case UMI_DIAGNOSTIC_PROBLEM_GROUP_SEVERITY:
            hash ^= (uint64_t)problem->effective_severity;
            return hash * GROUP_HASH_PRIME;
        case UMI_DIAGNOSTIC_PROBLEM_GROUP_NONE:
        default:
            return 1U;
    }
}

UmiStatus umi_diagnostic_problem_model_create(
    UmiDiagnosticProblemModel **out_model)
{
    UmiDiagnosticProblemModel *model;
    if (out_model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_model = NULL;
    model = (UmiDiagnosticProblemModel *)calloc(1U, sizeof(*model));
    if (model == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    model->revision = 1U;
    *out_model = model;
    return UMI_STATUS_OK;
}

void umi_diagnostic_problem_model_destroy(UmiDiagnosticProblemModel *model)
{
    if (model == NULL) return;
    free(model->items);
    model->items = NULL;
    free(model);
}

UmiStatus umi_diagnostic_problem_model_clear(UmiDiagnosticProblemModel *model)
{
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    model->count = 0U;
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_diagnostic_problem_model_begin_provider_run(
    UmiDiagnosticProblemModel *model,
    const char *provider_id,
    uint64_t run_id)
{
    if (model == NULL || provider_id == NULL || provider_id[0] == '\0' ||
        run_id == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_diagnostic_problem_model_upsert(
    UmiDiagnosticProblemModel *model,
    const UmiDiagnosticProblem *problem)
{
    UmiDiagnosticProblem normalized;
    size_t position;
    uint64_t previous_revision = 0U;
    UmiStatus status;
    if (model == NULL || validate_problem(problem) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    normalized = *problem;
    normalized.active = problem->active != 0;
    normalized.selected = problem->selected != 0;
    normalized.policy_disabled = problem->policy_disabled != 0;
    normalized.fixable = problem->fixable != 0;
    position = find_fingerprint(model, normalized.finding.fingerprint);
    if (position == SIZE_MAX) {
        status = reserve_problems(model, model->count + 1U);
        if (status != UMI_STATUS_OK) return status;
        position = model->count++;
        if (normalized.first_seen_run_id == 0U) {
            normalized.first_seen_run_id = normalized.last_seen_run_id;
        }
    } else {
        previous_revision = model->items[position].revision;
        normalized.first_seen_run_id = model->items[position].first_seen_run_id;
        if (normalized.first_seen_run_id == 0U) {
            normalized.first_seen_run_id = normalized.last_seen_run_id;
        }
        if (!problem->selected) {
            normalized.selected = model->items[position].selected;
        }
    }
    normalized.revision = next_revision(previous_revision);
    model->items[position] = normalized;
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_diagnostic_problem_model_finish_provider_run(
    UmiDiagnosticProblemModel *model,
    const char *provider_id,
    uint64_t run_id,
    size_t *out_resolved_count)
{
    size_t position;
    size_t resolved = 0U;
    if (model == NULL || provider_id == NULL || provider_id[0] == '\0' ||
        run_id == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (position = 0U; position < model->count; ++position) {
        UmiDiagnosticProblem *problem = &model->items[position];
        if (strcmp(problem->finding.provider_id, provider_id) != 0 ||
            problem->last_seen_run_id == run_id || !problem->active) {
            continue;
        }
        problem->active = 0;
        problem->finding.diagnostic.resolved = 1;
        problem->revision = next_revision(problem->revision);
        ++resolved;
    }
    if (resolved > 0U) model->revision = next_revision(model->revision);
    if (out_resolved_count != NULL) *out_resolved_count = resolved;
    return UMI_STATUS_OK;
}

UmiStatus umi_diagnostic_problem_model_remove(
    UmiDiagnosticProblemModel *model,
    uint64_t fingerprint)
{
    size_t position;
    if (model == NULL || fingerprint == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    position = find_fingerprint(model, fingerprint);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (position + 1U < model->count) {
        (void)memmove(&model->items[position],
                      &model->items[position + 1U],
                      (model->count - position - 1U) * sizeof(*model->items));
    }
    --model->count;
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_diagnostic_problem_model_find(
    const UmiDiagnosticProblemModel *model,
    uint64_t fingerprint,
    UmiDiagnosticProblem *out_problem)
{
    size_t position;
    if (model == NULL || fingerprint == 0U || out_problem == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_fingerprint(model, fingerprint);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_problem = model->items[position];
    return UMI_STATUS_OK;
}

UmiStatus umi_diagnostic_problem_model_at(
    const UmiDiagnosticProblemModel *model,
    size_t position,
    UmiDiagnosticProblem *out_problem)
{
    if (model == NULL || out_problem == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (position >= model->count) return UMI_STATUS_NOT_FOUND;
    *out_problem = model->items[position];
    return UMI_STATUS_OK;
}

UmiStatus umi_diagnostic_problem_model_select(
    UmiDiagnosticProblemModel *model,
    uint64_t fingerprint,
    int selected)
{
    size_t position;
    if (model == NULL || fingerprint == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    position = find_fingerprint(model, fingerprint);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    model->items[position].selected = selected != 0;
    model->items[position].revision =
        next_revision(model->items[position].revision);
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_diagnostic_problem_model_clear_selection(
    UmiDiagnosticProblemModel *model)
{
    size_t position;
    int changed = 0;
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (position = 0U; position < model->count; ++position) {
        if (!model->items[position].selected) continue;
        model->items[position].selected = 0;
        model->items[position].revision =
            next_revision(model->items[position].revision);
        changed = 1;
    }
    if (changed) model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

void umi_diagnostic_problem_query_init(UmiDiagnosticProblemQuery *query)
{
    if (query == NULL) return;
    (void)memset(query, 0, sizeof(*query));
    query->sort = UMI_DIAGNOSTIC_PROBLEM_SORT_SEVERITY;
    query->group = UMI_DIAGNOSTIC_PROBLEM_GROUP_FILE;
    query->limit = UMI_DIAGNOSTIC_PROBLEM_QUERY_RESULT_MAX;
}

UmiStatus umi_diagnostic_problem_query_execute(
    const UmiDiagnosticProblemModel *model,
    const UmiDiagnosticProblemQuery *query,
    UmiDiagnosticProblemQueryResult *out_result)
{
    ProblemSortEntry *matches;
    uint64_t *group_hashes;
    size_t position;
    size_t match_count = 0U;
    size_t group_count = 0U;
    size_t output_limit;
    size_t start;
    if (model == NULL || query == NULL || out_result == NULL ||
        query->sort < UMI_DIAGNOSTIC_PROBLEM_SORT_SEVERITY ||
        query->sort > UMI_DIAGNOSTIC_PROBLEM_SORT_NEWEST ||
        query->group < UMI_DIAGNOSTIC_PROBLEM_GROUP_NONE ||
        query->group > UMI_DIAGNOSTIC_PROBLEM_GROUP_CODE) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_result, 0, sizeof(*out_result));
    if (model->count == 0U) return UMI_STATUS_OK;
    if (model->count > SIZE_MAX / sizeof(*matches)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    matches = (ProblemSortEntry *)malloc(model->count * sizeof(*matches));
    group_hashes = (uint64_t *)calloc(model->count, sizeof(*group_hashes));
    if (matches == NULL || group_hashes == NULL) {
        free(matches);
        free(group_hashes);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    for (position = 0U; position < model->count; ++position) {
        if (!problem_matches(&model->items[position], query)) continue;
        matches[match_count].problem = model->items[position];
        matches[match_count].sort = query->sort;
        ++match_count;
    }
    if (match_count > 1U) {
        qsort(matches, match_count, sizeof(*matches), compare_sort_entries);
    }
    out_result->total_matches = match_count;
    for (position = 0U; position < match_count; ++position) {
        uint64_t current = group_hash(&matches[position].problem, query->group);
        size_t comparison;
        int unique = 1;
        for (comparison = 0U; comparison < group_count; ++comparison) {
            if (group_hashes[comparison] == current) {
                unique = 0;
                break;
            }
        }
        if (unique) group_hashes[group_count++] = current;
    }
    out_result->group_count = group_count;
    start = query->offset < match_count ? query->offset : match_count;
    output_limit = query->limit > 0U ? query->limit
                                    : UMI_DIAGNOSTIC_PROBLEM_QUERY_RESULT_MAX;
    if (output_limit > UMI_DIAGNOSTIC_PROBLEM_QUERY_RESULT_MAX) {
        output_limit = UMI_DIAGNOSTIC_PROBLEM_QUERY_RESULT_MAX;
    }
    for (position = start;
         position < match_count && out_result->count < output_limit;
         ++position) {
        out_result->items[out_result->count++] = matches[position].problem;
    }
    out_result->truncated = start + out_result->count < match_count;
    free(group_hashes);
    free(matches);
    return UMI_STATUS_OK;
}

UmiStatus umi_diagnostic_problem_model_snapshot(
    const UmiDiagnosticProblemModel *model,
    UmiDiagnosticProblemModelSnapshot *out_snapshot)
{
    size_t position;
    if (model == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_DIAGNOSTIC_PROBLEM_MODEL_API_VERSION;
    out_snapshot->problem_count = model->count;
    out_snapshot->revision = model->revision;
    for (position = 0U; position < model->count; ++position) {
        const UmiDiagnosticProblem *problem = &model->items[position];
        if (problem->active) ++out_snapshot->active_count;
        else ++out_snapshot->resolved_count;
        if (problem->suppression.suppressed) ++out_snapshot->suppressed_count;
        if (problem->policy_disabled) ++out_snapshot->disabled_count;
        if (problem->selected) ++out_snapshot->selected_count;
        if (problem->fixable) ++out_snapshot->fixable_count;
        if (problem->baseline_state == UMI_DIAGNOSTIC_BASELINE_NEW) {
            ++out_snapshot->new_count;
        } else if (problem->baseline_state == UMI_DIAGNOSTIC_BASELINE_UPDATED) {
            ++out_snapshot->updated_count;
        }
        if ((size_t)problem->effective_severity <
            UMI_DIAGNOSTIC_SEVERITY_COUNT) {
            ++out_snapshot->by_effective_severity[problem->effective_severity];
        }
    }
    return UMI_STATUS_OK;
}

size_t umi_diagnostic_problem_model_count(const UmiDiagnosticProblemModel *model)
{
    return model != NULL ? model->count : 0U;
}

uint64_t umi_diagnostic_problem_model_revision(
    const UmiDiagnosticProblemModel *model)
{
    return model != NULL ? model->revision : 0U;
}
