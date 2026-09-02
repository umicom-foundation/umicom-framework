/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/target_machine.c
 *
 * PURPOSE:
 *   Combine target and optimization profiles into a validated native code-generation machine contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/target_machine.h"
#include <string.h>
/*
 * Initialise nc target machine from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_nc_target_machine_init(UmiNativeTargetMachine *m,const UmiNativeTargetProfile *t,const UmiNativeOptimizationProfile *o){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||t==NULL||o==NULL||t->architecture==UMI_NC_ARCH_UNKNOWN)return UMI_STATUS_INVALID_ARGUMENT;memset(m,0,sizeof(*m));m->target=*t;m->optimization=*o;m->integer_registers=t->architecture==UMI_NC_ARCH_RISCV64?32U:16U;m->floating_registers=t->architecture==UMI_NC_ARCH_RISCV64?32U:16U;m->supports_division=true;m->supports_vectors=t->architecture==UMI_NC_ARCH_RISCV64?((t->feature_mask&(1ULL<<4U))!=0U):true;return UMI_STATUS_OK;}
/*
 * Provide the nc target machine supports opcode operation used by this module and its
 * client applications.
 */
bool umi_nc_target_machine_supports_opcode(const UmiNativeTargetMachine *m,UmiNativeIrOpcode op){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL)return false;/* Protect caller-owned memory by checking that required state is available before it is used. */ if((op==UMI_NC_IR_DIV||op==UMI_NC_IR_MOD)&&!m->supports_division)return false;return op<=UMI_NC_IR_COPY;}
