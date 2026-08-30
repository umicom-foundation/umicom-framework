/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/publication_guard.h
 *
 * PURPOSE:
 *   Suppress accidental feedback loops and redundant publications before routing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_PUBLICATION_GUARD_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_PUBLICATION_GUARD_H
#include "umicom/workbench_context_host/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiWorkbenchContextHostPublicationGuard {
    char source_panel_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    uint64_t content_hash;
    uint64_t last_published_ms;
    uint64_t duplicate_count;
    uint64_t accepted_count;
    uint64_t revision;
} UmiWorkbenchContextHostPublicationGuard;
void umi_workbench_context_host_publication_guard_init(
    UmiWorkbenchContextHostPublicationGuard *guard);
bool umi_workbench_context_host_publication_guard_accept(
    UmiWorkbenchContextHostPublicationGuard *guard,
    const char *source_panel_id,const char *group_id,
    const UmiContextPayload *payload,uint64_t now_ms);
#ifdef __cplusplus
}
#endif
#endif
