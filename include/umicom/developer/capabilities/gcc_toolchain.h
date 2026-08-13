/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/gcc_toolchain.h
 *
 * PURPOSE:
 *   Declare the Framework-owned GCC Toolchain developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_GCC_TOOLCHAIN_H
#define UMICOM_DEVELOPER_CAPABILITIES_GCC_TOOLCHAIN_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_GCC_TOOLCHAIN "umicom.developer.integration.gcc_toolchain"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_gcc_toolchain(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_GCC_TOOLCHAIN_H */
