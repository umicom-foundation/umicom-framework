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

static size_t physical_index(const UmiDiagnosticModel *model, size_t index)
{
    return (model->head + index) % model->capacity;
}

static size_t find_index_unlocked(const UmiDiagnosticModel *model, const char *id)
{
    size_t index;
    for (index = 0U; index < model->count; ++index) {
        if (strcmp(model->items[physical_index(model, index)].id, id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

UmiDiagnosticModelConfig umi_diagnostic_model_config_default(void)
{
    UmiDiagnosticModelConfig config = { UMI_DIAGNOSTIC_MODEL_DEFAULT_CAPACITY };
    return config;
}

UmiStatus umi_diagnostic_model_create(const UmiDiagnosticModelConfig *config,
                                      UmiDiagnosticModel **out_model)
{
    UmiDiagnosticModelConfig effective;
    UmiDiagnosticModel *model;
    if (out_model == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_model = NULL;
    effective = config != NULL ? *config : umi_diagnostic_model_config_default();
    if (effective.capacity == 0U || effective.capacity > UMI_DIAGNOSTIC_MODEL_MAX_CAPACITY) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    model = (UmiDiagnosticModel *)calloc(1U, sizeof(*model));
    if (model == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    model->items = (UmiDiagnosticSnapshot *)calloc(effective.capacity,
                                                   sizeof(*model->items));
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

void umi_diagnostic_model_destroy(UmiDiagnosticModel *model)
{
    if (model != NULL) {
        free(model->items);
        free(model);
    }
}

void umi_diagnostic_model_clear(UmiDiagnosticModel *model)
{
    if (model == NULL) return;
    umi_diagnostic_lock_acquire(&model->lock);
    (void)memset(model->items, 0, model->capacity * sizeof(*model->items));
    model->count = 0U;
    model->head = 0U;
    ++model->revision;
    umi_diagnostic_lock_release(&model->lock);
}

UmiStatus umi_diagnostic_model_upsert(UmiDiagnosticModel *model,
                                      const UmiDiagnosticSnapshot *snapshot)
{
    UmiDiagnosticSnapshot stored;
    size_t logical;
    size_t destination;
    if (model == NULL || umi_diagnostic_snapshot_validate(snapshot, NULL, 0U) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    stored = *snapshot;
    umi_diagnostic_lock_acquire(&model->lock);
    logical = find_index_unlocked(model, stored.id);
    ++model->total_received;
    ++model->revision;
    if (logical != SIZE_MAX) {
        destination = physical_index(model, logical);
        stored.sequence = model->items[destination].sequence;
    } else {
        stored.sequence = model->next_sequence++;
        if (model->count < model->capacity) {
            destination = physical_index(model, model->count);
            ++model->count;
        } else {
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

UmiStatus umi_diagnostic_model_remove(UmiDiagnosticModel *model, const char *id)
{
    size_t logical;
    size_t index;
    if (model == NULL || id == NULL || id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    umi_diagnostic_lock_acquire(&model->lock);
    logical = find_index_unlocked(model, id);
    if (logical == SIZE_MAX) {
        umi_diagnostic_lock_release(&model->lock);
        return UMI_STATUS_NOT_FOUND;
    }
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

UmiStatus umi_diagnostic_model_resolve(UmiDiagnosticModel *model,
                                       const char *id, int resolved)
{
    size_t logical;
    if (model == NULL || id == NULL || id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    umi_diagnostic_lock_acquire(&model->lock);
    logical = find_index_unlocked(model, id);
    if (logical == SIZE_MAX) {
        umi_diagnostic_lock_release(&model->lock);
        return UMI_STATUS_NOT_FOUND;
    }
    model->items[physical_index(model, logical)].resolved = resolved != 0;
    model->items[physical_index(model, logical)].revision = ++model->revision;
    umi_diagnostic_lock_release(&model->lock);
    return UMI_STATUS_OK;
}

UmiStatus umi_diagnostic_model_find(const UmiDiagnosticModel *model,
                                    const char *id,
                                    UmiDiagnosticSnapshot *out_snapshot)
{
    size_t logical;
    if (model == NULL || id == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_diagnostic_lock_acquire(&model->lock);
    logical = find_index_unlocked(model, id);
    if (logical != SIZE_MAX) *out_snapshot = model->items[physical_index(model, logical)];
    umi_diagnostic_lock_release(&model->lock);
    return logical != SIZE_MAX ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_diagnostic_model_at(const UmiDiagnosticModel *model,
                                  size_t chronological_index,
                                  UmiDiagnosticSnapshot *out_snapshot)
{
    if (model == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_diagnostic_lock_acquire(&model->lock);
    if (chronological_index >= model->count) {
        umi_diagnostic_lock_release(&model->lock);
        return UMI_STATUS_NOT_FOUND;
    }
    *out_snapshot = model->items[physical_index(model, chronological_index)];
    umi_diagnostic_lock_release(&model->lock);
    return UMI_STATUS_OK;
}

UmiStatus umi_diagnostic_model_summary(const UmiDiagnosticModel *model,
                                       UmiDiagnosticModelSummary *out_summary)
{
    size_t index;
    if (model == NULL || out_summary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_summary, 0, sizeof(*out_summary));
    umi_diagnostic_lock_acquire(&model->lock);
    out_summary->retained_count = model->count;
    out_summary->total_received = model->total_received;
    out_summary->overwritten_count = model->overwritten_count;
    out_summary->revision = model->revision;
    for (index = 0U; index < model->count; ++index) {
        const UmiDiagnosticSnapshot *item = &model->items[physical_index(model, index)];
        if (item->resolved != 0) ++out_summary->resolved_count;
        else ++out_summary->active_count;
        ++out_summary->by_severity[(size_t)item->severity];
        ++out_summary->by_kind[(size_t)item->kind];
    }
    umi_diagnostic_lock_release(&model->lock);
    return UMI_STATUS_OK;
}

size_t umi_diagnostic_model_count(const UmiDiagnosticModel *model)
{
    size_t count = 0U;
    if (model != NULL) {
        umi_diagnostic_lock_acquire(&model->lock);
        count = model->count;
        umi_diagnostic_lock_release(&model->lock);
    }
    return count;
}

uint64_t umi_diagnostic_model_revision(const UmiDiagnosticModel *model)
{
    uint64_t revision = 0U;
    if (model != NULL) {
        umi_diagnostic_lock_acquire(&model->lock);
        revision = model->revision;
        umi_diagnostic_lock_release(&model->lock);
    }
    return revision;
}
