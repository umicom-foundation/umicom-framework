/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/service.h
 *
 * PURPOSE:
 *   Provide the Framework-owned routing service for linked panels and typed contexts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_SERVICE_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_SERVICE_H

#include "umicom/workbench_context_link/group_catalogue.h"
#include "umicom/workbench_context_link/binding_registry.h"
#include "umicom/workbench_context_link/active_context_store.h"
#include "umicom/workbench_context_link/history_store.h"
#include "umicom/workbench_context_link/pin_store.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkDelivery {
    char binding_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char panel_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char application_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    UmiContextKind context_kind;
    UmiContextChannelColour colour;
    uint64_t sequence;
} UmiWorkbenchContextLinkDelivery;

typedef struct UmiWorkbenchContextLinkDeliveryBatch {
    UmiWorkbenchContextLinkDelivery items[UMI_WORKBENCH_CONTEXT_LINK_MAX_DELIVERIES];
    size_t count;
    uint64_t sequence;
} UmiWorkbenchContextLinkDeliveryBatch;

typedef struct UmiWorkbenchContextLinkMetrics {
    uint64_t publish_count;
    uint64_t delivery_count;
    uint64_t rejected_count;
    uint64_t duplicate_count;
    uint64_t pin_count;
    uint64_t navigation_count;
} UmiWorkbenchContextLinkMetrics;

typedef struct UmiWorkbenchContextLinkService {
    UmiWorkbenchContextLinkGroupCatalogue groups;
    UmiWorkbenchContextLinkBindingRegistry bindings;
    UmiWorkbenchContextLinkActiveContextStore active;
    UmiWorkbenchContextLinkHistoryStore history;
    UmiWorkbenchContextLinkPinStore pins;
    UmiWorkbenchContextLinkMetrics metrics;
    uint64_t sequence;
    uint64_t revision;
    bool suspended;
} UmiWorkbenchContextLinkService;

void umi_workbench_context_link_service_init(
    UmiWorkbenchContextLinkService *service);
void umi_workbench_context_link_service_destroy(
    UmiWorkbenchContextLinkService *service);
UmiStatus umi_workbench_context_link_service_define_group(
    UmiWorkbenchContextLinkService *service,
    const char *group_id,
    const char *title,
    UmiContextChannelColour colour,
    uint64_t allowed_kinds_mask,
    UmiWorkbenchContextLinkMode default_mode);
UmiStatus umi_workbench_context_link_service_bind_panel(
    UmiWorkbenchContextLinkService *service,
    const char *binding_id,
    const char *panel_id,
    const char *application_id,
    const char *group_id,
    UmiContextKind context_kind,
    UmiWorkbenchContextLinkMode mode);
UmiStatus umi_workbench_context_link_service_unbind_panel(
    UmiWorkbenchContextLinkService *service,
    const char *panel_id);
UmiStatus umi_workbench_context_link_service_publish(
    UmiWorkbenchContextLinkService *service,
    const char *group_id,
    const char *source_panel_id,
    const UmiContextPayload *payload,
    uint64_t now_ms,
    UmiWorkbenchContextLinkDeliveryBatch *out_deliveries);
const UmiWorkbenchContextLinkActiveSlot *
umi_workbench_context_link_service_current(
    const UmiWorkbenchContextLinkService *service,
    const char *group_id);
UmiStatus umi_workbench_context_link_service_pin_current(
    UmiWorkbenchContextLinkService *service,
    const char *pin_id,
    const char *group_id,
    uint64_t now_ms);
UmiStatus umi_workbench_context_link_service_restore_previous(
    UmiWorkbenchContextLinkService *service,
    const char *group_id,
    UmiWorkbenchContextLinkDeliveryBatch *out_deliveries);
UmiStatus umi_workbench_context_link_service_restore_next(
    UmiWorkbenchContextLinkService *service,
    const char *group_id,
    UmiWorkbenchContextLinkDeliveryBatch *out_deliveries);
void umi_workbench_context_link_service_set_suspended(
    UmiWorkbenchContextLinkService *service,
    bool suspended);
uint64_t umi_workbench_context_link_service_revision(
    const UmiWorkbenchContextLinkService *service);

#ifdef __cplusplus
}
#endif

#endif
