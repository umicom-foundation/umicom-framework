/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/riscv_privilege.h
 *
 * PURPOSE:
 *   Model supported RISC-V privilege modes and validate Umicom OS kernel prerequisites.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_RISCV_PRIVILEGE_H
#define UMICOM_PLATFORM_CROSS_TARGET_RISCV_PRIVILEGE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t UmiCtPrivilegeMask;
enum { UMI_CT_PRIV_USER=1U<<0, UMI_CT_PRIV_SUPERVISOR=1U<<1, UMI_CT_PRIV_MACHINE=1U<<2, UMI_CT_PRIV_HYPERVISOR=1U<<3 };
/**
 * Provide the ct riscv privilege has operation used by this module and its client
 * applications.
 */
bool umi_ct_riscv_privilege_has(UmiCtPrivilegeMask mask,UmiCtPrivilegeMask mode);
/**
 * Provide the ct riscv privilege validate os operation used by this module and its client
 * applications.
 */
UmiStatus umi_ct_riscv_privilege_validate_os(UmiCtPrivilegeMask mask,bool require_hypervisor);

#ifdef __cplusplus
}
#endif

#endif
