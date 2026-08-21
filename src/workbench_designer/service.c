/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/service.c
 *
 * PURPOSE:
 *   Implement Designer service ownership, session limits and lifecycle events.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/service.h"
#include "internal.h"
#include <stdio.h>


struct UmiWorkbenchDesignerService {
    UmiWorkbenchDesignerServiceConfig config;
    UmiWorkbenchDesignerSession *sessions[UMI_WORKBENCH_DESIGNER_MAX_SESSIONS];
    size_t count;
    char active_session_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    UmiWorkbenchDesignerEventBus event_bus;
    uint64_t revision;
};

UmiWorkbenchDesignerServiceConfig umi_workbench_designer_service_config_default(void)
{
    UmiWorkbenchDesignerServiceConfig config;
    config.maximum_sessions = UMI_WORKBENCH_DESIGNER_MAX_SESSIONS;
    config.close_clean_sessions_automatically = false;
    config.publish_events = true;
    return config;
}

UmiStatus umi_workbench_designer_service_create(
    const UmiWorkbenchDesignerServiceConfig *config,
    UmiWorkbenchDesignerService **out_service)
{
    UmiWorkbenchDesignerService *service;
    if (out_service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    service = (UmiWorkbenchDesignerService *)calloc(1U, sizeof(*service));
    if (service == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    service->config = config != NULL
        ? *config : umi_workbench_designer_service_config_default();
    if (service->config.maximum_sessions == 0U ||
        service->config.maximum_sessions > UMI_WORKBENCH_DESIGNER_MAX_SESSIONS) {
        free(service);
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_workbench_designer_event_bus_init(&service->event_bus);
    *out_service = service;
    return UMI_STATUS_OK;
}

void umi_workbench_designer_service_destroy(UmiWorkbenchDesignerService *service)
{
    size_t index;
    if (service == NULL) return;
    for (index = 0U; index < service->count; ++index) {
        umi_workbench_designer_session_destroy(service->sessions[index]);
    }
    free(service);
}

UmiWorkbenchDesignerSession *umi_workbench_designer_service_find(
    UmiWorkbenchDesignerService *service,
    const char *session_id)
{
    size_t index;
    if (service == NULL || session_id == NULL) return NULL;
    for (index = 0U; index < service->count; ++index) {
        if (strcmp(umi_workbench_designer_session_id(service->sessions[index]), session_id) == 0) {
            return service->sessions[index];
        }
    }
    return NULL;
}

const UmiWorkbenchDesignerSession *umi_workbench_designer_service_find_const(
    const UmiWorkbenchDesignerService *service,
    const char *session_id)
{
    return umi_workbench_designer_service_find(
        (UmiWorkbenchDesignerService *)service, session_id);
}

static void service_publish(
    UmiWorkbenchDesignerService *service,
    UmiWorkbenchDesignerEventKind kind,
    const char *session_id,
    const char *layout_id,
    const char *message)
{
    UmiWorkbenchDesignerEvent event;
    int written;
    if (!service->config.publish_events) return;
    (void)memset(&event, 0, sizeof(event));
    event.kind = kind;
    written = snprintf(event.event_id, sizeof(event.event_id), "designer-event-%llu",
        (unsigned long long)(service->event_bus.next_sequence));
    if (written < 0 || (size_t)written >= sizeof(event.event_id)) return;
    if (session_id != NULL) (void)umi_workbench_designer_copy_text(event.session_id, sizeof(event.session_id), session_id);
    if (layout_id != NULL) (void)umi_workbench_designer_copy_text(event.layout_id, sizeof(event.layout_id), layout_id);
    if (message != NULL) (void)umi_workbench_designer_copy_text(event.message, sizeof(event.message), message);
    (void)umi_workbench_designer_event_publish(&service->event_bus, &event);
}

UmiStatus umi_workbench_designer_service_open(
    UmiWorkbenchDesignerService *service,
    const char *session_id,
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchDesignerSessionConfig *config,
    UmiWorkbenchDesignerSession **out_session)
{
    UmiWorkbenchDesignerSession *session;
    UmiStatus status;
    if (service == NULL || session_id == NULL || document == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_workbench_designer_service_find(service, session_id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    if (service->count >= service->config.maximum_sessions) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    status = umi_workbench_designer_session_create(
        session_id, document, config, &session);
    if (status != UMI_STATUS_OK) return status;
    service->sessions[service->count++] = session;
    if (service->active_session_id[0] == '\0') {
        (void)umi_workbench_designer_copy_text(
            service->active_session_id,
            sizeof(service->active_session_id), session_id);
    }
    service->revision += 1U;
    service_publish(
        service, UMI_WORKBENCH_DESIGNER_EVENT_SESSION_OPENED,
        session_id, document->identity.layout_id, "Designer session opened");
    if (out_session != NULL) *out_session = session;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_designer_service_close(
    UmiWorkbenchDesignerService *service,
    const char *session_id,
    bool discard_dirty)
{
    size_t index;
    if (service == NULL || session_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < service->count; ++index) {
        UmiWorkbenchDesignerSession *session = service->sessions[index];
        if (strcmp(umi_workbench_designer_session_id(session), session_id) != 0) continue;
        if (umi_workbench_designer_session_dirty(session) && !discard_dirty) {
            return UMI_STATUS_BUSY;
        }
        service_publish(
            service, UMI_WORKBENCH_DESIGNER_EVENT_SESSION_CLOSED,
            session_id,
            umi_workbench_designer_session_document_const(session)->identity.layout_id,
            "Designer session closed");
        umi_workbench_designer_session_destroy(session);
        for (; index + 1U < service->count; ++index) {
            service->sessions[index] = service->sessions[index + 1U];
        }
        service->count -= 1U;
        if (strcmp(service->active_session_id, session_id) == 0) {
            service->active_session_id[0] = '\0';
            if (service->count > 0U) {
                (void)umi_workbench_designer_copy_text(
                    service->active_session_id,
                    sizeof(service->active_session_id),
                    umi_workbench_designer_session_id(service->sessions[0]));
            }
        }
        service->revision += 1U;
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_workbench_designer_service_activate(
    UmiWorkbenchDesignerService *service,
    const char *session_id)
{
    UmiWorkbenchDesignerSession *session;
    if (service == NULL || session_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    session = umi_workbench_designer_service_find(service, session_id);
    if (session == NULL) return UMI_STATUS_NOT_FOUND;
    (void)umi_workbench_designer_copy_text(
        service->active_session_id,
        sizeof(service->active_session_id), session_id);
    service->revision += 1U;
    service_publish(
        service, UMI_WORKBENCH_DESIGNER_EVENT_SESSION_ACTIVATED,
        session_id,
        umi_workbench_designer_session_document_const(session)->identity.layout_id,
        "Designer session activated");
    return UMI_STATUS_OK;
}

UmiWorkbenchDesignerSession *umi_workbench_designer_service_active(
    UmiWorkbenchDesignerService *service)
{
    if (service == NULL || service->active_session_id[0] == '\0') return NULL;
    return umi_workbench_designer_service_find(service, service->active_session_id);
}

size_t umi_workbench_designer_service_session_count(
    const UmiWorkbenchDesignerService *service)
{ return service != NULL ? service->count : 0U; }

size_t umi_workbench_designer_service_dirty_count(
    const UmiWorkbenchDesignerService *service)
{
    size_t index;
    size_t count = 0U;
    if (service == NULL) return 0U;
    for (index = 0U; index < service->count; ++index) {
        if (umi_workbench_designer_session_dirty(service->sessions[index])) count += 1U;
    }
    return count;
}

UmiWorkbenchDesignerEventBus *umi_workbench_designer_service_event_bus(
    UmiWorkbenchDesignerService *service)
{ return service != NULL ? &service->event_bus : NULL; }

uint64_t umi_workbench_designer_service_revision(
    const UmiWorkbenchDesignerService *service)
{ return service != NULL ? service->revision : 0U; }
