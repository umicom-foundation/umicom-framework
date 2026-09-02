/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/riscv_abi.c
 *
 * PURPOSE:
 *   Parse and validate standard RISC-V ABI names against XLEN and floating-point capabilities.
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

#include "umicom/platform/cross_target/riscv_abi.h"

#include <string.h>
/*
 * Provide the ct riscv abi descriptor operation used by this module and its client
 * applications.
 */
UmiStatus umi_ct_riscv_abi_descriptor(const char*a,UmiCtAbiDescriptor*d){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(a==NULL||d==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(d,0,sizeof(*d));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_ct_copy(d->abi_id,sizeof(d->abi_id),a)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;d->calling_convention=UMI_CT_CALL_RISCV;d->stack_alignment=16U;d->long_double_bits=128U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(a,"ilp32")==0||strcmp(a,"ilp32f")==0||strcmp(a,"ilp32d")==0){d->data_model=UMI_CT_DATA_ILP32;d->pointer_bits=32U;}else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(a,"lp64")==0||strcmp(a,"lp64f")==0||strcmp(a,"lp64d")==0){d->data_model=UMI_CT_DATA_LP64;d->pointer_bits=64U;}/* Use this fallback path when the earlier condition does not apply. */ else return UMI_STATUS_NOT_FOUND;d->hard_float=strchr(a,'f')!=NULL||strchr(a,'d')!=NULL;return UMI_STATUS_OK;}
/*
 * Provide the ct riscv abi matches xlen operation used by this module and its client
 * applications.
 */
bool umi_ct_riscv_abi_matches_xlen(const UmiCtAbiDescriptor*d,uint32_t x){return d!=NULL&&d->pointer_bits==x;}
