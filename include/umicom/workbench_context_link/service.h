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

/**
 * Represent the workbench context link delivery data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextLinkDelivery {
    char binding_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char panel_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char application_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    UmiContextKind context_kind;
    UmiContextChannelColour colour;
    uint64_t sequence;
} UmiWorkbenchContextLinkDelivery;

/**
 * Represent the workbench context link delivery batch data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextLinkDeliveryBatch {
    UmiWorkbenchContextLinkDelivery items[UMI_WORKBENCH_CONTEXT_LINK_MAX_DELIVERIES];
    size_t count;
    uint64_t sequence;
} UmiWorkbenchContextLinkDeliveryBatch;

/**
 * Represent the workbench context link metrics data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextLinkMetrics {
    uint64_t publish_count;
    uint64_t delivery_count;
    uint64_t rejected_count;
    uint64_t duplicate_count;
    uint64_t pin_count;
    uint64_t navigation_count;
} UmiWorkbenchContextLinkMetrics;

/**
 * Represent the workbench context link service data shared with callers of this public
 * contract.
 */
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

/**
 * Initialise workbench context link service from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_service_init(
    UmiWorkbenchContextLinkService *service);
/**
 * Release or reset state held by workbench context link service so the same storage can be
 * reused safely.
 */
void umi_workbench_context_link_service_destroy(
    UmiWorkbenchContextLinkService *service);
/**
 * Provide the workbench context link service define group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_link_service_define_group(
    UmiWorkbenchContextLinkService *service,
    const char *group_id,
    const char *title,
    UmiContextChannelColour colour,
    uint64_t allowed_kinds_mask,
    UmiWorkbenchContextLinkMode default_mode);
/**
 * Provide the workbench context link service bind panel operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_link_service_bind_panel(
    UmiWorkbenchContextLinkService *service,
    const char *binding_id,
    const char *panel_id,
    const char *application_id,
    const char *group_id,
    UmiContextKind context_kind,
    UmiWorkbenchContextLinkMode mode);
/**
 * Provide the workbench context link service unbind panel operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_link_service_unbind_panel(
    UmiWorkbenchContextLinkService *service,
    const char *panel_id);
/**
 * Provide the workbench context link service publish operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_link_service_publish(
    UmiWorkbenchContextLinkService *service,
    const char *group_id,
    const char *source_panel_id,
    const UmiContextPayload *payload,
    uint64_t now_ms,
    UmiWorkbenchContextLinkDeliveryBatch *out_deliveries);
/**
 * Provide the workbench context link service current operation used by this module and its
 * client applications.
 */
const UmiWorkbenchContextLinkActiveSlot *
umi_workbench_context_link_service_current(
    const UmiWorkbenchContextLinkService *service,
    const char *group_id);
/**
 * Provide the workbench context link service pin current operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_link_service_pin_current(
    UmiWorkbenchContextLinkService *service,
    const char *pin_id,
    const char *group_id,
    uint64_t now_ms);
/**
 * Provide the workbench context link service restore previous operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_service_restore_previous(
    UmiWorkbenchContextLinkService *service,
    const char *group_id,
    UmiWorkbenchContextLinkDeliveryBatch *out_deliveries);
/**
 * Provide the workbench context link service restore next operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_link_service_restore_next(
    UmiWorkbenchContextLinkService *service,
    const char *group_id,
    UmiWorkbenchContextLinkDeliveryBatch *out_deliveries);
/**
 * Provide the workbench context link service set suspended operation used by this module
 * and its client applications.
 */
void umi_workbench_context_link_service_set_suspended(
    UmiWorkbenchContextLinkService *service,
    bool suspended);
/**
 * Provide the workbench context link service revision operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_link_service_revision(
    const UmiWorkbenchContextLinkService *service);

#ifdef __cplusplus
}
#endif

#endif
