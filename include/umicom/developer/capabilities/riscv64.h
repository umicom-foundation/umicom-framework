/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/riscv64.h
 *
 * PURPOSE:
 *   Declare the Framework-owned RISC-V 64 developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_RISCV64_H
#define UMICOM_DEVELOPER_CAPABILITIES_RISCV64_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_RISCV64 "umicom.developer.portability.riscv64"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_riscv64(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_RISCV64_H */
