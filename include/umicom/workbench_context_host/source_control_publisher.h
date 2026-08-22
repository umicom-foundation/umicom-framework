/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/source_control_publisher.h
 *
 * PURPOSE:
 *   Publish source-control selections as project contexts enriched with repository, branch, path and change metadata.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_SOURCE_CONTROL_PUBLISHER_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_SOURCE_CONTROL_PUBLISHER_H
#include "umicom/workbench_context_host/host.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_workbench_context_host_publish_source_control(
    UmiWorkbenchContextHost *host,
    const char *group_id,
    const char *source_panel_id,
    const char *context_id,
    const char * project_id,
    const char * repository_id,
    const char * root_path,
    const char * branch,
    const char * path,
    const char * change_kind,
    uint64_t now_ms);
#ifdef __cplusplus
}
#endif
#endif
