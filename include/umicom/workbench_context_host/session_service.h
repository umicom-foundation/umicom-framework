/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/session_service.h
 *
 * PURPOSE:
 *   Persist context-host assignments through the Framework Session Store using bounded chunked values.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_SESSION_SERVICE_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_SESSION_SERVICE_H

#include "umicom/platform/session_store.h"
#include "umicom/workbench_context_host/session.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_WORKBENCH_CONTEXT_HOST_SESSION_CHUNK_CAPACITY 1800U
#define UMI_WORKBENCH_CONTEXT_HOST_SESSION_MAX_CHUNKS 40U

UmiStatus umi_workbench_context_host_session_save(
    const UmiWorkbenchContextHost *host,
    UmiSessionStore *store,
    const char *key_prefix);
UmiStatus umi_workbench_context_host_session_restore(
    UmiWorkbenchContextHost *host,
    const UmiSessionStore *store,
    const char *key_prefix,
    bool *out_restored);
UmiStatus umi_workbench_context_host_session_remove(
    UmiSessionStore *store,
    const char *key_prefix);

#ifdef __cplusplus
}
#endif

#endif
