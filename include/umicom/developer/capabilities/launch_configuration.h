/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/launch_configuration.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Launch Configuration developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_LAUNCH_CONFIGURATION_H
#define UMICOM_DEVELOPER_CAPABILITIES_LAUNCH_CONFIGURATION_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_LAUNCH_CONFIGURATION "umicom.developer.execution.launch_configuration"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_launch_configuration(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_LAUNCH_CONFIGURATION_H */
