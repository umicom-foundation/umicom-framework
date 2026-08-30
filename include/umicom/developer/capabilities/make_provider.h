/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/make_provider.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Make Provider developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_MAKE_PROVIDER_H
#define UMICOM_DEVELOPER_CAPABILITIES_MAKE_PROVIDER_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_MAKE_PROVIDER "umicom.developer.integration.make_provider"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_make_provider(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_MAKE_PROVIDER_H */
