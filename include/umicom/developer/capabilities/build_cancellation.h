/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/build_cancellation.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Build Cancellation developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_BUILD_CANCELLATION_H
#define UMICOM_DEVELOPER_CAPABILITIES_BUILD_CANCELLATION_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_BUILD_CANCELLATION "umicom.developer.build.build_cancellation"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_build_cancellation(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_BUILD_CANCELLATION_H */
