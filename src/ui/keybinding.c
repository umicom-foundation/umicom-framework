/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/keybinding.c
 *
 * PURPOSE:
 *   Implement keyboard chord registration, conflict detection and context-sensitive resolution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Resolution returns a command identifier only; execution remains the responsibility of the authoritative command registry.
 */

#include "umicom/ui/keybinding.h"

#include <stdlib.h>
#include <string.h>

#include "umicom/platform/threading.h"
#include "ui_internal.h"

struct UmiUiKeybindingRegistry {
    UmiUiKeybindingSnapshot items[UMI_UI_KEYBINDING_MAX];
    size_t count;
    uint64_t revision;
    UmiMutex *mutex;
};

static size_t find_item(const UmiUiKeybindingRegistry *registry, const char *id)
{
    size_t index;
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->items[index].binding_id, id) == 0) return index;
    }
    return SIZE_MAX;
}

UmiStatus umi_ui_keybinding_registry_create(UmiUiKeybindingRegistry **out_registry)
{
    UmiUiKeybindingRegistry *registry;
    UmiStatus status;
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiUiKeybindingRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_mutex_create(&registry->mutex);
    if (status != UMI_STATUS_OK) {
        free(registry);
        return status;
    }
    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_ui_keybinding_registry_destroy(UmiUiKeybindingRegistry *registry)
{
    if (registry == NULL) return;
    umi_mutex_destroy(registry->mutex);
    free(registry);
}

UmiStatus umi_ui_keybinding_registry_upsert(UmiUiKeybindingRegistry *registry,
                                            const UmiUiKeybindingSnapshot *item)
{
    size_t index;
    size_t existing;
    if (registry == NULL || item == NULL ||
        !umi_ui_id_is_valid(item->binding_id) ||
        !umi_ui_id_is_valid(item->command_id) ||
        item->chord[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)umi_mutex_lock(registry->mutex);
    existing = find_item(registry, item->binding_id);
    for (index = 0U; index < registry->count; ++index) {
        if (index != existing &&
            registry->items[index].enabled && item->enabled &&
            strcmp(registry->items[index].chord, item->chord) == 0 &&
            strcmp(registry->items[index].when_expression,
                   item->when_expression) == 0 &&
            (strcmp(registry->items[index].command_id,
                    item->command_id) != 0 ||
             strcmp(registry->items[index].argument,
                    item->argument) != 0)) {
            (void)umi_mutex_unlock(registry->mutex);
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }

    if (existing == SIZE_MAX) {
        if (registry->count >= UMI_UI_KEYBINDING_MAX) {
            (void)umi_mutex_unlock(registry->mutex);
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        existing = registry->count++;
    }
    registry->items[existing] = *item;
    registry->revision = umi_ui_next_revision(registry->revision);
    (void)umi_mutex_unlock(registry->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_keybinding_registry_remove(UmiUiKeybindingRegistry *registry,
                                            const char *binding_id)
{
    size_t index;
    if (registry == NULL || binding_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(registry->mutex);
    index = find_item(registry, binding_id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(registry->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    if (index + 1U < registry->count) {
        (void)memmove(&registry->items[index],
                      &registry->items[index + 1U],
                      (registry->count - index - 1U) * sizeof(registry->items[0]));
    }
    --registry->count;
    registry->revision = umi_ui_next_revision(registry->revision);
    (void)umi_mutex_unlock(registry->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_keybinding_registry_find(const UmiUiKeybindingRegistry *registry,
                                          const char *binding_id,
                                          UmiUiKeybindingSnapshot *out_item)
{
    size_t index;
    if (registry == NULL || binding_id == NULL || out_item == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(registry->mutex);
    index = find_item(registry, binding_id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(registry->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    *out_item = registry->items[index];
    (void)umi_mutex_unlock(registry->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_keybinding_registry_at(const UmiUiKeybindingRegistry *registry,
                                        size_t index,
                                        UmiUiKeybindingSnapshot *out_item)
{
    if (registry == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(registry->mutex);
    if (index >= registry->count) {
        (void)umi_mutex_unlock(registry->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    *out_item = registry->items[index];
    (void)umi_mutex_unlock(registry->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_keybinding_registry_resolve(
    const UmiUiKeybindingRegistry *registry,
    const UmiUiContextStore *context,
    const char *chord,
    UmiUiKeybindingResolution *out_resolution)
{
    size_t index;
    const UmiUiKeybindingSnapshot *best = NULL;
    if (registry == NULL || chord == NULL || out_resolution == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_resolution, 0, sizeof(*out_resolution));

    (void)umi_mutex_lock(registry->mutex);
    for (index = 0U; index < registry->count; ++index) {
        const UmiUiKeybindingSnapshot *candidate = &registry->items[index];
        if (!candidate->enabled || strcmp(candidate->chord, chord) != 0) continue;
        if (candidate->when_expression[0] != '\0' &&
            !umi_ui_context_evaluate(context, candidate->when_expression)) {
            continue;
        }
        if (best == NULL || candidate->order < best->order) best = candidate;
    }
    if (best == NULL) {
        (void)umi_mutex_unlock(registry->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    (void)umi_ui_copy_text(out_resolution->binding_id,
                           sizeof(out_resolution->binding_id),
                           best->binding_id);
    (void)umi_ui_copy_text(out_resolution->command_id,
                           sizeof(out_resolution->command_id),
                           best->command_id);
    (void)umi_ui_copy_text(out_resolution->argument,
                           sizeof(out_resolution->argument),
                           best->argument);
    (void)umi_mutex_unlock(registry->mutex);
    return UMI_STATUS_OK;
}

size_t umi_ui_keybinding_registry_count(const UmiUiKeybindingRegistry *registry)
{
    size_t count = 0U;
    if (registry != NULL) {
        (void)umi_mutex_lock(registry->mutex);
        count = registry->count;
        (void)umi_mutex_unlock(registry->mutex);
    }
    return count;
}

uint64_t umi_ui_keybinding_registry_revision(const UmiUiKeybindingRegistry *registry)
{
    uint64_t revision = 0U;
    if (registry != NULL) {
        (void)umi_mutex_lock(registry->mutex);
        revision = registry->revision;
        (void)umi_mutex_unlock(registry->mutex);
    }
    return revision;
}
