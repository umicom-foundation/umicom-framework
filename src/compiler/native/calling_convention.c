/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/calling_convention.c
 *
 * PURPOSE:
 *   Describe register argument, return and stack alignment rules for supported native ABIs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/calling_convention.h"
#include <string.h>
UmiStatus umi_nc_calling_convention_default(const UmiNativeTargetProfile *t,UmiNativeCallingConvention *c){if(t==NULL||c==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(c,0,sizeof(*c));c->stack_alignment=16U;c->caller_cleans_stack=true;if(t->architecture==UMI_NC_ARCH_RISCV64){if(umi_nc_copy_text(c->name,sizeof(c->name),"riscv-lp64")!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;for(size_t i=0U;i<8U;i++)c->integer_argument_registers[i]=(uint32_t)(10U+i);c->integer_argument_count=8U;c->return_register=10U;return UMI_STATUS_OK;}if(t->architecture==UMI_NC_ARCH_X86_64){bool win=t->object_format==UMI_NC_OBJECT_COFF;if(umi_nc_copy_text(c->name,sizeof(c->name),win?"windows-x64":"sysv-x86-64")!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;static const uint32_t sysv[]={7U,6U,2U,1U,8U,9U};static const uint32_t ms[]={1U,2U,8U,9U};const uint32_t *src=win?ms:sysv;size_t n=win?4U:6U;for(size_t i=0U;i<n;i++)c->integer_argument_registers[i]=src[i];c->integer_argument_count=n;c->return_register=0U;return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
