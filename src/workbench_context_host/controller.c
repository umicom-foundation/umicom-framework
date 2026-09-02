/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/controller.c
 *
 * PURPOSE:
 *   Implement the Workbench Context Host Slave Controller lifecycle.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/controller.h"
#include <string.h>
/*
 * Initialise workbench context host slave controller from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_host_slave_controller_init(
    UmiWorkbenchContextHostSlaveController *controller,UmiWorkbenchContextHost *host)
{/* Preserve the original failure result so the caller can respond to the correct cause. */ if(controller){memset(controller,0,sizeof(*controller));controller->host=host;controller->state=UMI_WORKBENCH_CONTEXT_HOST_CONTROLLER_INITIALISED;controller->last_status=host?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;controller->revision=1U;}}
/*
 * Provide the workbench context host slave controller start operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_host_slave_controller_start(UmiWorkbenchContextHostSlaveController *controller)
{/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!controller||!controller->host)return UMI_STATUS_INVALID_ARGUMENT;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(controller->state!=UMI_WORKBENCH_CONTEXT_HOST_CONTROLLER_INITIALISED&&controller->state!=UMI_WORKBENCH_CONTEXT_HOST_CONTROLLER_QUIESCED)return UMI_STATUS_INVALID_STATE;umi_workbench_context_host_set_suspended(controller->host,false);controller->state=UMI_WORKBENCH_CONTEXT_HOST_CONTROLLER_RUNNING;++controller->revision;return UMI_STATUS_OK;}
/*
 * Provide the workbench context host slave controller quiesce operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_host_slave_controller_quiesce(UmiWorkbenchContextHostSlaveController *controller)
{/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!controller||!controller->host)return UMI_STATUS_INVALID_ARGUMENT;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(controller->state!=UMI_WORKBENCH_CONTEXT_HOST_CONTROLLER_RUNNING)return UMI_STATUS_INVALID_STATE;umi_workbench_context_host_set_suspended(controller->host,true);controller->state=UMI_WORKBENCH_CONTEXT_HOST_CONTROLLER_QUIESCED;++controller->revision;return UMI_STATUS_OK;}
/*
 * Provide the workbench context host slave controller resume operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_host_slave_controller_resume(UmiWorkbenchContextHostSlaveController *controller)
{return umi_workbench_context_host_slave_controller_start(controller);}
/*
 * Provide the workbench context host slave controller stop operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_host_slave_controller_stop(UmiWorkbenchContextHostSlaveController *controller)
{/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!controller||!controller->host)return UMI_STATUS_INVALID_ARGUMENT;umi_workbench_context_host_set_suspended(controller->host,true);controller->state=UMI_WORKBENCH_CONTEXT_HOST_CONTROLLER_STOPPED;++controller->revision;return UMI_STATUS_OK;}
