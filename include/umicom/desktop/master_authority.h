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

#include "umicom/desktop/runtime.h"
#include "umicom/desktop/shell_model.h"
#include "umicom/runtime/master_controller.h"

#define UMI_DESKTOP_MASTER_AUTHORITY_ID "org.umicom.framework.desktop"

UmiStatus umi_master_controller_install_desktop_authority(
    UmiMasterController *controller);
UmiDesktopRuntime *umi_master_controller_desktop_runtime(
    UmiMasterController *controller);
UmiDesktopShellModel *umi_master_controller_desktop_shell(
    UmiMasterController *controller);

#endif
