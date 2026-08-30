/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/abi_compatibility.c
 *
 * PURPOSE:
 *   Classify ABI compatibility and identify the first material incompatibility.
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

#include "umicom/platform/cross_target/abi_compatibility.h"

#include <stdio.h>
#include <string.h>
static void set_reason(char*r,size_t c,const char*t){if(r!=NULL&&c>0U)(void)snprintf(r,c,"%s",t);}
UmiCtAbiCompatibility umi_ct_abi_compare(const UmiCtAbiDescriptor*a,const UmiCtAbiDescriptor*b,char*r,size_t c){if(a==NULL||b==NULL){set_reason(r,c,"missing descriptor");return UMI_CT_ABI_INCOMPATIBLE;}if(a->pointer_bits!=b->pointer_bits||a->data_model!=b->data_model){set_reason(r,c,"data model");return UMI_CT_ABI_INCOMPATIBLE;}if(a->calling_convention!=b->calling_convention){set_reason(r,c,"calling convention");return UMI_CT_ABI_BRIDGE_REQUIRED;}if(a->hard_float!=b->hard_float){set_reason(r,c,"floating point ABI");return UMI_CT_ABI_BRIDGE_REQUIRED;}if(a->stack_alignment!=b->stack_alignment){set_reason(r,c,"stack alignment");return UMI_CT_ABI_INCOMPATIBLE;}set_reason(r,c,"compatible");return UMI_CT_ABI_COMPATIBLE;}
