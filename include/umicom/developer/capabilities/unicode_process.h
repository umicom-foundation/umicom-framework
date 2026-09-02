/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/unicode_process.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Unicode Process developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_UNICODE_PROCESS_H
#define UMICOM_DEVELOPER_CAPABILITIES_UNICODE_PROCESS_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_UNICODE_PROCESS "umicom.developer.portability.unicode_process"

/**
 * Provide the developer capability unicode process operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_unicode_process(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_UNICODE_PROCESS_H */
