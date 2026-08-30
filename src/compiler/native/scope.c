/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/scope.c
 *
 * PURPOSE:
 *   Represent lexical scope ancestry and bounded symbol membership for C name lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/scope.h"
#include <string.h>
UmiStatus umi_nc_scope_init(UmiNativeScope *s,uint32_t id,uint32_t parent,uint32_t depth){if(s==NULL||id==0U)return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof(*s));s->id=id;s->parent_id=parent;s->depth=depth;return UMI_STATUS_OK;}
bool umi_nc_scope_contains_symbol(const UmiNativeScope *s,uint32_t id){if(s==NULL||id==0U)return false;for(size_t i=0U;i<s->symbol_count;i++)if(s->symbol_ids[i]==id)return true;return false;}
UmiStatus umi_nc_scope_add_symbol(UmiNativeScope *s,uint32_t id){if(s==NULL||id==0U)return UMI_STATUS_INVALID_ARGUMENT;if(umi_nc_scope_contains_symbol(s,id))return UMI_STATUS_ALREADY_EXISTS;if(s->symbol_count>=UMI_NC_MAX_SCOPE_SYMBOLS)return UMI_STATUS_CAPACITY_EXCEEDED;s->symbol_ids[s->symbol_count++]=id;return UMI_STATUS_OK;}
