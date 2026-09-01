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
static size_t find_index(const UmiEditorDiagnosticRegistry*r,const char*id){size_t i;if(r==NULL||id==NULL)return SIZE_MAX;for(i=0U;i<r->count;++i)if(strcmp(r->items[i].id,id)==0)return i;return SIZE_MAX;}
UmiStatus umi_editor_diagnostic_registry_create(UmiEditorDiagnosticRegistry **out){UmiEditorDiagnosticRegistry*r;if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;r=calloc(1U,sizeof(*r));if(r==NULL)return UMI_STATUS_OUT_OF_MEMORY;r->revision=1U;*out=r;return UMI_STATUS_OK;}
void umi_editor_diagnostic_registry_destroy(UmiEditorDiagnosticRegistry*r){free(r);}
UmiStatus umi_editor_diagnostic_registry_upsert(UmiEditorDiagnosticRegistry*r,const UmiEditorDiagnosticSnapshot*item){size_t i;if(r==NULL||item==NULL||item->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;i=find_index(r,item->id);if(i==SIZE_MAX){if(r->count>=UMI_EDITOR_DIAGNOSTIC_CAPACITY)return UMI_STATUS_CAPACITY_EXCEEDED;i=r->count++;}r->items[i]=*item;r->items[i].struct_size=(uint32_t)sizeof(UmiEditorDiagnosticSnapshot);r->items[i].api_version=1U;r->items[i].revision=++r->revision;return UMI_STATUS_OK;}
UmiStatus umi_editor_diagnostic_registry_remove(UmiEditorDiagnosticRegistry*r,const char*id){size_t i;if(r==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;i=find_index(r,id);if(i==SIZE_MAX)return UMI_STATUS_NOT_FOUND;if(i+1U<r->count)memmove(&r->items[i],&r->items[i+1U],(r->count-i-1U)*sizeof(r->items[0]));r->count--;r->revision++;return UMI_STATUS_OK;}
UmiStatus umi_editor_diagnostic_registry_find(const UmiEditorDiagnosticRegistry*r,const char*id,UmiEditorDiagnosticSnapshot*out){size_t i;if(r==NULL||id==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;i=find_index(r,id);if(i==SIZE_MAX)return UMI_STATUS_NOT_FOUND;*out=r->items[i];return UMI_STATUS_OK;}
UmiStatus umi_editor_diagnostic_registry_at(const UmiEditorDiagnosticRegistry*r,size_t i,UmiEditorDiagnosticSnapshot*out){if(r==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(i>=r->count)return UMI_STATUS_NOT_FOUND;*out=r->items[i];return UMI_STATUS_OK;}
size_t umi_editor_diagnostic_registry_count(const UmiEditorDiagnosticRegistry*r){return r!=NULL?r->count:0U;}
uint64_t umi_editor_diagnostic_registry_revision(const UmiEditorDiagnosticRegistry*r){return r!=NULL?r->revision:0U;}
