/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/multi_configuration.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Multi Configuration developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_MULTI_CONFIGURATION_H
#define UMICOM_DEVELOPER_CAPABILITIES_MULTI_CONFIGURATION_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_MULTI_CONFIGURATION "umicom.developer.build.multi_configuration"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_multi_configuration(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_MULTI_CONFIGURATION_H */
