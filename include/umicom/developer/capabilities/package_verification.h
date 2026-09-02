/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/package_verification.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Package Verification developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_PACKAGE_VERIFICATION_H
#define UMICOM_DEVELOPER_CAPABILITIES_PACKAGE_VERIFICATION_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_PACKAGE_VERIFICATION "umicom.developer.delivery.package_verification"

/**
 * Provide the developer capability package verification operation used by this module and
 * its client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_package_verification(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_PACKAGE_VERIFICATION_H */
