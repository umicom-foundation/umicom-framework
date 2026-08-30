/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/diagnostic_publisher.h
 *
 * PURPOSE:
 *   Publish source diagnostics as source-location contexts enriched with severity, code and message metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_DIAGNOSTIC_PUBLISHER_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_DIAGNOSTIC_PUBLISHER_H
#include "umicom/workbench_context_host/host.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_workbench_context_host_publish_diagnostic(
    UmiWorkbenchContextHost *host,
    const char *group_id,
    const char *source_panel_id,
    const char *context_id,
    const char * workspace_id,
    const char * file_path,
    uint32_t line,
    uint32_t column,
    const char * severity,
    const char * diagnostic_code,
    const char * message,
    uint64_t now_ms);
#ifdef __cplusplus
}
#endif
#endif
