/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/delivery_plan.h
 *
 * PURPOSE:
 *   Define the reusable context delivery plan contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_DELIVERY_PLAN_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_DELIVERY_PLAN_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link delivery plan data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextLinkDeliveryPlan {
    uint32_t structure_size;
    char plan_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char source_panel_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    UmiContextKind context_kind;
    UmiContextChannelColour colour;
    UmiWorkbenchContextLinkMode mode;
    UmiWorkbenchContextLinkState state;
    UmiWorkbenchContextLinkOrigin origin;
    UmiWorkbenchContextLinkPriority priority;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchContextLinkDeliveryPlan;

/**
 * Initialise workbench context link delivery plan from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_delivery_plan_init(UmiWorkbenchContextLinkDeliveryPlan *record,
                                           const char *identity);
/**
 * Check that workbench context link delivery plan satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_link_delivery_plan_validate(
    const UmiWorkbenchContextLinkDeliveryPlan *record);
/**
 * Copy workbench context link delivery plan into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_delivery_plan_copy(
    UmiWorkbenchContextLinkDeliveryPlan *destination,
    const UmiWorkbenchContextLinkDeliveryPlan *source);
/**
 * Provide the workbench context link delivery plan hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_link_delivery_plan_hash(
    const UmiWorkbenchContextLinkDeliveryPlan *record);
/**
 * Provide the workbench context link delivery plan set primary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_delivery_plan_set_primary(
    UmiWorkbenchContextLinkDeliveryPlan *record,
    const char *value);
/**
 * Provide the workbench context link delivery plan set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_delivery_plan_set_secondary(
    UmiWorkbenchContextLinkDeliveryPlan *record,
    const char *value);
/**
 * Provide the workbench context link delivery plan touch operation used by this module and
 * its client applications.
 */
void umi_workbench_context_link_delivery_plan_touch(
    UmiWorkbenchContextLinkDeliveryPlan *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
