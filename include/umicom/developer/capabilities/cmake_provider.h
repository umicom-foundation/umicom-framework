/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/cmake_provider.h
 *
 * PURPOSE:
 *   Declare the Framework-owned CMake Provider developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_CMAKE_PROVIDER_H
#define UMICOM_DEVELOPER_CAPABILITIES_CMAKE_PROVIDER_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_CMAKE_PROVIDER "umicom.developer.integration.cmake_provider"

/**
 * Provide the developer capability cmake provider operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_cmake_provider(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_CMAKE_PROVIDER_H */
