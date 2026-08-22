/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/session.h
 *
 * PURPOSE:
 *   Capture and restore active group plus panel assignments without persisting transient context payloads.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_SESSION_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_SESSION_H
#include "umicom/workbench_context_host/host.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_WORKBENCH_CONTEXT_HOST_SESSION_TEXT_CAPACITY 65536U
typedef struct UmiWorkbenchContextHostSessionAssignment {
    char endpoint_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    UmiWorkbenchContextLinkMode mode;
} UmiWorkbenchContextHostSessionAssignment;
typedef struct UmiWorkbenchContextHostSession {
    uint32_t structure_size;
    char host_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char active_group_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    UmiWorkbenchContextHostSessionAssignment assignments[
        UMI_WORKBENCH_CONTEXT_HOST_MAX_SESSION_ASSIGNMENTS];
    size_t assignment_count;
    uint64_t source_revision;
} UmiWorkbenchContextHostSession;
void umi_workbench_context_host_session_init(UmiWorkbenchContextHostSession *session);
UmiStatus umi_workbench_context_host_session_capture(
    const UmiWorkbenchContextHost *host,UmiWorkbenchContextHostSession *out_session);
UmiStatus umi_workbench_context_host_session_apply(
    UmiWorkbenchContextHost *host,const UmiWorkbenchContextHostSession *session);
UmiStatus umi_workbench_context_host_session_encode(
    const UmiWorkbenchContextHostSession *session,char *out_text,size_t capacity);
UmiStatus umi_workbench_context_host_session_decode(
    const char *text,UmiWorkbenchContextHostSession *out_session);
#ifdef __cplusplus
}
#endif
#endif
