/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/aarch64.h
 *
 * PURPOSE:
 *   Declare the Framework-owned AArch64 developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_AARCH64_H
#define UMICOM_DEVELOPER_CAPABILITIES_AARCH64_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_AARCH64 "umicom.developer.portability.aarch64"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_aarch64(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_AARCH64_H */
