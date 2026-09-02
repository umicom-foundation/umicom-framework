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
/*
 * Initialise nc scope from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_nc_scope_init(UmiNativeScope *s,uint32_t id,uint32_t parent,uint32_t depth){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||id==0U)return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof(*s));s->id=id;s->parent_id=parent;s->depth=depth;return UMI_STATUS_OK;}
/*
 * Provide the nc scope contains symbol operation used by this module and its client
 * applications.
 */
bool umi_nc_scope_contains_symbol(const UmiNativeScope *s,uint32_t id){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||id==0U)return false;/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<s->symbol_count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->symbol_ids[i]==id)return true;return false;}
/*
 * Provide the nc scope add symbol operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_scope_add_symbol(UmiNativeScope *s,uint32_t id){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||id==0U)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_scope_contains_symbol(s,id))return UMI_STATUS_ALREADY_EXISTS;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->symbol_count>=UMI_NC_MAX_SCOPE_SYMBOLS)return UMI_STATUS_CAPACITY_EXCEEDED;s->symbol_ids[s->symbol_count++]=id;return UMI_STATUS_OK;}
