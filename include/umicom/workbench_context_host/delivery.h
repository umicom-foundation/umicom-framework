/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/delivery.h
 *
 * PURPOSE:
 *   Carry one typed context delivery from the Framework link router into a panel inbox.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_DELIVERY_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_DELIVERY_H
#include "umicom/workbench_context_host/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the workbench context host delivery data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextHostDelivery {
    uint32_t structure_size;
    char delivery_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char endpoint_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char panel_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char application_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char source_panel_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    UmiContextPayload payload;
    UmiWorkbenchContextHostDeliveryDisposition disposition;
    uint64_t sequence;
    uint64_t queued_at_ms;
    uint64_t consumed_at_ms;
    uint64_t revision;
} UmiWorkbenchContextHostDelivery;
/**
 * Initialise workbench context host delivery from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_host_delivery_init(
    UmiWorkbenchContextHostDelivery *delivery,
    const char *delivery_id);
/**
 * Check that workbench context host delivery satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_context_host_delivery_validate(
    const UmiWorkbenchContextHostDelivery *delivery);
#ifdef __cplusplus
}
#endif
#endif
