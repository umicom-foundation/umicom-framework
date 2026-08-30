/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/terminal_publisher.h
 *
 * PURPOSE:
 *   Publish terminal session selection and working-directory metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_TERMINAL_PUBLISHER_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_TERMINAL_PUBLISHER_H
#include "umicom/workbench_context_host/host.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_workbench_context_host_publish_terminal(
    UmiWorkbenchContextHost *host,
    const char *group_id,
    const char *source_panel_id,
    const char *context_id,
    const char * session_id,
    const char * shell_id,
    const char * working_directory,
    const char * command_text,
    int64_t exit_code,
    uint64_t now_ms);
#ifdef __cplusplus
}
#endif
#endif
