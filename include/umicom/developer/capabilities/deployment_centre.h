/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/deployment_centre.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Deployment Centre developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_DEPLOYMENT_CENTRE_H
#define UMICOM_DEVELOPER_CAPABILITIES_DEPLOYMENT_CENTRE_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_DEPLOYMENT_CENTRE "umicom.developer.experience.deployment_centre"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_deployment_centre(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_DEPLOYMENT_CENTRE_H */
