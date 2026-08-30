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
void umi_workbench_context_host_delivery_init(
    UmiWorkbenchContextHostDelivery *delivery,
    const char *delivery_id);
UmiStatus umi_workbench_context_host_delivery_validate(
    const UmiWorkbenchContextHostDelivery *delivery);
#ifdef __cplusplus
}
#endif
#endif
