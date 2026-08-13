/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/ninja_provider.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Ninja Provider developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_NINJA_PROVIDER_H
#define UMICOM_DEVELOPER_CAPABILITIES_NINJA_PROVIDER_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_NINJA_PROVIDER "umicom.developer.integration.ninja_provider"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_ninja_provider(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_NINJA_PROVIDER_H */
