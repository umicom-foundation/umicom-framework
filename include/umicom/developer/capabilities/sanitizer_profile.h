/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/sanitizer_profile.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Sanitizer Profile developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_SANITIZER_PROFILE_H
#define UMICOM_DEVELOPER_CAPABILITIES_SANITIZER_PROFILE_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_SANITIZER_PROFILE "umicom.developer.build.sanitizer_profile"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_sanitizer_profile(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_SANITIZER_PROFILE_H */
