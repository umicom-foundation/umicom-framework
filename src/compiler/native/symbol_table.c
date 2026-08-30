/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/symbol_table.c
 *
 * PURPOSE:
 *   Provide scope-aware symbol registration and lexical name lookup with deterministic shadowing semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/symbol_table.h"
#include <string.h>
void umi_nc_symbol_table_init(UmiNativeSymbolTable *t){if(t!=NULL)memset(t,0,sizeof(*t));}
const UmiNativeScope *umi_nc_symbol_table_scope(const UmiNativeSymbolTable *t,uint32_t id){if(t==NULL||id==0U)return NULL;for(size_t i=0U;i<t->scope_count;i++)if(t->scopes[i].id==id)return &t->scopes[i];return NULL;}
UmiStatus umi_nc_symbol_table_add_scope(UmiNativeSymbolTable *t,const UmiNativeScope *s){if(t==NULL||s==NULL||s->id==0U)return UMI_STATUS_INVALID_ARGUMENT;if(umi_nc_symbol_table_scope(t,s->id)!=NULL)return UMI_STATUS_ALREADY_EXISTS;if(t->scope_count>=UMI_NC_MAX_SCOPES)return UMI_STATUS_CAPACITY_EXCEEDED;t->scopes[t->scope_count++]=*s;t->revision++;return UMI_STATUS_OK;}
UmiStatus umi_nc_symbol_table_add_symbol(UmiNativeSymbolTable *t,const UmiNativeSymbol *s){if(t==NULL||s==NULL||umi_nc_symbol_validate(s)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;UmiNativeScope *scope=NULL;for(size_t i=0U;i<t->scope_count;i++)if(t->scopes[i].id==s->scope_id){scope=&t->scopes[i];break;}if(scope==NULL)return UMI_STATUS_NOT_FOUND;for(size_t i=0U;i<t->symbol_count;i++)if(t->symbols[i].scope_id==s->scope_id&&strcmp(t->symbols[i].name,s->name)==0)return UMI_STATUS_ALREADY_EXISTS;if(t->symbol_count>=UMI_NC_MAX_SYMBOLS)return UMI_STATUS_CAPACITY_EXCEEDED;t->symbols[t->symbol_count]=*s;UmiStatus st=umi_nc_scope_add_symbol(scope,s->id);if(st!=UMI_STATUS_OK)return st;t->symbol_count++;t->revision++;return UMI_STATUS_OK;}
const UmiNativeSymbol *umi_nc_symbol_table_lookup(const UmiNativeSymbolTable *t,uint32_t scope_id,const char *name){if(t==NULL||scope_id==0U||name==NULL)return NULL;uint32_t current=scope_id;for(size_t depth=0U;depth<UMI_NC_MAX_SCOPES&&current!=0U;depth++){for(size_t i=t->symbol_count;i>0U;i--){const UmiNativeSymbol *s=&t->symbols[i-1U];if(s->scope_id==current&&strcmp(s->name,name)==0)return s;}const UmiNativeScope *scope=umi_nc_symbol_table_scope(t,current);if(scope==NULL)break;current=scope->parent_id;}return NULL;}
