/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/command_registry.c
 *
 * PURPOSE:
 *   Implement deterministic command registration, metadata snapshots,
 *   enablement queries and execution without holding registry locks while
 *   product code runs.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/command_registry.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/platform/threading.h"

typedef struct UmiCommandEntry {
    char command_id[UMI_COMMAND_ID_CAPACITY];
    char title[UMI_COMMAND_TITLE_CAPACITY];
    char category[UMI_COMMAND_CATEGORY_CAPACITY];
    char description[UMI_COMMAND_DESCRIPTION_CAPACITY];
    char required_permission[UMI_COMMAND_PERMISSION_CAPACITY];
    uint32_t flags;
    UmiRegisteredCommandHandler handler;
    UmiRegisteredCommandEnabledFn enabled;
    void *user_data;
} UmiCommandEntry;

struct UmiCommandRegistry {
    UmiCommandEntry entries[UMI_COMMAND_REGISTRY_MAX];
    size_t count;
    UmiMutex *mutex;
};

static size_t umi_command_find_index(const UmiCommandRegistry *registry,
                                     const char *command_id)
{
    size_t index;
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->entries[index].command_id, command_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

static int umi_command_strings_fit(const UmiCommandDescriptor *descriptor)
{
    return strlen(descriptor->command_id) < UMI_COMMAND_ID_CAPACITY &&
           strlen(descriptor->title) < UMI_COMMAND_TITLE_CAPACITY &&
           strlen(descriptor->category) < UMI_COMMAND_CATEGORY_CAPACITY &&
           strlen(descriptor->description) <
               UMI_COMMAND_DESCRIPTION_CAPACITY &&
           strlen(descriptor->required_permission) <
               UMI_COMMAND_PERMISSION_CAPACITY;
}

static void umi_command_copy_snapshot(const UmiCommandEntry *entry,
                                      UmiCommandSnapshot *out_snapshot)
{
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    (void)snprintf(out_snapshot->command_id,
                   sizeof(out_snapshot->command_id),
                   "%s",
                   entry->command_id);
    (void)snprintf(out_snapshot->title,
                   sizeof(out_snapshot->title),
                   "%s",
                   entry->title);
    (void)snprintf(out_snapshot->category,
                   sizeof(out_snapshot->category),
                   "%s",
                   entry->category);
    (void)snprintf(out_snapshot->description,
                   sizeof(out_snapshot->description),
                   "%s",
                   entry->description);
    (void)snprintf(out_snapshot->required_permission,
                   sizeof(out_snapshot->required_permission),
                   "%s",
                   entry->required_permission);
    out_snapshot->flags = entry->flags;
}

UmiStatus umi_command_registry_create(UmiCommandRegistry **out_registry)
{
    UmiCommandRegistry *registry;
    UmiStatus status;

    if (out_registry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_registry = NULL;

    registry = (UmiCommandRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    status = umi_mutex_create(&registry->mutex);
    if (status != UMI_STATUS_OK) {
        free(registry);
        return status;
    }

    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_command_registry_destroy(UmiCommandRegistry *registry)
{
    if (registry == NULL) {
        return;
    }
    umi_mutex_destroy(registry->mutex);
    free(registry);
}

UmiStatus umi_command_registry_register(
    UmiCommandRegistry *registry,
    const UmiCommandDescriptor *descriptor)
{
    UmiCommandEntry *entry;

    if (registry == NULL || descriptor == NULL ||
        descriptor->structure_size < sizeof(*descriptor) ||
        descriptor->command_id == NULL || descriptor->command_id[0] == '\0' ||
        descriptor->title == NULL || descriptor->category == NULL ||
        descriptor->description == NULL ||
        descriptor->required_permission == NULL ||
        descriptor->handler == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (!umi_command_strings_fit(descriptor)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)umi_mutex_lock(registry->mutex);

    if (umi_command_find_index(registry, descriptor->command_id) != SIZE_MAX) {
        (void)umi_mutex_unlock(registry->mutex);
        return UMI_STATUS_ALREADY_EXISTS;
    }

    if (registry->count >= UMI_COMMAND_REGISTRY_MAX) {
        (void)umi_mutex_unlock(registry->mutex);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    entry = &registry->entries[registry->count++];
    (void)memset(entry, 0, sizeof(*entry));
    (void)snprintf(entry->command_id,
                   sizeof(entry->command_id),
                   "%s",
                   descriptor->command_id);
    (void)snprintf(entry->title,
                   sizeof(entry->title),
                   "%s",
                   descriptor->title);
    (void)snprintf(entry->category,
                   sizeof(entry->category),
                   "%s",
                   descriptor->category);
    (void)snprintf(entry->description,
                   sizeof(entry->description),
                   "%s",
                   descriptor->description);
    (void)snprintf(entry->required_permission,
                   sizeof(entry->required_permission),
                   "%s",
                   descriptor->required_permission);
    entry->flags = descriptor->flags;
    entry->handler = descriptor->handler;
    entry->enabled = descriptor->enabled;
    entry->user_data = descriptor->user_data;

    (void)umi_mutex_unlock(registry->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_command_registry_unregister(UmiCommandRegistry *registry,
                                          const char *command_id)
{
    size_t index;
    size_t move_count;

    if (registry == NULL || command_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)umi_mutex_lock(registry->mutex);
    index = umi_command_find_index(registry, command_id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(registry->mutex);
        return UMI_STATUS_NOT_FOUND;
    }

    move_count = registry->count - index - 1U;
    if (move_count > 0U) {
        (void)memmove(&registry->entries[index],
                      &registry->entries[index + 1U],
                      move_count * sizeof(registry->entries[0]));
    }
    registry->count -= 1U;
    (void)memset(&registry->entries[registry->count],
                 0,
                 sizeof(registry->entries[0]));

    (void)umi_mutex_unlock(registry->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_command_registry_snapshot(
    const UmiCommandRegistry *registry,
    const char *command_id,
    UmiCommandSnapshot *out_snapshot)
{
    size_t index;
    UmiCommandRegistry *mutable_registry;

    if (registry == NULL || command_id == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    mutable_registry = (UmiCommandRegistry *)registry;
    (void)umi_mutex_lock(mutable_registry->mutex);
    index = umi_command_find_index(registry, command_id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(mutable_registry->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    umi_command_copy_snapshot(&registry->entries[index], out_snapshot);
    (void)umi_mutex_unlock(mutable_registry->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_command_registry_at(const UmiCommandRegistry *registry,
                                  size_t index,
                                  UmiCommandSnapshot *out_snapshot)
{
    UmiCommandRegistry *mutable_registry;

    if (registry == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    mutable_registry = (UmiCommandRegistry *)registry;
    (void)umi_mutex_lock(mutable_registry->mutex);
    if (index >= registry->count) {
        (void)umi_mutex_unlock(mutable_registry->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    umi_command_copy_snapshot(&registry->entries[index], out_snapshot);
    (void)umi_mutex_unlock(mutable_registry->mutex);
    return UMI_STATUS_OK;
}

size_t umi_command_registry_count(const UmiCommandRegistry *registry)
{
    size_t count;
    UmiCommandRegistry *mutable_registry;

    if (registry == NULL) {
        return 0U;
    }

    mutable_registry = (UmiCommandRegistry *)registry;
    (void)umi_mutex_lock(mutable_registry->mutex);
    count = registry->count;
    (void)umi_mutex_unlock(mutable_registry->mutex);
    return count;
}

int umi_command_registry_is_enabled(const UmiCommandRegistry *registry,
                                    const char *command_id,
                                    const char *argument)
{
    size_t index;
    UmiRegisteredCommandEnabledFn enabled;
    void *user_data;
    UmiCommandRegistry *mutable_registry;

    if (registry == NULL || command_id == NULL) {
        return 0;
    }

    mutable_registry = (UmiCommandRegistry *)registry;
    (void)umi_mutex_lock(mutable_registry->mutex);
    index = umi_command_find_index(registry, command_id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(mutable_registry->mutex);
        return 0;
    }
    enabled = registry->entries[index].enabled;
    user_data = registry->entries[index].user_data;
    (void)umi_mutex_unlock(mutable_registry->mutex);

    return enabled == NULL || enabled(user_data, argument != NULL ? argument : "");
}

UmiStatus umi_command_registry_execute(UmiCommandRegistry *registry,
                                       const char *command_id,
                                       const char *argument,
                                       char *out_message,
                                       size_t message_capacity)
{
    size_t index;
    UmiRegisteredCommandHandler handler;
    UmiRegisteredCommandEnabledFn enabled;
    void *user_data;

    if (registry == NULL || command_id == NULL ||
        (out_message == NULL && message_capacity > 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (out_message != NULL && message_capacity > 0U) {
        out_message[0] = '\0';
    }

    (void)umi_mutex_lock(registry->mutex);
    index = umi_command_find_index(registry, command_id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(registry->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    handler = registry->entries[index].handler;
    enabled = registry->entries[index].enabled;
    user_data = registry->entries[index].user_data;
    (void)umi_mutex_unlock(registry->mutex);

    if (enabled != NULL &&
        !enabled(user_data, argument != NULL ? argument : "")) {
        return UMI_STATUS_INVALID_STATE;
    }

    return handler(user_data,
                   argument != NULL ? argument : "",
                   out_message,
                   message_capacity);
}
