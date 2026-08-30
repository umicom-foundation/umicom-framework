/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/cross_compile_profile.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Cross Compile Profile developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_CROSS_COMPILE_PROFILE_H
#define UMICOM_DEVELOPER_CAPABILITIES_CROSS_COMPILE_PROFILE_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_CROSS_COMPILE_PROFILE "umicom.developer.configuration.cross_compile_profile"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_cross_compile_profile(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_CROSS_COMPILE_PROFILE_H */
