/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/command.h
 *
 * PURPOSE:
 *   Define toolkit-neutral context-host commands and execute them against the host service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_COMMAND_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_COMMAND_H
#include "umicom/workbench_context_host/host.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiWorkbenchContextHostCommand {
    uint32_t structure_size;
    char command_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    UmiWorkbenchContextHostCommandKind kind;
    char endpoint_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char argument[UMI_WORKBENCH_CONTEXT_HOST_TEXT_CAPACITY];
    UmiWorkbenchContextLinkMode mode;
    uint64_t timestamp_ms;
} UmiWorkbenchContextHostCommand;
void umi_workbench_context_host_command_init(
    UmiWorkbenchContextHostCommand *command,UmiWorkbenchContextHostCommandKind kind);
UmiStatus umi_workbench_context_host_command_execute(
    UmiWorkbenchContextHost *host,const UmiWorkbenchContextHostCommand *command);
#ifdef __cplusplus
}
#endif
#endif
