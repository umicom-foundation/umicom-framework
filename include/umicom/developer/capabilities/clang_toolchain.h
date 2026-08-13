/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/clang_toolchain.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Clang Toolchain developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_CLANG_TOOLCHAIN_H
#define UMICOM_DEVELOPER_CAPABILITIES_CLANG_TOOLCHAIN_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_CLANG_TOOLCHAIN "umicom.developer.integration.clang_toolchain"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_clang_toolchain(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_CLANG_TOOLCHAIN_H */
