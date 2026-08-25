/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/relocation.c
 *
 * PURPOSE:
 *   Represent object relocation requests independently from ELF, COFF and Mach-O writers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/relocation.h"
#include <string.h>
UmiStatus umi_nc_relocation_init(UmiNativeRelocation *r,UmiNativeRelocationKind kind,const char *section,uint64_t off,const char *symbol,int64_t add){if(r==NULL||section==NULL||symbol==NULL||section[0]=='\0'||symbol[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;memset(r,0,sizeof(*r));r->kind=kind;r->offset=off;r->addend=add;if(umi_nc_copy_text(r->section,sizeof(r->section),section)!=UMI_STATUS_OK||umi_nc_copy_text(r->symbol,sizeof(r->symbol),symbol)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;return UMI_STATUS_OK;}
UmiStatus umi_nc_relocation_validate(const UmiNativeRelocation *r,UmiNativeObjectFormat f){if(r==NULL||f==UMI_NC_OBJECT_UNKNOWN)return UMI_STATUS_INVALID_ARGUMENT;if(f==UMI_NC_OBJECT_COFF&&r->kind==UMI_NC_RELOC_GOT)return UMI_STATUS_UNAVAILABLE;return UMI_STATUS_OK;}
