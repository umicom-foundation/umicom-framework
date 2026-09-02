/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/package_manifest.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Package Manifest developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_PACKAGE_MANIFEST_H
#define UMICOM_DEVELOPER_CAPABILITIES_PACKAGE_MANIFEST_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_PACKAGE_MANIFEST "umicom.developer.delivery.package_manifest"

/**
 * Provide the developer capability package manifest operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_package_manifest(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_PACKAGE_MANIFEST_H */
