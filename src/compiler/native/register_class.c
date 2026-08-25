/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/register_class.c
 *
 * PURPOSE:
 *   Describe target register classes and volatile/callee-saved availability for future allocation passes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/register_class.h"
#include <string.h>
UmiStatus umi_nc_register_class_default(UmiNativeMachineArch a,UmiNativeRegisterClassKind k,UmiNativeRegisterClass *o){if(o==NULL||a==UMI_NC_ARCH_UNKNOWN)return UMI_STATUS_INVALID_ARGUMENT;memset(o,0,sizeof(*o));o->kind=k;o->width_bits=k==UMI_NC_REG_VECTOR?128U:64U;if(a==UMI_NC_ARCH_RISCV64){o->count=k==UMI_NC_REG_VECTOR?32U:32U;o->caller_saved_mask=0x0FFF0FE0ULL;o->callee_saved_mask=0x000FF00FULL;}else{o->count=k==UMI_NC_REG_VECTOR?16U:16U;o->caller_saved_mask=0x00000FC7ULL;o->callee_saved_mask=0x0000F038ULL;}return UMI_STATUS_OK;}
size_t umi_nc_register_class_usable(const UmiNativeRegisterClass *r){if(r==NULL)return 0U;uint64_t mask=r->caller_saved_mask|r->callee_saved_mask;size_t n=0U;for(size_t i=0U;i<r->count&&i<64U;i++)if((mask&(1ULL<<i))!=0U)n++;return n;}
