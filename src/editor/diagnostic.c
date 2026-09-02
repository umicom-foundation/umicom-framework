/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/diagnostic.c
 *
 * PURPOSE:
 *   Implement editor diagnostics independently of their compiler, linter or language-server provider.
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
#include "umicom/editor/diagnostic.h"
#include <stdlib.h>
#include <string.h>
struct UmiEditorDiagnosticRegistry { UmiEditorDiagnosticSnapshot items[UMI_EDITOR_DIAGNOSTIC_CAPACITY]; size_t count; uint64_t revision; };
/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiEditorDiagnosticRegistry*r,const char*id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||id==NULL)return SIZE_MAX;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<r->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(r->items[i].id,id)==0)return i;return SIZE_MAX;}
/*
 * Initialise editor diagnostic registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_diagnostic_registry_create(UmiEditorDiagnosticRegistry **out){UmiEditorDiagnosticRegistry*r;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;r=calloc(1U,sizeof(*r));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL)return UMI_STATUS_OUT_OF_MEMORY;r->revision=1U;*out=r;return UMI_STATUS_OK;}
/*
 * Release or reset state held by editor diagnostic registry so the same storage can be
 * reused safely.
 */
void umi_editor_diagnostic_registry_destroy(UmiEditorDiagnosticRegistry*r){free(r);}
/*
 * Provide the editor diagnostic registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_diagnostic_registry_upsert(UmiEditorDiagnosticRegistry*r,const UmiEditorDiagnosticSnapshot*item){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||item==NULL||item->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;i=find_index(r,item->id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r->count>=UMI_EDITOR_DIAGNOSTIC_CAPACITY)return UMI_STATUS_CAPACITY_EXCEEDED;i=r->count++;}r->items[i]=*item;r->items[i].struct_size=(uint32_t)sizeof(UmiEditorDiagnosticSnapshot);r->items[i].api_version=1U;r->items[i].revision=++r->revision;return UMI_STATUS_OK;}
/*
 * Remove editor diagnostic registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_editor_diagnostic_registry_remove(UmiEditorDiagnosticRegistry*r,const char*id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;i=find_index(r,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX)return UMI_STATUS_NOT_FOUND;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i+1U<r->count)memmove(&r->items[i],&r->items[i+1U],(r->count-i-1U)*sizeof(r->items[0]));r->count--;r->revision++;return UMI_STATUS_OK;}
/*
 * Find editor diagnostic registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_editor_diagnostic_registry_find(const UmiEditorDiagnosticRegistry*r,const char*id,UmiEditorDiagnosticSnapshot*out){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||id==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;i=find_index(r,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX)return UMI_STATUS_NOT_FOUND;*out=r->items[i];return UMI_STATUS_OK;}
/*
 * Find editor diagnostic registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_editor_diagnostic_registry_at(const UmiEditorDiagnosticRegistry*r,size_t i,UmiEditorDiagnosticSnapshot*out){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i>=r->count)return UMI_STATUS_NOT_FOUND;*out=r->items[i];return UMI_STATUS_OK;}
/*
 * Return the number of records represented by editor diagnostic registry without changing
 * their state.
 */
size_t umi_editor_diagnostic_registry_count(const UmiEditorDiagnosticRegistry*r){return r!=NULL?r->count:0U;}
/*
 * Provide the editor diagnostic registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_diagnostic_registry_revision(const UmiEditorDiagnosticRegistry*r){return r!=NULL?r->revision:0U;}
