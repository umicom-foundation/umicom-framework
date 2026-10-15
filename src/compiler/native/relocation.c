/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/relocation.c
 *
 * PURPOSE:
 *   Represent object relocation requests independently from ELF, COFF and Mach-O writers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/relocation.h"
#include "../../base/value_construction_internal.h"
#include <string.h>
/*
 * Initialise nc relocation from caller-provided values so later operations receive a known
 * state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_nc_relocation_init(UmiNativeRelocation *r,UmiNativeRelocationKind kind,const char *section,uint64_t off,const char *symbol,int64_t add){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||section==NULL||symbol==NULL||section[0]=='\0'||symbol[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;memset(r,0,sizeof(*r));r->kind=kind;r->offset=off;r->addend=add;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_copy_text(r->section,sizeof(r->section),section)!=UMI_STATUS_OK||umi_nc_copy_text(r->symbol,sizeof(r->symbol),symbol)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;return UMI_STATUS_OK;}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_nc_relocation_init_build_value(UmiNativeRelocation *r,UmiNativeRelocationKind kind,const char *section,uint64_t off,const char *symbol,int64_t add){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||section==NULL||symbol==NULL||section[0]=='\0'||symbol[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;memset(r,0,sizeof(*r));r->kind=kind;r->offset=off;r->addend=add;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_copy_text(r->section,sizeof(r->section),section)!=UMI_STATUS_OK||umi_nc_copy_text(r->symbol,sizeof(r->symbol),symbol)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;return UMI_STATUS_OK;}

UMI_DEFINE_STAGED_VALUE_INIT(umi_nc_relocation_init, UmiNativeRelocation, r,
    (UmiNativeRelocation *r, UmiNativeRelocationKind kind, const char *section, uint64_t off, const char *symbol, int64_t add),
    umi_nc_relocation_init_build_value(candidate, kind, section, off, symbol, add))

/* Check that nc relocation satisfies its contract before another service relies on it. */
UmiStatus umi_nc_relocation_validate(const UmiNativeRelocation *r,UmiNativeObjectFormat f){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||f==UMI_NC_OBJECT_UNKNOWN)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(f==UMI_NC_OBJECT_COFF&&r->kind==UMI_NC_RELOC_GOT)return UMI_STATUS_UNAVAILABLE;return UMI_STATUS_OK;}
