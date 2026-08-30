/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/components/host.c
 *
 * PURPOSE:
 *   Implement one toolkit-neutral reusable UI component concern.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */
#include "umicom/ui/components/host.h"

#include <stdlib.h>
#include <string.h>

typedef struct UmiUiComponentHostEntry {
    UmiUiComponentHostSnapshot snapshot;
    UmiUiViewPresentation presentation;
} UmiUiComponentHostEntry;

struct UmiUiComponentHostService {
    UmiUiComponentHostEntry entries[UMI_UI_COMPONENT_HOST_MAX];
    size_t count;
    UmiUiViewFactoryRegistry *view_factories;
    uint64_t revision;
};

static size_t find_entry(
    const UmiUiComponentHostService *service,
    const char *host_id)
{
    size_t index;
    if (service == NULL || host_id == NULL) return SIZE_MAX;
    for (index = 0U; index < service->count; ++index) {
        if (strcmp(service->entries[index].snapshot.descriptor.host_id,
                   host_id) == 0) return index;
    }
    return SIZE_MAX;
}

static int valid_descriptor(const UmiUiComponentHostDescriptor *descriptor)
{
    return descriptor != NULL &&
        descriptor->structure_size >= sizeof(*descriptor) &&
        umi_ui_id_is_valid(descriptor->host_id) &&
        umi_ui_id_is_valid(descriptor->component_id) &&
        umi_ui_id_is_valid(descriptor->owner_application_id) &&
        umi_ui_id_is_valid(descriptor->view_type) &&
        umi_ui_id_is_valid(descriptor->view_id) &&
        descriptor->title[0] != '\0';
}

static UmiStatus refresh_entry(
    UmiUiComponentHostService *service,
    UmiUiComponentHostEntry *entry)
{
    UmiUiViewPresentation *presentation =
        (UmiUiViewPresentation *)calloc(1U, sizeof(*presentation));
    UmiStatus status;
    if (presentation == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_ui_view_presentation_build(
        service->view_factories,
        entry->snapshot.descriptor.view_type,
        entry->snapshot.descriptor.view_id,
        presentation);
    if (status != UMI_STATUS_OK) {
        entry->snapshot.state = UMI_UI_COMPONENT_HOST_FAILED;
        entry->snapshot.revision += 1U;
        service->revision += 1U;
        free(presentation);
        return status;
    }
    entry->presentation = *presentation;
    if (entry->snapshot.state == UMI_UI_COMPONENT_HOST_FAILED)
        entry->snapshot.state = UMI_UI_COMPONENT_HOST_MOUNTED;
    entry->snapshot.presentation_property_count =
        presentation->property_count;
    entry->snapshot.revision += 1U;
    service->revision += 1U;
    free(presentation);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_component_host_init(UmiUiComponentHost *host,
                                     const char *kind,
                                     void *adapter_handle)
{
    if (host == NULL || kind == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    const size_t length = strlen(kind);
    if (length >= sizeof(host->kind)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(host->kind, kind, length + 1U);
    host->adapter_handle = adapter_handle;
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_component_host_service_create(
    UmiUiComponentHostService **out_service)
{
    UmiUiComponentHostService *service;
    UmiStatus status;
    if (out_service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_service = NULL;
    service = (UmiUiComponentHostService *)calloc(1U, sizeof(*service));
    if (service == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_ui_view_factory_registry_create(&service->view_factories);
    if (status != UMI_STATUS_OK) {
        free(service);
        return status;
    }
    *out_service = service;
    return UMI_STATUS_OK;
}

void umi_ui_component_host_service_destroy(
    UmiUiComponentHostService *service)
{
    if (service == NULL) return;
    umi_ui_view_factory_registry_destroy(service->view_factories);
    free(service);
}

UmiUiViewFactoryRegistry *umi_ui_component_host_view_factories(
    UmiUiComponentHostService *service)
{
    return service != NULL ? service->view_factories : NULL;
}

UmiStatus umi_ui_component_host_mount(
    UmiUiComponentHostService *service,
    const UmiUiComponentHostDescriptor *descriptor)
{
    UmiUiComponentHostEntry *entry;
    UmiStatus status;
    if (service == NULL || !valid_descriptor(descriptor))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (find_entry(service, descriptor->host_id) != SIZE_MAX)
        return UMI_STATUS_ALREADY_EXISTS;
    if (service->count >= UMI_UI_COMPONENT_HOST_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    entry = &service->entries[service->count];
    (void)memset(entry, 0, sizeof(*entry));
    entry->snapshot.descriptor = *descriptor;
    entry->snapshot.descriptor.visible = descriptor->visible != 0;
    entry->snapshot.state = UMI_UI_COMPONENT_HOST_MOUNTED;
    status = refresh_entry(service, entry);
    if (status != UMI_STATUS_OK) {
        (void)memset(entry, 0, sizeof(*entry));
        return status;
    }
    service->count += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_component_host_unmount(
    UmiUiComponentHostService *service,
    const char *host_id)
{
    size_t index;
    if (service == NULL || host_id == NULL || host_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_entry(service, host_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (index + 1U < service->count) {
        (void)memmove(&service->entries[index], &service->entries[index + 1U],
                      (service->count - index - 1U) *
                          sizeof(service->entries[0]));
    }
    service->count -= 1U;
    (void)memset(&service->entries[service->count], 0,
                 sizeof(service->entries[0]));
    service->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_component_host_activate(
    UmiUiComponentHostService *service,
    const char *host_id)
{
    size_t index;
    size_t active_index;
    if (service == NULL || host_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_entry(service, host_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (service->entries[index].snapshot.state ==
            UMI_UI_COMPONENT_HOST_SUSPENDED ||
        service->entries[index].snapshot.state ==
            UMI_UI_COMPONENT_HOST_FAILED) return UMI_STATUS_INVALID_STATE;
    for (active_index = 0U; active_index < service->count; ++active_index) {
        UmiUiComponentHostSnapshot *snapshot =
            &service->entries[active_index].snapshot;
        if (snapshot->state == UMI_UI_COMPONENT_HOST_ACTIVE) {
            snapshot->state = UMI_UI_COMPONENT_HOST_MOUNTED;
            snapshot->revision += 1U;
        }
    }
    service->entries[index].snapshot.state = UMI_UI_COMPONENT_HOST_ACTIVE;
    service->entries[index].snapshot.revision += 1U;
    service->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_component_host_suspend(
    UmiUiComponentHostService *service,
    const char *host_id)
{
    size_t index;
    if (service == NULL || host_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_entry(service, host_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (service->entries[index].snapshot.state ==
        UMI_UI_COMPONENT_HOST_SUSPENDED) return UMI_STATUS_INVALID_STATE;
    service->entries[index].snapshot.state = UMI_UI_COMPONENT_HOST_SUSPENDED;
    service->entries[index].snapshot.revision += 1U;
    service->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_component_host_resume(
    UmiUiComponentHostService *service,
    const char *host_id)
{
    size_t index;
    if (service == NULL || host_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_entry(service, host_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (service->entries[index].snapshot.state !=
        UMI_UI_COMPONENT_HOST_SUSPENDED) return UMI_STATUS_INVALID_STATE;
    service->entries[index].snapshot.state = UMI_UI_COMPONENT_HOST_MOUNTED;
    service->entries[index].snapshot.revision += 1U;
    service->revision += 1U;
    return refresh_entry(service, &service->entries[index]);
}

UmiStatus umi_ui_component_host_show(
    UmiUiComponentHostService *service,
    const char *host_id,
    int visible)
{
    size_t index;
    if (service == NULL || host_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_entry(service, host_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    service->entries[index].snapshot.descriptor.visible = visible != 0;
    service->entries[index].snapshot.revision += 1U;
    service->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_component_host_refresh(
    UmiUiComponentHostService *service,
    const char *host_id)
{
    size_t index;
    if (service == NULL || host_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_entry(service, host_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (service->entries[index].snapshot.state ==
        UMI_UI_COMPONENT_HOST_SUSPENDED) return UMI_STATUS_INVALID_STATE;
    return refresh_entry(service, &service->entries[index]);
}

UmiStatus umi_ui_component_host_snapshot(
    const UmiUiComponentHostService *service,
    const char *host_id,
    UmiUiComponentHostSnapshot *out_snapshot)
{
    size_t index;
    if (service == NULL || host_id == NULL || out_snapshot == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_entry(service, host_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_snapshot = service->entries[index].snapshot;
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_component_host_at(
    const UmiUiComponentHostService *service,
    size_t index,
    UmiUiComponentHostSnapshot *out_snapshot)
{
    if (service == NULL || out_snapshot == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= service->count) return UMI_STATUS_NOT_FOUND;
    *out_snapshot = service->entries[index].snapshot;
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_component_host_presentation(
    const UmiUiComponentHostService *service,
    const char *host_id,
    UmiUiViewPresentation *out_presentation)
{
    size_t index;
    if (service == NULL || host_id == NULL || out_presentation == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_entry(service, host_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (service->entries[index].snapshot.state == UMI_UI_COMPONENT_HOST_FAILED)
        return UMI_STATUS_UNAVAILABLE;
    *out_presentation = service->entries[index].presentation;
    return UMI_STATUS_OK;
}

size_t umi_ui_component_host_count(
    const UmiUiComponentHostService *service)
{
    return service != NULL ? service->count : 0U;
}
