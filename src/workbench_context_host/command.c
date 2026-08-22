/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/command.c
 *
 * PURPOSE:
 *   Execute context-group, navigation, pinning, inbox and lifecycle commands.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/command.h"
#include <string.h>
void umi_workbench_context_host_command_init(
    UmiWorkbenchContextHostCommand *command,UmiWorkbenchContextHostCommandKind kind)
{if(command){memset(command,0,sizeof(*command));command->structure_size=(uint32_t)sizeof(*command);command->kind=kind;command->mode=UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;}}
UmiStatus umi_workbench_context_host_command_execute(
    UmiWorkbenchContextHost *host,const UmiWorkbenchContextHostCommand *command)
{
    if(!host||!command||command->structure_size!=sizeof(*command))return UMI_STATUS_INVALID_ARGUMENT;
    switch(command->kind){
    case UMI_WORKBENCH_CONTEXT_HOST_COMMAND_SET_ACTIVE_GROUP:
        return umi_workbench_context_host_set_active_group(host,command->group_id);
    case UMI_WORKBENCH_CONTEXT_HOST_COMMAND_ASSIGN_PANEL:
        return umi_workbench_context_host_assign_endpoint_group(host,command->endpoint_id,command->group_id,command->mode);
    case UMI_WORKBENCH_CONTEXT_HOST_COMMAND_UNASSIGN_PANEL: {
        UmiWorkbenchContextHostEndpoint *endpoint =
            umi_workbench_context_host_endpoint_registry_find(&host->endpoints, command->endpoint_id);
        if (!endpoint) return UMI_STATUS_NOT_FOUND;
        endpoint->group_id[0] = '\0';
        endpoint->mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_NONE;
        ++endpoint->revision;
        (void)umi_workbench_context_link_service_unbind_panel(host->link_service, endpoint->panel_id);
        ++host->revision;
        return UMI_STATUS_OK;
    }
    case UMI_WORKBENCH_CONTEXT_HOST_COMMAND_BACK:return umi_workbench_context_host_back(host,command->timestamp_ms);
    case UMI_WORKBENCH_CONTEXT_HOST_COMMAND_FORWARD:return umi_workbench_context_host_forward(host,command->timestamp_ms);
    case UMI_WORKBENCH_CONTEXT_HOST_COMMAND_PIN:return umi_workbench_context_host_pin(host,command->argument,command->timestamp_ms);
    case UMI_WORKBENCH_CONTEXT_HOST_COMMAND_CLEAR_PANEL_INBOX:return umi_workbench_context_host_clear_inbox(host,command->endpoint_id);
    case UMI_WORKBENCH_CONTEXT_HOST_COMMAND_SUSPEND:umi_workbench_context_host_set_suspended(host,true);return UMI_STATUS_OK;
    case UMI_WORKBENCH_CONTEXT_HOST_COMMAND_RESUME:umi_workbench_context_host_set_suspended(host,false);return UMI_STATUS_OK;
    default:return UMI_STATUS_NOT_IMPLEMENTED;
    }
}
