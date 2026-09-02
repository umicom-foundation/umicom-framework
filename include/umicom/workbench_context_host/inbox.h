/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/inbox.h
 *
 * PURPOSE:
 *   Provide a bounded dynamic FIFO inbox for typed contexts delivered to one panel.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_INBOX_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_INBOX_H
#include "umicom/workbench_context_host/delivery.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the workbench context host inbox data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextHostInbox {
    char endpoint_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char panel_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    UmiWorkbenchContextHostDelivery *items;
    size_t count;
    size_t capacity;
    uint64_t dropped_count;
    uint64_t revision;
} UmiWorkbenchContextHostInbox;
/**
 * Initialise workbench context host inbox from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_context_host_inbox_init(
    UmiWorkbenchContextHostInbox *inbox,
    const char *endpoint_id,
    const char *panel_id);
/**
 * Release or reset state held by workbench context host inbox so the same storage can be
 * reused safely.
 */
void umi_workbench_context_host_inbox_destroy(
    UmiWorkbenchContextHostInbox *inbox);
/**
 * Provide the workbench context host inbox push operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_host_inbox_push(
    UmiWorkbenchContextHostInbox *inbox,
    const UmiWorkbenchContextHostDelivery *delivery);
/**
 * Provide the workbench context host inbox pop operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_host_inbox_pop(
    UmiWorkbenchContextHostInbox *inbox,
    UmiWorkbenchContextHostDelivery *out_delivery);
/**
 * Provide the workbench context host inbox peek operation used by this module and its
 * client applications.
 */
const UmiWorkbenchContextHostDelivery *umi_workbench_context_host_inbox_peek(
    const UmiWorkbenchContextHostInbox *inbox);
/**
 * Release or reset state held by workbench context host inbox so the same storage can be
 * reused safely.
 */
void umi_workbench_context_host_inbox_clear(
    UmiWorkbenchContextHostInbox *inbox);
#ifdef __cplusplus
}
#endif
#endif
