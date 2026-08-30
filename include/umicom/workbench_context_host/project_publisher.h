/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/project_publisher.h
 *
 * PURPOSE:
 *   Publish canonical project contexts from Studio project, build and source-control views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_PROJECT_PUBLISHER_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_PROJECT_PUBLISHER_H
#include "umicom/workbench_context_host/host.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_workbench_context_host_publish_project(
    UmiWorkbenchContextHost * host,
    const char * group_id,
    const char * source_panel_id,
    const char * context_id,
    const char * project_id,
    const char * repository_id,
    const char * root_path,
    const char * target_id,
    const char * configuration_id,
    const char * language_id,
    uint64_t now_ms);
#ifdef __cplusplus
}
#endif
#endif
