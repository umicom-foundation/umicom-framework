/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/process_allowlist.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Process Allowlist developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_PROCESS_ALLOWLIST_H
#define UMICOM_DEVELOPER_CAPABILITIES_PROCESS_ALLOWLIST_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_PROCESS_ALLOWLIST "umicom.developer.security.process_allowlist"

/**
 * Provide the developer capability process allowlist operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_process_allowlist(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_PROCESS_ALLOWLIST_H */
