/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/macos.h
 *
 * PURPOSE:
 *   Declare the Framework-owned macOS developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_MACOS_H
#define UMICOM_DEVELOPER_CAPABILITIES_MACOS_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_MACOS "umicom.developer.portability.macos"

/**
 * Provide the developer capability macos operation used by this module and its client
 * applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_macos(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_MACOS_H */
