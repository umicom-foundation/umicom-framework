/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/workspace_publisher.h
 *
 * PURPOSE:
 *   Publish canonical workspace contexts when workbench profiles, perspectives and layouts change.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_WORKSPACE_PUBLISHER_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_WORKSPACE_PUBLISHER_H
#include "umicom/workbench_context_host/host.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_workbench_context_host_publish_workspace(
    UmiWorkbenchContextHost * host,
    const char * group_id,
    const char * source_panel_id,
    const char * context_id,
    const char * workspace_id,
    const char * profile_id,
    const char * root_path,
    const char * perspective_id,
    const char * layout_id,
    bool trusted,
    uint64_t now_ms);
#ifdef __cplusplus
}
#endif
#endif
