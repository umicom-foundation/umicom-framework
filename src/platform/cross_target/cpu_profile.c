/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/cpu_profile.c
 *
 * PURPOSE:
 *   Describe deployable CPU profiles and evaluate runtime feature/XLEN compatibility.
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

#include "umicom/platform/cross_target/cpu_profile.h"

UmiStatus umi_ct_cpu_profile_validate(const UmiCtCpuProfile*p){if(p==NULL||!umi_ct_id_valid(p->profile_id))return UMI_STATUS_INVALID_ARGUMENT;if(p->xlen!=32U&&p->xlen!=64U)return UMI_STATUS_INVALID_ARGUMENT;if(p->minimum_cores==0U)return UMI_STATUS_INVALID_ARGUMENT;if((p->architecture==UMI_CT_ARCH_RISCV32&&p->xlen!=32U)||(p->architecture==UMI_CT_ARCH_RISCV64&&p->xlen!=64U))return UMI_STATUS_INVALID_STATE;return UMI_STATUS_OK;}
bool umi_ct_cpu_profile_matches(const UmiCtCpuProfile*p,UmiCtArchitecture a,uint32_t x,uint32_t c,const UmiCtCpuFeatureSet*f){return p!=NULL&&f!=NULL&&p->architecture==a&&p->xlen==x&&c>=p->minimum_cores&&umi_ct_cpu_feature_set_missing(f,&p->required)==0U;}
