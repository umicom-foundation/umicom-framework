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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/cross_target/syscall_catalogue.h"

#include <string.h>
void umi_ct_syscall_catalogue_init(UmiCtSyscallCatalogue*c){if(c!=NULL)memset(c,0,sizeof(*c));}
UmiStatus umi_ct_syscall_catalogue_add(UmiCtSyscallCatalogue*c,const UmiCtSyscallDescriptor*d){size_t i;if(c==NULL||umi_ct_syscall_descriptor_validate(d)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<c->count;++i)if(c->items[i].number==d->number||strcmp(c->items[i].name,d->name)==0)return UMI_STATUS_ALREADY_EXISTS;if(c->count>=UMI_CT_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;c->items[c->count++]=*d;return UMI_STATUS_OK;}
const UmiCtSyscallDescriptor *umi_ct_syscall_catalogue_find_number(const UmiCtSyscallCatalogue*c,uint32_t n){size_t i;if(c==NULL)return NULL;for(i=0U;i<c->count;++i)if(c->items[i].number==n)return &c->items[i];return NULL;}
