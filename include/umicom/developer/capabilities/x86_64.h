/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/x86_64.h
 *
 * PURPOSE:
 *   Declare the Framework-owned x86-64 developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_X86_64_H
#define UMICOM_DEVELOPER_CAPABILITIES_X86_64_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_X86_64 "umicom.developer.portability.x86_64"

/**
 * Provide the developer capability x86 64 operation used by this module and its client
 * applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_x86_64(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_X86_64_H */
