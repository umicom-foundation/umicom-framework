/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/warning_policy.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Warning Policy developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_WARNING_POLICY_H
#define UMICOM_DEVELOPER_CAPABILITIES_WARNING_POLICY_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_WARNING_POLICY "umicom.developer.build.warning_policy"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_warning_policy(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_WARNING_POLICY_H */
