/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/master_authority.h
 *
 * PURPOSE:
 *   Install the federated desktop under the established Master Controller.
 *   This authority coordinates application windows through Framework and does
 *   not replace the Master/Slave Controller architecture or terminology.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Provide the master controller install desktop authority operation used by this module
 * and its client applications.
 */
UmiStatus umi_master_controller_install_desktop_authority(
    UmiMasterController *controller);
/**
 * Provide the master controller desktop runtime operation used by this module and its
 * client applications.
 */
UmiDesktopRuntime *umi_master_controller_desktop_runtime(
    UmiMasterController *controller);
/**
 * Provide the master controller desktop shell operation used by this module and its client
 * applications.
 */
UmiDesktopShellModel *umi_master_controller_desktop_shell(
    UmiMasterController *controller);
/**
 * Provide the master controller desktop content operation used by this module and its
 * client applications.
 */
UmiDesktopContentRuntime *umi_master_controller_desktop_content(
    UmiMasterController *controller);
/**
 * Provide the master controller desktop component host operation used by this module and
 * its client applications.
 */
UmiUiComponentHostService *umi_master_controller_desktop_component_host(
    UmiMasterController *controller);
/**
 * Provide the master controller desktop component drag drop operation used by this module
 * and its client applications.
 */
UmiDesktopComponentDragDrop *umi_master_controller_desktop_component_drag_drop(
    UmiMasterController *controller);
/**
 * Provide the master controller desktop context synchronizer operation used by this module
 * and its client applications.
 */
UmiDesktopContextSynchronizer *umi_master_controller_desktop_context_synchronizer(
    UmiMasterController *controller);
/**
 * Provide the master controller desktop layout history operation used by this module and
 * its client applications.
 */
UmiDesktopLayoutHistory *umi_master_controller_desktop_layout_history(
    UmiMasterController *controller);
/**
 * Provide the master controller desktop session recovery operation used by this module and
 * its client applications.
 */
UmiDesktopSessionRecovery *umi_master_controller_desktop_session_recovery(
    UmiMasterController *controller);
/**
 * Provide the master controller desktop monitor interaction operation used by this module
 * and its client applications.
 */
UmiDesktopMonitorInteraction *umi_master_controller_desktop_monitor_interaction(
    UmiMasterController *controller);

#endif
