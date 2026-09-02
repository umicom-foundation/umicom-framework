/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/session.h
 *
 * PURPOSE:
 *   Capture and restore active group plus panel assignments without persisting transient context payloads.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_SESSION_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_SESSION_H
#include "umicom/workbench_context_host/host.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_WORKBENCH_CONTEXT_HOST_SESSION_TEXT_CAPACITY 65536U
/**
 * Represent the workbench context host session assignment data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextHostSessionAssignment {
    char endpoint_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    UmiWorkbenchContextLinkMode mode;
} UmiWorkbenchContextHostSessionAssignment;
/**
 * Represent the workbench context host session data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextHostSession {
    uint32_t structure_size;
    char host_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char active_group_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    UmiWorkbenchContextHostSessionAssignment assignments[
        UMI_WORKBENCH_CONTEXT_HOST_MAX_SESSION_ASSIGNMENTS];
    size_t assignment_count;
    uint64_t source_revision;
} UmiWorkbenchContextHostSession;
/**
 * Initialise workbench context host session from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_host_session_init(UmiWorkbenchContextHostSession *session);
/**
 * Provide the workbench context host session capture operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_host_session_capture(
    const UmiWorkbenchContextHost *host,UmiWorkbenchContextHostSession *out_session);
/**
 * Perform workbench context host session through the module contract so client
 * applications do not duplicate its policy.
 */
UmiStatus umi_workbench_context_host_session_apply(
    UmiWorkbenchContextHost *host,const UmiWorkbenchContextHostSession *session);
/**
 * Write workbench context host session in its stable representation and report capacity or
 * input failures to the caller.
 */
UmiStatus umi_workbench_context_host_session_encode(
    const UmiWorkbenchContextHostSession *session,char *out_text,size_t capacity);
/**
 * Read workbench context host session into validated module state and return a status when
 * input cannot be used.
 */
UmiStatus umi_workbench_context_host_session_decode(
    const char *text,UmiWorkbenchContextHostSession *out_session);
#ifdef __cplusplus
}
#endif
#endif
