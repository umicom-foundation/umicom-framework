/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostics/model.c
 *
 * PURPOSE:
 *   Implement the model behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostics/model.h"

#include <stdlib.h>
#include <string.h>

#include "diagnostic_lock.h"

struct UmiDiagnosticModel {
    UmiDiagnosticSnapshot *items;
    size_t capacity;
    size_t count;
    size_t head;
    uint64_t next_sequence;
    uint64_t total_received;
    uint64_t overwritten_count;
    uint64_t revision;
    UmiDiagnosticLock lock;
};

/* Provide the physical index operation used by this module and its client applications. */
static size_t physical_index(const UmiDiagnosticModel *model, size_t index)
{
    return (model->head + index) % model->capacity;
}

/*
 * Provide the find index unlocked operation used by this module and its client
 * applications.
 */
static size_t find_index_unlocked(const UmiDiagnosticModel *model, const char *id)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(model->items[physical_index(model, index)].id, id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

/*
 * Provide the diagnostic model config default operation used by this module and its client
 * applications.
 */
UmiDiagnosticModelConfig umi_diagnostic_model_config_default(void)
{
    UmiDiagnosticModelConfig config = { UMI_DIAGNOSTIC_MODEL_DEFAULT_CAPACITY };
    return config;
}

/*
 * Initialise diagnostic model from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_diagnostic_model_create(const UmiDiagnosticModelConfig *config,
                                      UmiDiagnosticModel **out_model)
{
    UmiDiagnosticModelConfig effective;
    UmiDiagnosticModel *model;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_model == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_model = NULL;
    effective = config != NULL ? *config : umi_diagnostic_model_config_default();
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (effective.capacity == 0U || effective.capacity > UMI_DIAGNOSTIC_MODEL_MAX_CAPACITY) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    model = (UmiDiagnosticModel *)calloc(1U, sizeof(*model));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    model->items = (UmiDiagnosticSnapshot *)calloc(effective.capacity,
                                                   sizeof(*model->items));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model->items == NULL) {
        free(model);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    model->capacity = effective.capacity;
    model->next_sequence = 1U;
    umi_diagnostic_lock_init(&model->lock);
    *out_model = model;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by diagnostic model so the same storage can be reused
 * safely.
 */
void umi_diagnostic_model_destroy(UmiDiagnosticModel *model)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model != NULL) {
        free(model->items);
        free(model);
    }
}

/*
 * Release or reset state held by diagnostic model so the same storage can be reused
 * safely.
 */
void umi_diagnostic_model_clear(UmiDiagnosticModel *model)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return;
    umi_diagnostic_lock_acquire(&model->lock);
    (void)memset(model->items, 0, model->capacity * sizeof(*model->items));
    model->count = 0U;
    model->head = 0U;
    ++model->revision;
    umi_diagnostic_lock_release(&model->lock);
}

/*
 * Provide the diagnostic model upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_diagnostic_model_upsert(UmiDiagnosticModel *model,
                                      const UmiDiagnosticSnapshot *snapshot)
{
    UmiDiagnosticSnapshot stored;
    size_t logical;
    size_t destination;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || umi_diagnostic_snapshot_validate(snapshot, NULL, 0U) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    stored = *snapshot;
    umi_diagnostic_lock_acquire(&model->lock);
    logical = find_index_unlocked(model, stored.id);
    ++model->total_received;
    ++model->revision;
    /* Apply this branch only when its contract condition is satisfied. */
    if (logical != SIZE_MAX) {
        destination = physical_index(model, logical);
        stored.sequence = model->items[destination].sequence;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        stored.sequence = model->next_sequence++;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (model->count < model->capacity) {
            destination = physical_index(model, model->count);
            ++model->count;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            destination = model->head;
            model->head = (model->head + 1U) % model->capacity;
            ++model->overwritten_count;
        }
    }
    stored.revision = model->revision;
    model->items[destination] = stored;
    umi_diagnostic_lock_release(&model->lock);
    return UMI_STATUS_OK;
}

/*
 * Remove diagnostic model while keeping the remaining records in a valid and discoverable
 * state.
 */
UmiStatus umi_diagnostic_model_remove(UmiDiagnosticModel *model, const char *id)
{
    size_t logical;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || id == NULL || id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    umi_diagnostic_lock_acquire(&model->lock);
    logical = find_index_unlocked(model, id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (logical == SIZE_MAX) {
        umi_diagnostic_lock_release(&model->lock);
        return UMI_STATUS_NOT_FOUND;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = logical; index + 1U < model->count; ++index) {
        model->items[physical_index(model, index)] =
            model->items[physical_index(model, index + 1U)];
    }
    --model->count;
    (void)memset(&model->items[physical_index(model, model->count)], 0,
                 sizeof(*model->items));
    ++model->revision;
    umi_diagnostic_lock_release(&model->lock);
    return UMI_STATUS_OK;
}

/*
 * Provide the diagnostic model resolve operation used by this module and its client
 * applications.
 */
UmiStatus umi_diagnostic_model_resolve(UmiDiagnosticModel *model,
                                       const char *id, int resolved)
{
    size_t logical;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || id == NULL || id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    umi_diagnostic_lock_acquire(&model->lock);
    logical = find_index_unlocked(model, id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (logical == SIZE_MAX) {
        umi_diagnostic_lock_release(&model->lock);
        return UMI_STATUS_NOT_FOUND;
    }
    model->items[physical_index(model, logical)].resolved = resolved != 0;
    model->items[physical_index(model, logical)].revision = ++model->revision;
    umi_diagnostic_lock_release(&model->lock);
    return UMI_STATUS_OK;
}

/*
 * Find diagnostic model while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_diagnostic_model_find(const UmiDiagnosticModel *model,
                                    const char *id,
                                    UmiDiagnosticSnapshot *out_snapshot)
{
    size_t logical;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || id == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_diagnostic_lock_acquire(&model->lock);
    logical = find_index_unlocked(model, id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (logical != SIZE_MAX) *out_snapshot = model->items[physical_index(model, logical)];
    umi_diagnostic_lock_release(&model->lock);
    return logical != SIZE_MAX ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}

/*
 * Find diagnostic model while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_diagnostic_model_at(const UmiDiagnosticModel *model,
                                  size_t chronological_index,
                                  UmiDiagnosticSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_diagnostic_lock_acquire(&model->lock);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (chronological_index >= model->count) {
        umi_diagnostic_lock_release(&model->lock);
        return UMI_STATUS_NOT_FOUND;
    }
    *out_snapshot = model->items[physical_index(model, chronological_index)];
    umi_diagnostic_lock_release(&model->lock);
    return UMI_STATUS_OK;
}

/*
 * Provide the diagnostic model summary operation used by this module and its client
 * applications.
 */
UmiStatus umi_diagnostic_model_summary(const UmiDiagnosticModel *model,
                                       UmiDiagnosticModelSummary *out_summary)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || out_summary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_summary, 0, sizeof(*out_summary));
    umi_diagnostic_lock_acquire(&model->lock);
    out_summary->retained_count = model->count;
    out_summary->total_received = model->total_received;
    out_summary->overwritten_count = model->overwritten_count;
    out_summary->revision = model->revision;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        const UmiDiagnosticSnapshot *item = &model->items[physical_index(model, index)];
        /* Apply this branch only when its contract condition is satisfied. */
        if (item->resolved != 0) ++out_summary->resolved_count;
        /* Use this fallback path when the earlier condition does not apply. */
        else ++out_summary->active_count;
        ++out_summary->by_severity[(size_t)item->severity];
        ++out_summary->by_kind[(size_t)item->kind];
    }
    umi_diagnostic_lock_release(&model->lock);
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by diagnostic model without changing their
 * state.
 */
size_t umi_diagnostic_model_count(const UmiDiagnosticModel *model)
{
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model != NULL) {
        umi_diagnostic_lock_acquire(&model->lock);
        count = model->count;
        umi_diagnostic_lock_release(&model->lock);
    }
    return count;
}

/*
 * Provide the diagnostic model revision operation used by this module and its client
 * applications.
 */
uint64_t umi_diagnostic_model_revision(const UmiDiagnosticModel *model)
{
    uint64_t revision = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model != NULL) {
        umi_diagnostic_lock_acquire(&model->lock);
        revision = model->revision;
        umi_diagnostic_lock_release(&model->lock);
    }
    return revision;
}
