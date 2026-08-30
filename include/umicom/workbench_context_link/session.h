/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/session.h
 *
 * PURPOSE:
 *   Define the reusable context-link workbench session contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_SESSION_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_SESSION_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkSession {
    uint32_t structure_size;
    char session_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char workspace_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char layout_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkSession;

void umi_workbench_context_link_session_init(UmiWorkbenchContextLinkSession *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_session_validate(
    const UmiWorkbenchContextLinkSession *record);
UmiStatus umi_workbench_context_link_session_copy(
    UmiWorkbenchContextLinkSession *destination,
    const UmiWorkbenchContextLinkSession *source);
uint64_t umi_workbench_context_link_session_hash(
    const UmiWorkbenchContextLinkSession *record);
UmiStatus umi_workbench_context_link_session_set_primary(
    UmiWorkbenchContextLinkSession *record,
    const char *value);
UmiStatus umi_workbench_context_link_session_set_secondary(
    UmiWorkbenchContextLinkSession *record,
    const char *value);
void umi_workbench_context_link_session_touch(
    UmiWorkbenchContextLinkSession *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
