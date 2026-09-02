/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/riscv_vector_profile.c
 *
 * PURPOSE:
 *   Model RISC-V Vector minimum VLEN/ELEN requirements for accelerated Framework kernels.
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

#include "umicom/platform/cross_target/riscv_vector_profile.h"

/*
 * Provide the ct riscv vector profile satisfied operation used by this module and its
 * client applications.
 */
bool umi_ct_riscv_vector_profile_satisfied(const UmiCtRiscvVectorProfile*p,uint32_t v,uint32_t e,bool present){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL)return false;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!p->required&&!present)return true;return present&&v>=p->minimum_vlen&&e>=p->minimum_elen;}
