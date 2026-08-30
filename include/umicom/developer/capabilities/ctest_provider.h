/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/ctest_provider.h
 *
 * PURPOSE:
 *   Declare the Framework-owned CTest Provider developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_CTEST_PROVIDER_H
#define UMICOM_DEVELOPER_CAPABILITIES_CTEST_PROVIDER_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_CTEST_PROVIDER "umicom.developer.integration.ctest_provider"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_ctest_provider(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_CTEST_PROVIDER_H */
