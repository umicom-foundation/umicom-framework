/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/umicom_os_profile.c
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

#include "umicom/platform/cross_target/umicom_os_profile.h"

#include "umicom/platform/cross_target/target_triple.h"
#include <string.h>
UmiStatus umi_ct_umicom_os_profile_default(UmiCtUmicomOsProfile*p){if(p==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(p,0,sizeof(*p));if(umi_ct_target_triple_parse("riscv64-umicom-umicom-native",&p->target)!=UMI_STATUS_OK)return UMI_STATUS_INTERNAL_ERROR;if(umi_ct_riscv_profile_builtin("umos-rv64",&p->cpu_profile)!=UMI_STATUS_OK)return UMI_STATUS_INTERNAL_ERROR;p->privileges=UMI_CT_PRIV_USER|UMI_CT_PRIV_SUPERVISOR|UMI_CT_PRIV_MACHINE;p->memory_model=(UmiCtRiscvMemoryModel){UMI_CT_RISCV_MMU_SV39,56U,39U,4096U,true};p->kernel_stack_size=32768U;p->smp=true;p->userspace=true;return UMI_STATUS_OK;}
UmiStatus umi_ct_umicom_os_profile_validate(const UmiCtUmicomOsProfile*p){if(p==NULL||p->target.operating_system!=UMI_CT_OS_UMICOM||p->target.architecture!=UMI_CT_ARCH_RISCV64||p->kernel_stack_size<16384U)return UMI_STATUS_INVALID_ARGUMENT;if(umi_ct_riscv_privilege_validate_os(p->privileges,false)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_STATE;if(umi_ct_riscv_memory_model_validate(&p->memory_model,64U)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_STATE;if(p->userspace&&!umi_ct_riscv_privilege_has(p->privileges,UMI_CT_PRIV_USER))return UMI_STATUS_INVALID_STATE;return UMI_STATUS_OK;}
