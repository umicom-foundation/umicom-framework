/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/controller.c
 *
 * PURPOSE:
 *   Implement the Workbench Context Host Slave Controller lifecycle.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/controller.h"
#include <string.h>
void umi_workbench_context_host_slave_controller_init(
    UmiWorkbenchContextHostSlaveController *controller,UmiWorkbenchContextHost *host)
{if(controller){memset(controller,0,sizeof(*controller));controller->host=host;controller->state=UMI_WORKBENCH_CONTEXT_HOST_CONTROLLER_INITIALISED;controller->last_status=host?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;controller->revision=1U;}}
UmiStatus umi_workbench_context_host_slave_controller_start(UmiWorkbenchContextHostSlaveController *controller)
{if(!controller||!controller->host)return UMI_STATUS_INVALID_ARGUMENT;if(controller->state!=UMI_WORKBENCH_CONTEXT_HOST_CONTROLLER_INITIALISED&&controller->state!=UMI_WORKBENCH_CONTEXT_HOST_CONTROLLER_QUIESCED)return UMI_STATUS_INVALID_STATE;umi_workbench_context_host_set_suspended(controller->host,false);controller->state=UMI_WORKBENCH_CONTEXT_HOST_CONTROLLER_RUNNING;++controller->revision;return UMI_STATUS_OK;}
UmiStatus umi_workbench_context_host_slave_controller_quiesce(UmiWorkbenchContextHostSlaveController *controller)
{if(!controller||!controller->host)return UMI_STATUS_INVALID_ARGUMENT;if(controller->state!=UMI_WORKBENCH_CONTEXT_HOST_CONTROLLER_RUNNING)return UMI_STATUS_INVALID_STATE;umi_workbench_context_host_set_suspended(controller->host,true);controller->state=UMI_WORKBENCH_CONTEXT_HOST_CONTROLLER_QUIESCED;++controller->revision;return UMI_STATUS_OK;}
UmiStatus umi_workbench_context_host_slave_controller_resume(UmiWorkbenchContextHostSlaveController *controller)
{return umi_workbench_context_host_slave_controller_start(controller);}
UmiStatus umi_workbench_context_host_slave_controller_stop(UmiWorkbenchContextHostSlaveController *controller)
{if(!controller||!controller->host)return UMI_STATUS_INVALID_ARGUMENT;umi_workbench_context_host_set_suspended(controller->host,true);controller->state=UMI_WORKBENCH_CONTEXT_HOST_CONTROLLER_STOPPED;++controller->revision;return UMI_STATUS_OK;}
