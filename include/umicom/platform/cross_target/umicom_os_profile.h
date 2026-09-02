/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/umicom_os_profile.h
 *
 * PURPOSE:
 *   Construct the default Umicom OS RISC-V runtime profile and validate its kernel prerequisites.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_CROSS_TARGET_UMICOM_OS_PROFILE_H
#define UMICOM_PLATFORM_CROSS_TARGET_UMICOM_OS_PROFILE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/platform/cross_target/riscv_profile.h"
#include "umicom/platform/cross_target/riscv_privilege.h"
#include "umicom/platform/cross_target/riscv_memory_model.h"
/**
 * Represent the ct umicom os profile data shared with callers of this public contract.
 */
typedef struct UmiCtUmicomOsProfile { UmiCtTarget target; UmiCtRiscvProfile cpu_profile; UmiCtPrivilegeMask privileges; UmiCtRiscvMemoryModel memory_model; uint32_t kernel_stack_size; bool smp; bool userspace; } UmiCtUmicomOsProfile;
/**
 * Provide the ct umicom os profile default operation used by this module and its client
 * applications.
 */
UmiStatus umi_ct_umicom_os_profile_default(UmiCtUmicomOsProfile *out_profile);
/**
 * Check that ct umicom os profile satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_ct_umicom_os_profile_validate(const UmiCtUmicomOsProfile *profile);

#ifdef __cplusplus
}
#endif

#endif
