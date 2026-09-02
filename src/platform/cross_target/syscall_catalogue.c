/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/syscall_catalogue.c
 *
 * PURPOSE:
 *   Maintain unique syscall number/name registrations for ABI stability checks.
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

#include "umicom/platform/cross_target/syscall_catalogue.h"

#include <string.h>
/*
 * Initialise ct syscall catalogue from caller-provided values so later operations receive
 * a known state.
 */
void umi_ct_syscall_catalogue_init(UmiCtSyscallCatalogue*c){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c!=NULL)memset(c,0,sizeof(*c));}
/* Add ct syscall catalogue only after its inputs and available capacity have been checked. */
UmiStatus umi_ct_syscall_catalogue_add(UmiCtSyscallCatalogue*c,const UmiCtSyscallDescriptor*d){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||umi_ct_syscall_descriptor_validate(d)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<c->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c->items[i].number==d->number||strcmp(c->items[i].name,d->name)==0)return UMI_STATUS_ALREADY_EXISTS;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c->count>=UMI_CT_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;c->items[c->count++]=*d;return UMI_STATUS_OK;}
/*
 * Provide the ct syscall catalogue find number operation used by this module and its
 * client applications.
 */
const UmiCtSyscallDescriptor *umi_ct_syscall_catalogue_find_number(const UmiCtSyscallCatalogue*c,uint32_t n){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL)return NULL;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<c->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c->items[i].number==n)return &c->items[i];return NULL;}
