/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/symbol.c
 *
 * PURPOSE:
 *   Implement document symbol and outline entries reusable with LSP or native parsers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This is a product-neutral C23 model. The registry owns snapshot copies by
 * value; callers own external resources and coordinate cross-thread mutation.
 */
#include "umicom/editor/symbol.h"
#include <stdlib.h>
#include <string.h>
struct UmiEditorSymbolRegistry { UmiEditorSymbolSnapshot items[UMI_EDITOR_SYMBOL_CAPACITY]; size_t count; uint64_t revision; };
/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiEditorSymbolRegistry*r,const char*id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||id==NULL)return SIZE_MAX;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<r->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(r->items[i].id,id)==0)return i;return SIZE_MAX;}
/*
 * Initialise editor symbol registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_symbol_registry_create(UmiEditorSymbolRegistry **out){UmiEditorSymbolRegistry*r;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;r=calloc(1U,sizeof(*r));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL)return UMI_STATUS_OUT_OF_MEMORY;r->revision=1U;*out=r;return UMI_STATUS_OK;}
/*
 * Release or reset state held by editor symbol registry so the same storage can be reused
 * safely.
 */
void umi_editor_symbol_registry_destroy(UmiEditorSymbolRegistry*r){free(r);}
/*
 * Provide the editor symbol registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_symbol_registry_upsert(UmiEditorSymbolRegistry*r,const UmiEditorSymbolSnapshot*item){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||item==NULL||item->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;i=find_index(r,item->id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r->count>=UMI_EDITOR_SYMBOL_CAPACITY)return UMI_STATUS_CAPACITY_EXCEEDED;i=r->count++;}r->items[i]=*item;r->items[i].struct_size=(uint32_t)sizeof(UmiEditorSymbolSnapshot);r->items[i].api_version=1U;r->items[i].revision=++r->revision;return UMI_STATUS_OK;}
/*
 * Remove editor symbol registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_editor_symbol_registry_remove(UmiEditorSymbolRegistry*r,const char*id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;i=find_index(r,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX)return UMI_STATUS_NOT_FOUND;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i+1U<r->count)memmove(&r->items[i],&r->items[i+1U],(r->count-i-1U)*sizeof(r->items[0]));r->count--;r->revision++;return UMI_STATUS_OK;}
/*
 * Find editor symbol registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_editor_symbol_registry_find(const UmiEditorSymbolRegistry*r,const char*id,UmiEditorSymbolSnapshot*out){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||id==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;i=find_index(r,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX)return UMI_STATUS_NOT_FOUND;*out=r->items[i];return UMI_STATUS_OK;}
/*
 * Find editor symbol registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_editor_symbol_registry_at(const UmiEditorSymbolRegistry*r,size_t i,UmiEditorSymbolSnapshot*out){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i>=r->count)return UMI_STATUS_NOT_FOUND;*out=r->items[i];return UMI_STATUS_OK;}
/*
 * Return the number of records represented by editor symbol registry without changing
 * their state.
 */
size_t umi_editor_symbol_registry_count(const UmiEditorSymbolRegistry*r){return r!=NULL?r->count:0U;}
/*
 * Provide the editor symbol registry revision operation used by this module and its client
 * applications.
 */
uint64_t umi_editor_symbol_registry_revision(const UmiEditorSymbolRegistry*r){return r!=NULL?r->revision:0U;}
