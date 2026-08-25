/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/riscv_profile.c
 *
 * PURPOSE:
 *   Define named RISC-V deployment profiles for Linux, bare-metal and Umicom OS targets.
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

#include "umicom/platform/cross_target/riscv_profile.h"

#include <string.h>
UmiStatus umi_ct_riscv_profile_builtin(const char*id,UmiCtRiscvProfile*p){const char*isa;if(id==NULL||p==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(p,0,sizeof(*p));if(umi_ct_copy(p->profile_id,sizeof(p->profile_id),id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;if(strcmp(id,"rv64gc")==0){isa="rv64gc";p->minimum_cores=1U;}else if(strcmp(id,"umos-rv64")==0){isa="rv64gc_zba_zbb";p->minimum_cores=2U;p->requires_mmu=true;p->supervisor_mode=true;}else if(strcmp(id,"bare-rv64imac")==0){isa="rv64imac";p->minimum_cores=1U;}else return UMI_STATUS_NOT_FOUND;return umi_ct_riscv_isa_parse(isa,&p->minimum_isa);}
bool umi_ct_riscv_profile_satisfied(const UmiCtRiscvProfile*p,const UmiCtRiscvIsa*a,uint32_t cores,bool mmu){if(p==NULL||a==NULL||a->xlen!=p->minimum_isa.xlen||cores<p->minimum_cores)return false;if(p->requires_mmu&&!mmu)return false;if((a->features.bits&p->minimum_isa.features.bits)!=p->minimum_isa.features.bits)return false;return a->integer_base&&(!p->minimum_isa.multiply||a->multiply);}
