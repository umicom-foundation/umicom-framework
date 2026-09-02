/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/symbol.c
 *
 * PURPOSE:
 *   Describe native compiler symbols, linkage and type ownership independently from storage implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/symbol.h"
#include <string.h>
/*
 * Initialise nc symbol from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_nc_symbol_init(UmiNativeSymbol *s,uint32_t id,UmiNativeSymbolKind kind,const char *name,uint32_t type,uint32_t scope){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||id==0U||name==NULL||name[0]=='\0'||scope==0U)return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof(*s));s->id=id;s->kind=kind;s->type_id=type;s->scope_id=scope;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_copy_text(s->name,sizeof(s->name),name)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;return UMI_STATUS_OK;}
/* Check that nc symbol satisfies its contract before another service relies on it. */
UmiStatus umi_nc_symbol_validate(const UmiNativeSymbol *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||s->id==0U||s->scope_id==0U||s->name[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->external_linkage&&s->internal_linkage)return UMI_STATUS_INVALID_STATE;return UMI_STATUS_OK;}
