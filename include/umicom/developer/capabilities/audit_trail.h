/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/audit_trail.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Audit Trail developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_AUDIT_TRAIL_H
#define UMICOM_DEVELOPER_CAPABILITIES_AUDIT_TRAIL_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_AUDIT_TRAIL "umicom.developer.security.audit_trail"

/**
 * Provide the developer capability audit trail operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_audit_trail(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_AUDIT_TRAIL_H */
