/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/multi_configuration.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Multi Configuration developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_MULTI_CONFIGURATION_H
#define UMICOM_DEVELOPER_CAPABILITIES_MULTI_CONFIGURATION_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_MULTI_CONFIGURATION "umicom.developer.build.multi_configuration"

/**
 * Provide the developer capability multi configuration operation used by this module and
 * its client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_multi_configuration(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_MULTI_CONFIGURATION_H */
