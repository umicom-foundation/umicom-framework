/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/windows_ucrt.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Windows UCRT developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_WINDOWS_UCRT_H
#define UMICOM_DEVELOPER_CAPABILITIES_WINDOWS_UCRT_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_WINDOWS_UCRT "umicom.developer.portability.windows_ucrt"

/**
 * Provide the developer capability windows ucrt operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_windows_ucrt(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_WINDOWS_UCRT_H */
