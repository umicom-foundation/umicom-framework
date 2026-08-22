/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/selection_publisher.h
 *
 * PURPOSE:
 *   Publish generic selection contexts from trees, tables, inspectors and list views.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_SELECTION_PUBLISHER_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_SELECTION_PUBLISHER_H
#include "umicom/workbench_context_host/host.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_workbench_context_host_publish_selection(
    UmiWorkbenchContextHost * host,
    const char * group_id,
    const char * source_panel_id,
    const char * context_id,
    const char * selection_id,
    const char * selection_type,
    const char * primary_id,
    const char * secondary_id,
    uint64_t index,
    uint64_t count,
    uint64_t now_ms);
#ifdef __cplusplus
}
#endif
#endif
