/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/inbox_registry.h
 *
 * PURPOSE:
 *   Maintain endpoint-addressable delivery inboxes for linked workbench panels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_INBOX_REGISTRY_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_INBOX_REGISTRY_H
#include "umicom/workbench_context_host/inbox.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the workbench context host inbox registry data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextHostInboxRegistry {
    UmiWorkbenchContextHostInbox *items;
    size_t count;
    size_t capacity;
    uint64_t revision;
} UmiWorkbenchContextHostInboxRegistry;
/**
 * Initialise workbench context host inbox registry from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_host_inbox_registry_init(
    UmiWorkbenchContextHostInboxRegistry *registry);
/**
 * Release or reset state held by workbench context host inbox registry so the same storage
 * can be reused safely.
 */
void umi_workbench_context_host_inbox_registry_destroy(
    UmiWorkbenchContextHostInboxRegistry *registry);
/**
 * Provide the workbench context host inbox registry ensure operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_host_inbox_registry_ensure(
    UmiWorkbenchContextHostInboxRegistry *registry,
    const char *endpoint_id,const char *panel_id,
    UmiWorkbenchContextHostInbox **out_inbox);
/**
 * Find workbench context host inbox registry while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiWorkbenchContextHostInbox *umi_workbench_context_host_inbox_registry_find(
    UmiWorkbenchContextHostInboxRegistry *registry,const char *endpoint_id);
/**
 * Provide the workbench context host inbox registry find const operation used by this
 * module and its client applications.
 */
const UmiWorkbenchContextHostInbox *umi_workbench_context_host_inbox_registry_find_const(
    const UmiWorkbenchContextHostInboxRegistry *registry,const char *endpoint_id);
/**
 * Remove workbench context host inbox registry while keeping the remaining records in a
 * valid and discoverable state.
 */
UmiStatus umi_workbench_context_host_inbox_registry_remove(
    UmiWorkbenchContextHostInboxRegistry *registry,const char *endpoint_id);
#ifdef __cplusplus
}
#endif
#endif
