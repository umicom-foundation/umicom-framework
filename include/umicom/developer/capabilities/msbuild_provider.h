/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/msbuild_provider.h
 *
 * PURPOSE:
 *   Declare the Framework-owned MSBuild Provider developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_MSBUILD_PROVIDER_H
#define UMICOM_DEVELOPER_CAPABILITIES_MSBUILD_PROVIDER_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_MSBUILD_PROVIDER "umicom.developer.integration.msbuild_provider"

/**
 * Provide the developer capability msbuild provider operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_msbuild_provider(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_MSBUILD_PROVIDER_H */
