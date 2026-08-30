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
typedef struct UmiWorkbenchContextHostInboxRegistry {
    UmiWorkbenchContextHostInbox *items;
    size_t count;
    size_t capacity;
    uint64_t revision;
} UmiWorkbenchContextHostInboxRegistry;
void umi_workbench_context_host_inbox_registry_init(
    UmiWorkbenchContextHostInboxRegistry *registry);
void umi_workbench_context_host_inbox_registry_destroy(
    UmiWorkbenchContextHostInboxRegistry *registry);
UmiStatus umi_workbench_context_host_inbox_registry_ensure(
    UmiWorkbenchContextHostInboxRegistry *registry,
    const char *endpoint_id,const char *panel_id,
    UmiWorkbenchContextHostInbox **out_inbox);
UmiWorkbenchContextHostInbox *umi_workbench_context_host_inbox_registry_find(
    UmiWorkbenchContextHostInboxRegistry *registry,const char *endpoint_id);
const UmiWorkbenchContextHostInbox *umi_workbench_context_host_inbox_registry_find_const(
    const UmiWorkbenchContextHostInboxRegistry *registry,const char *endpoint_id);
UmiStatus umi_workbench_context_host_inbox_registry_remove(
    UmiWorkbenchContextHostInboxRegistry *registry,const char *endpoint_id);
#ifdef __cplusplus
}
#endif
#endif
