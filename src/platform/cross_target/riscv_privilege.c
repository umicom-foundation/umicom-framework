/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/riscv_privilege.c
 *
 * PURPOSE:
 *   Model supported RISC-V privilege modes and validate Umicom OS kernel prerequisites.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/cross_target/riscv_privilege.h"

bool umi_ct_riscv_privilege_has(UmiCtPrivilegeMask m,UmiCtPrivilegeMask x){return (m&x)==x;}
UmiStatus umi_ct_riscv_privilege_validate_os(UmiCtPrivilegeMask m,bool h){if(!umi_ct_riscv_privilege_has(m,UMI_CT_PRIV_MACHINE|UMI_CT_PRIV_SUPERVISOR|UMI_CT_PRIV_USER))return UMI_STATUS_INVALID_STATE;if(h&&!umi_ct_riscv_privilege_has(m,UMI_CT_PRIV_HYPERVISOR))return UMI_STATUS_UNAVAILABLE;return UMI_STATUS_OK;}
