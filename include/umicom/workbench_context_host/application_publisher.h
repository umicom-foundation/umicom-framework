/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/application_publisher.h
 *
 * PURPOSE:
 *   Publish application-launcher selections for Desk and cross-application navigation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_APPLICATION_PUBLISHER_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_APPLICATION_PUBLISHER_H
#include "umicom/workbench_context_host/host.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the workbench context host publish application operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_host_publish_application(
    UmiWorkbenchContextHost *host,
    const char *group_id,
    const char *source_panel_id,
    const char *context_id,
    const char * application_target_id,
    const char * runtime_state,
    const char * taskbar_group,
    const char * layout_id,
    uint64_t now_ms);
#ifdef __cplusplus
}
#endif
#endif
