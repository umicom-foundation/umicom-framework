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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/cross_target/riscv_abi.h"

#include <string.h>
UmiStatus umi_ct_riscv_abi_descriptor(const char*a,UmiCtAbiDescriptor*d){if(a==NULL||d==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(d,0,sizeof(*d));if(umi_ct_copy(d->abi_id,sizeof(d->abi_id),a)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;d->calling_convention=UMI_CT_CALL_RISCV;d->stack_alignment=16U;d->long_double_bits=128U;if(strcmp(a,"ilp32")==0||strcmp(a,"ilp32f")==0||strcmp(a,"ilp32d")==0){d->data_model=UMI_CT_DATA_ILP32;d->pointer_bits=32U;}else if(strcmp(a,"lp64")==0||strcmp(a,"lp64f")==0||strcmp(a,"lp64d")==0){d->data_model=UMI_CT_DATA_LP64;d->pointer_bits=64U;}else return UMI_STATUS_NOT_FOUND;d->hard_float=strchr(a,'f')!=NULL||strchr(a,'d')!=NULL;return UMI_STATUS_OK;}
bool umi_ct_riscv_abi_matches_xlen(const UmiCtAbiDescriptor*d,uint32_t x){return d!=NULL&&d->pointer_bits==x;}
