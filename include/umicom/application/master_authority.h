/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/master_authority.h
 *
 * PURPOSE:
 *   Install the Framework-owned application portfolio, federation, shared
 *   context and resource authorities under the established Master Controller.
 *   This extends the Master Controller and does not replace the Master/Slave
 *   Controller architecture or its lifecycle terminology.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_MASTER_AUTHORITY_H
#define UMICOM_APPLICATION_MASTER_AUTHORITY_H

#include "umicom/application/context_hub.h"
#include "umicom/application/federation.h"
#include "umicom/application/resource_broker.h"
#include "umicom/runtime/master_controller.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_APPLICATION_MASTER_AUTHORITY_ID \
    "org.umicom.framework.application"

/**
 * Provide the master controller install application authority operation used by this
 * module and its client applications.
 */
UmiStatus umi_master_controller_install_application_authority(
    UmiMasterController *controller,
    const char *application_id);
/**
 * Provide the master controller attach application operation used by this module and its
 * client applications.
 */
UmiStatus umi_master_controller_attach_application(
    UmiMasterController *controller,
    const UmiApplicationDefinition *application);
/**
 * Provide the master controller application definition operation used by this module and
 * its client applications.
 */
const UmiApplicationDefinition *umi_master_controller_application_definition(
    UmiMasterController *controller);
/**
 * Provide the master controller application federation operation used by this module and
 * its client applications.
 */
UmiFederationRouter *umi_master_controller_application_federation(
    UmiMasterController *controller);
/**
 * Provide the master controller application context operation used by this module and its
 * client applications.
 */
UmiApplicationContextHub *umi_master_controller_application_context(
    UmiMasterController *controller);
/**
 * Provide the master controller application resources operation used by this module and
 * its client applications.
 */
UmiApplicationResourceBroker *umi_master_controller_application_resources(
    UmiMasterController *controller);

#ifdef __cplusplus
}
#endif

#endif
