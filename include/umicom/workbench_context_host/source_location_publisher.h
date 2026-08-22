/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/source_location_publisher.h
 *
 * PURPOSE:
 *   Publish canonical source-location contexts from editors, diagnostics, search and debugger views.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_SOURCE_LOCATION_PUBLISHER_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_SOURCE_LOCATION_PUBLISHER_H
#include "umicom/workbench_context_host/host.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_workbench_context_host_publish_source_location(
    UmiWorkbenchContextHost * host,
    const char * group_id,
    const char * source_panel_id,
    const char * context_id,
    const char * workspace_id,
    const char * file_path,
    const char * symbol,
    uint32_t line,
    uint32_t column,
    uint32_t selection_length,
    uint64_t now_ms);
#ifdef __cplusplus
}
#endif
#endif
