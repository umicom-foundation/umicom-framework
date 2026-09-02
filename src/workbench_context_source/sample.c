/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_source/sample.c
 *
 * PURPOSE:
 *   Implement live interaction sample construction, validation, metadata and content hashing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_source/sample.h"

#include <string.h>

/*
 * Initialise workbench context source sample from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_source_sample_init(
    UmiWorkbenchContextSourceSample *sample,
    UmiWorkbenchContextSourceKind source_kind,
    UmiWorkbenchContextSourceTrigger trigger,
    UmiContextKind context_kind,
    const char *sample_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (sample == NULL) return;
    memset(sample, 0, sizeof(*sample));
    sample->structure_size = (uint32_t)sizeof(*sample);
    sample->source_kind = source_kind;
    sample->trigger = trigger;
    sample->context_kind = context_kind;
    sample->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (sample_id != NULL) {
        (void)umi_workbench_context_source_copy_text(
            sample->sample_id,
            sizeof(sample->sample_id),
            sample_id);
    }
}

/*
 * Provide the workbench context source sample set identity operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_sample_set_identity(
    UmiWorkbenchContextSourceSample *sample,
    const char *source_id,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (sample == NULL || source_id == NULL ||
        application_id == NULL || panel_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_context_source_copy_text(
        sample->source_id, sizeof(sample->source_id), source_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_context_source_copy_text(
        sample->application_id, sizeof(sample->application_id), application_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_context_source_copy_text(
        sample->panel_id, sizeof(sample->panel_id), panel_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace_id != NULL) {
        status = umi_workbench_context_source_copy_text(
            sample->workspace_id, sizeof(sample->workspace_id), workspace_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    ++sample->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench context source sample set subject operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_sample_set_subject(
    UmiWorkbenchContextSourceSample *sample,
    const char *subject_id,
    const char *secondary_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (sample == NULL || subject_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_context_source_copy_text(
        sample->subject_id, sizeof(sample->subject_id), subject_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (secondary_id != NULL) {
        status = umi_workbench_context_source_copy_text(
            sample->secondary_id, sizeof(sample->secondary_id), secondary_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    ++sample->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench context source sample set location operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_sample_set_location(
    UmiWorkbenchContextSourceSample *sample,
    const char *path,
    const char *symbol,
    uint32_t line,
    uint32_t column,
    uint32_t selection_length)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (sample == NULL || path == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_source_copy_text(
        sample->path, sizeof(sample->path), path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (symbol != NULL) {
        status = umi_workbench_context_source_copy_text(
            sample->symbol, sizeof(sample->symbol), symbol);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    sample->line = line;
    sample->column = column;
    sample->selection_length = selection_length;
    ++sample->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench context source sample add metadata operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_sample_add_metadata(
    UmiWorkbenchContextSourceSample *sample,
    const char *name,
    const char *value)
{
    UmiWorkbenchContextEventMetadata *item;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (sample == NULL || name == NULL || value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (sample->metadata_count >= UMI_WORKBENCH_CONTEXT_SOURCE_MAX_METADATA) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    item = &sample->metadata[sample->metadata_count];
    status = umi_workbench_context_event_copy_text(
        item->name, sizeof(item->name), name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_context_event_copy_text(
        item->value, sizeof(item->value), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    ++sample->metadata_count;
    ++sample->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench context source sample find metadata operation used by this module
 * and its client applications.
 */
const UmiWorkbenchContextEventMetadata *
umi_workbench_context_source_sample_find_metadata(
    const UmiWorkbenchContextSourceSample *sample,
    const char *name)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (sample == NULL || name == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < sample->metadata_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(sample->metadata[index].name, name) == 0) {
            return &sample->metadata[index];
        }
    }
    return NULL;
}

/*
 * Check that workbench context source sample satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_context_source_sample_validate(
    const UmiWorkbenchContextSourceSample *sample)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (sample == NULL || sample->structure_size != sizeof(*sample) ||
        sample->sample_id[0] == '\0' ||
        sample->source_id[0] == '\0' ||
        sample->application_id[0] == '\0' ||
        sample->panel_id[0] == '\0' ||
        sample->source_kind < UMI_WORKBENCH_CONTEXT_SOURCE_GENERIC ||
        sample->source_kind > UMI_WORKBENCH_CONTEXT_SOURCE_MEDIA ||
        sample->trigger < UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_ACTIVATE ||
        sample->trigger > UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_NAVIGATE ||
        sample->context_kind < UMI_CONTEXT_KIND_GENERIC ||
        sample->context_kind > UMI_CONTEXT_KIND_SELECTION ||
        sample->metadata_count > UMI_WORKBENCH_CONTEXT_SOURCE_MAX_METADATA) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench context source sample refresh hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_source_sample_refresh_hash(
    UmiWorkbenchContextSourceSample *sample)
{
    uint64_t hash = UINT64_C(1469598103934665603);
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (sample == NULL) return 0U;
    hash = umi_workbench_context_source_hash_text(
        hash, sample->source_id, sizeof(sample->source_id));
    hash = umi_workbench_context_source_hash_text(
        hash, sample->application_id, sizeof(sample->application_id));
    hash = umi_workbench_context_source_hash_text(
        hash, sample->panel_id, sizeof(sample->panel_id));
    hash = umi_workbench_context_source_hash_text(
        hash, sample->subject_id, sizeof(sample->subject_id));
    hash = umi_workbench_context_source_hash_text(
        hash, sample->secondary_id, sizeof(sample->secondary_id));
    hash = umi_workbench_context_source_hash_text(
        hash, sample->path, sizeof(sample->path));
    hash = umi_workbench_context_source_hash_text(
        hash, sample->symbol, sizeof(sample->symbol));
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < sample->metadata_count; ++index) {
        hash = umi_workbench_context_source_hash_text(
            hash, sample->metadata[index].name,
            sizeof(sample->metadata[index].name));
        hash = umi_workbench_context_source_hash_text(
            hash, sample->metadata[index].value,
            sizeof(sample->metadata[index].value));
    }
    hash ^= (uint64_t)sample->source_kind;
    hash *= UINT64_C(1099511628211);
    hash ^= (uint64_t)sample->trigger;
    hash *= UINT64_C(1099511628211);
    hash ^= (uint64_t)sample->context_kind;
    hash *= UINT64_C(1099511628211);
    hash ^= sample->line;
    hash *= UINT64_C(1099511628211);
    hash ^= sample->column;
    hash *= UINT64_C(1099511628211);
    hash ^= sample->selection_length;
    hash *= UINT64_C(1099511628211);
    hash ^= sample->unsigned_value;
    hash *= UINT64_C(1099511628211);
    sample->content_hash = hash;
    return hash;
}
