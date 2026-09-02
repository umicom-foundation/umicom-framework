/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/install_plan.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Install Plan developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_INSTALL_PLAN_H
#define UMICOM_DEVELOPER_CAPABILITIES_INSTALL_PLAN_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_INSTALL_PLAN "umicom.developer.delivery.install_plan"

/**
 * Provide the developer capability install plan operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_install_plan(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_INSTALL_PLAN_H */
