/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/master_authority.h
 *
 * PURPOSE:
 *   Install the federated desktop under the established Master Controller.
 *   This authority coordinates application windows through Framework and does
 *   not replace the Master/Slave Controller architecture or terminology.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_MASTER_AUTHORITY_H
#define UMICOM_DESKTOP_MASTER_AUTHORITY_H

#include "umicom/desktop/content_runtime.h"
#include "umicom/desktop/component_drag_drop.h"
#include "umicom/desktop/context_synchronizer.h"
#include "umicom/desktop/layout_history.h"
#include "umicom/desktop/monitor_interaction.h"
#include "umicom/desktop/runtime.h"
#include "umicom/desktop/session_recovery.h"
#include "umicom/desktop/shell_model.h"
#include "umicom/runtime/master_controller.h"

#define UMI_DESKTOP_MASTER_AUTHORITY_ID "org.umicom.framework.desktop"

UmiStatus umi_master_controller_install_desktop_authority(
    UmiMasterController *controller);
UmiDesktopRuntime *umi_master_controller_desktop_runtime(
    UmiMasterController *controller);
UmiDesktopShellModel *umi_master_controller_desktop_shell(
    UmiMasterController *controller);
UmiDesktopContentRuntime *umi_master_controller_desktop_content(
    UmiMasterController *controller);
UmiUiComponentHostService *umi_master_controller_desktop_component_host(
    UmiMasterController *controller);
UmiDesktopComponentDragDrop *umi_master_controller_desktop_component_drag_drop(
    UmiMasterController *controller);
UmiDesktopContextSynchronizer *umi_master_controller_desktop_context_synchronizer(
    UmiMasterController *controller);
UmiDesktopLayoutHistory *umi_master_controller_desktop_layout_history(
    UmiMasterController *controller);
UmiDesktopSessionRecovery *umi_master_controller_desktop_session_recovery(
    UmiMasterController *controller);
UmiDesktopMonitorInteraction *umi_master_controller_desktop_monitor_interaction(
    UmiMasterController *controller);

#endif
