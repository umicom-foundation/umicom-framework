/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostics/baseline.c
 *
 * PURPOSE:
 *   Implement diagnostic baseline capture and deterministic comparison without
 *   deleting either current or historical findings.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostics/baseline.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

#define BASELINE_INITIAL_CAPACITY 32U
#define BASELINE_MAX_CAPACITY 65536U

typedef struct BaselineEntryState {
    UmiDiagnosticBaselineEntry entry;
    uint64_t seen_token;
} BaselineEntryState;

struct UmiDiagnosticBaseline {
    BaselineEntryState *items;
    size_t count;
    size_t capacity;
    uint64_t generation;
    uint64_t comparison_token;
    uint64_t active_token;
    uint64_t revision;
    UmiDiagnosticBaselineDiff current_diff;
};

static uint64_t next_revision(uint64_t value)
{
    return value == UINT64_MAX ? 1U : value + 1U;
}

static size_t find_fingerprint(const UmiDiagnosticBaseline *baseline,
                               uint64_t fingerprint)
{
    size_t position;
    if (baseline == NULL || fingerprint == 0U) return SIZE_MAX;
    for (position = 0U; position < baseline->count; ++position) {
        if (baseline->items[position].entry.fingerprint == fingerprint) {
            return position;
        }
    }
    return SIZE_MAX;
}

static UmiStatus reserve_entries(UmiDiagnosticBaseline *baseline,
                                 size_t required)
{
    size_t capacity;
    BaselineEntryState *replacement;
    if (required <= baseline->capacity) return UMI_STATUS_OK;
    capacity = baseline->capacity > 0U
        ? baseline->capacity : BASELINE_INITIAL_CAPACITY;
    while (capacity < required) {
        if (capacity >= BASELINE_MAX_CAPACITY || capacity > SIZE_MAX / 2U) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        capacity *= 2U;
        if (capacity > BASELINE_MAX_CAPACITY) capacity = BASELINE_MAX_CAPACITY;
    }
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (BaselineEntryState *)realloc(
        baseline->items, capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    baseline->items = replacement;
    baseline->capacity = capacity;
    return UMI_STATUS_OK;
}

static void fill_entry(UmiDiagnosticBaselineEntry *entry,
                       const UmiDiagnosticProviderFinding *finding)
{
    (void)memset(entry, 0, sizeof(*entry));
    entry->struct_size = (uint32_t)sizeof(*entry);
    entry->api_version = UMI_DIAGNOSTIC_BASELINE_API_VERSION;
    entry->fingerprint = finding->fingerprint != 0U
        ? finding->fingerprint : umi_diagnostic_provider_fingerprint(finding);
    entry->content_fingerprint = finding->content_fingerprint != 0U
        ? finding->content_fingerprint
        : umi_diagnostic_provider_content_fingerprint(finding);
    (void)memcpy(entry->provider_id, finding->provider_id,
                 strlen(finding->provider_id) + 1U);
    (void)memcpy(entry->diagnostic_id, finding->diagnostic.id,
                 strlen(finding->diagnostic.id) + 1U);
    (void)memcpy(entry->source, finding->diagnostic.source,
                 strlen(finding->diagnostic.source) + 1U);
    (void)memcpy(entry->code, finding->diagnostic.code,
                 strlen(finding->diagnostic.code) + 1U);
    (void)memcpy(entry->uri, finding->diagnostic.uri,
                 strlen(finding->diagnostic.uri) + 1U);
    entry->line = finding->diagnostic.line;
    entry->column = finding->diagnostic.column;
    entry->severity = finding->diagnostic.severity;
}

static UmiStatus capture_without_revision(
    UmiDiagnosticBaseline *baseline,
    const UmiDiagnosticProviderFinding *finding)
{
    UmiDiagnosticBaselineEntry entry;
    size_t position;
    UmiStatus status;
    if (umi_diagnostic_provider_finding_validate(finding) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    fill_entry(&entry, finding);
    position = find_fingerprint(baseline, entry.fingerprint);
    if (position == SIZE_MAX) {
        status = reserve_entries(baseline, baseline->count + 1U);
        if (status != UMI_STATUS_OK) return status;
        position = baseline->count++;
    }
    baseline->items[position].entry = entry;
    baseline->items[position].seen_token = 0U;
    return UMI_STATUS_OK;
}

UmiStatus umi_diagnostic_baseline_create(UmiDiagnosticBaseline **out_baseline)
{
    UmiDiagnosticBaseline *baseline;
    if (out_baseline == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_baseline = NULL;
    baseline = (UmiDiagnosticBaseline *)calloc(1U, sizeof(*baseline));
    if (baseline == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    baseline->generation = 1U;
    baseline->revision = 1U;
    *out_baseline = baseline;
    return UMI_STATUS_OK;
}

void umi_diagnostic_baseline_destroy(UmiDiagnosticBaseline *baseline)
{
    if (baseline == NULL) return;
    free(baseline->items);
    baseline->items = NULL;
    free(baseline);
}

UmiStatus umi_diagnostic_baseline_clear(UmiDiagnosticBaseline *baseline)
{
    if (baseline == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    baseline->count = 0U;
    baseline->active_token = 0U;
    baseline->generation = next_revision(baseline->generation);
    baseline->revision = next_revision(baseline->revision);
    (void)memset(&baseline->current_diff, 0, sizeof(baseline->current_diff));
    return UMI_STATUS_OK;
}

UmiStatus umi_diagnostic_baseline_capture_finding(
    UmiDiagnosticBaseline *baseline,
    const UmiDiagnosticProviderFinding *finding)
{
    UmiStatus status;
    if (baseline == NULL || finding == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = capture_without_revision(baseline, finding);
    if (status == UMI_STATUS_OK) {
        baseline->active_token = 0U;
        baseline->revision = next_revision(baseline->revision);
    }
    return status;
}

UmiStatus umi_diagnostic_baseline_capture_batch(
    UmiDiagnosticBaseline *baseline,
    const UmiDiagnosticProviderBatch *batch)
{
    size_t position;
    UmiDiagnosticProviderFinding finding;
    UmiStatus status;
    if (baseline == NULL || batch == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    baseline->count = 0U;
    for (position = 0U; position < umi_diagnostic_provider_batch_count(batch);
         ++position) {
        status = umi_diagnostic_provider_batch_at(batch, position, &finding);
        if (status != UMI_STATUS_OK) {
            baseline->count = 0U;
            return status;
        }
        status = capture_without_revision(baseline, &finding);
        if (status != UMI_STATUS_OK) {
            baseline->count = 0U;
            return status;
        }
    }
    baseline->generation = next_revision(baseline->generation);
    baseline->active_token = 0U;
    baseline->revision = next_revision(baseline->revision);
    (void)memset(&baseline->current_diff, 0, sizeof(baseline->current_diff));
    return UMI_STATUS_OK;
}

UmiStatus umi_diagnostic_baseline_begin_comparison(
    UmiDiagnosticBaseline *baseline)
{
    if (baseline == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    baseline->comparison_token = next_revision(baseline->comparison_token);
    baseline->active_token = baseline->comparison_token;
    (void)memset(&baseline->current_diff, 0, sizeof(baseline->current_diff));
    baseline->current_diff.struct_size =
        (uint32_t)sizeof(baseline->current_diff);
    baseline->current_diff.api_version = UMI_DIAGNOSTIC_BASELINE_API_VERSION;
    baseline->current_diff.baseline_generation = baseline->generation;
    baseline->current_diff.revision = next_revision(baseline->revision);
    baseline->revision = baseline->current_diff.revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_diagnostic_baseline_compare(
    UmiDiagnosticBaseline *baseline,
    const UmiDiagnosticProviderFinding *finding,
    UmiDiagnosticBaselineDecision *out_decision)
{
    uint64_t fingerprint;
    uint64_t content_fingerprint;
    size_t position;
    BaselineEntryState *state;
    if (baseline == NULL || finding == NULL || out_decision == NULL ||
        baseline->active_token == 0U ||
        umi_diagnostic_provider_finding_validate(finding) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    fingerprint = finding->fingerprint != 0U
        ? finding->fingerprint : umi_diagnostic_provider_fingerprint(finding);
    content_fingerprint = finding->content_fingerprint != 0U
        ? finding->content_fingerprint
        : umi_diagnostic_provider_content_fingerprint(finding);
    (void)memset(out_decision, 0, sizeof(*out_decision));
    out_decision->struct_size = (uint32_t)sizeof(*out_decision);
    out_decision->api_version = UMI_DIAGNOSTIC_BASELINE_API_VERSION;
    out_decision->fingerprint = fingerprint;
    out_decision->current_content_fingerprint = content_fingerprint;
    out_decision->current_severity = finding->diagnostic.severity;
    position = find_fingerprint(baseline, fingerprint);
    if (position == SIZE_MAX) {
        out_decision->state = UMI_DIAGNOSTIC_BASELINE_NEW;
        ++baseline->current_diff.new_count;
        return UMI_STATUS_OK;
    }
    state = &baseline->items[position];
    state->seen_token = baseline->active_token;
    out_decision->baseline_content_fingerprint =
        state->entry.content_fingerprint;
    out_decision->baseline_severity = state->entry.severity;
    out_decision->severity_changed =
        state->entry.severity != finding->diagnostic.severity;
    out_decision->content_changed =
        state->entry.content_fingerprint != content_fingerprint;
    if (out_decision->content_changed || out_decision->severity_changed) {
        out_decision->state = UMI_DIAGNOSTIC_BASELINE_UPDATED;
        ++baseline->current_diff.updated_count;
        if (out_decision->severity_changed) {
            ++baseline->current_diff.severity_changed_count;
        }
    } else {
        out_decision->state = UMI_DIAGNOSTIC_BASELINE_UNCHANGED;
        ++baseline->current_diff.unchanged_count;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_diagnostic_baseline_finish_comparison(
    UmiDiagnosticBaseline *baseline,
    UmiDiagnosticBaselineDiff *out_diff)
{
    size_t position;
    if (baseline == NULL || out_diff == NULL || baseline->active_token == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    baseline->current_diff.absent_count = 0U;
    for (position = 0U; position < baseline->count; ++position) {
        if (baseline->items[position].seen_token != baseline->active_token) {
            ++baseline->current_diff.absent_count;
        }
    }
    baseline->current_diff.revision = next_revision(baseline->revision);
    baseline->revision = baseline->current_diff.revision;
    *out_diff = baseline->current_diff;
    return UMI_STATUS_OK;
}

UmiStatus umi_diagnostic_baseline_at(
    const UmiDiagnosticBaseline *baseline,
    size_t position,
    UmiDiagnosticBaselineEntry *out_entry)
{
    if (baseline == NULL || out_entry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (position >= baseline->count) return UMI_STATUS_NOT_FOUND;
    *out_entry = baseline->items[position].entry;
    return UMI_STATUS_OK;
}

UmiStatus umi_diagnostic_baseline_absent_at(
    const UmiDiagnosticBaseline *baseline,
    size_t position,
    UmiDiagnosticBaselineEntry *out_entry)
{
    size_t cursor;
    size_t absent_index = 0U;
    if (baseline == NULL || out_entry == NULL || baseline->active_token == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (cursor = 0U; cursor < baseline->count; ++cursor) {
        if (baseline->items[cursor].seen_token == baseline->active_token) continue;
        if (absent_index == position) {
            *out_entry = baseline->items[cursor].entry;
            return UMI_STATUS_OK;
        }
        ++absent_index;
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_diagnostic_baseline_snapshot(
    const UmiDiagnosticBaseline *baseline,
    UmiDiagnosticBaselineSnapshot *out_snapshot)
{
    size_t position;
    if (baseline == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_DIAGNOSTIC_BASELINE_API_VERSION;
    out_snapshot->entry_count = baseline->count;
    out_snapshot->generation = baseline->generation;
    out_snapshot->revision = baseline->revision;
    if (baseline->active_token != 0U) {
        for (position = 0U; position < baseline->count; ++position) {
            if (baseline->items[position].seen_token == baseline->active_token) {
                ++out_snapshot->seen_count;
            } else {
                ++out_snapshot->absent_count;
            }
        }
    }
    return UMI_STATUS_OK;
}

size_t umi_diagnostic_baseline_count(const UmiDiagnosticBaseline *baseline)
{
    return baseline != NULL ? baseline->count : 0U;
}

uint64_t umi_diagnostic_baseline_revision(const UmiDiagnosticBaseline *baseline)
{
    return baseline != NULL ? baseline->revision : 0U;
}
