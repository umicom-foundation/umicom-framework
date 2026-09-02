/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/linux_glibc.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Linux GLIBC developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_LINUX_GLIBC_H
#define UMICOM_DEVELOPER_CAPABILITIES_LINUX_GLIBC_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_LINUX_GLIBC "umicom.developer.portability.linux_glibc"

/**
 * Provide the developer capability linux glibc operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_linux_glibc(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_LINUX_GLIBC_H */
