/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/msvc_toolchain.h
 *
 * PURPOSE:
 *   Declare the Framework-owned MSVC Toolchain developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_MSVC_TOOLCHAIN_H
#define UMICOM_DEVELOPER_CAPABILITIES_MSVC_TOOLCHAIN_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_MSVC_TOOLCHAIN "umicom.developer.integration.msvc_toolchain"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_msvc_toolchain(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_MSVC_TOOLCHAIN_H */
