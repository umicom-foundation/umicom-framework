/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/code_action.c
 *
 * PURPOSE:
 *   Implement editor code actions that execute through canonical command IDs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This is a product-neutral C23 model. The registry owns snapshot copies by
 * value; callers own external resources and coordinate cross-thread mutation.
 */
#include "umicom/editor/code_action.h"
#include <stdlib.h>
#include <string.h>
struct UmiEditorCodeActionRegistry { UmiEditorCodeActionSnapshot items[UMI_EDITOR_CODE_ACTION_CAPACITY]; size_t count; uint64_t revision; };
static size_t find_index(const UmiEditorCodeActionRegistry*r,const char*id){size_t i;if(r==NULL||id==NULL)return SIZE_MAX;for(i=0U;i<r->count;++i)if(strcmp(r->items[i].id,id)==0)return i;return SIZE_MAX;}
UmiStatus umi_editor_code_action_registry_create(UmiEditorCodeActionRegistry **out){UmiEditorCodeActionRegistry*r;if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;r=calloc(1U,sizeof(*r));if(r==NULL)return UMI_STATUS_OUT_OF_MEMORY;r->revision=1U;*out=r;return UMI_STATUS_OK;}
void umi_editor_code_action_registry_destroy(UmiEditorCodeActionRegistry*r){free(r);}
UmiStatus umi_editor_code_action_registry_upsert(UmiEditorCodeActionRegistry*r,const UmiEditorCodeActionSnapshot*item){size_t i;if(r==NULL||item==NULL||item->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;i=find_index(r,item->id);if(i==SIZE_MAX){if(r->count>=UMI_EDITOR_CODE_ACTION_CAPACITY)return UMI_STATUS_CAPACITY_EXCEEDED;i=r->count++;}r->items[i]=*item;r->items[i].struct_size=(uint32_t)sizeof(UmiEditorCodeActionSnapshot);r->items[i].api_version=1U;r->items[i].revision=++r->revision;return UMI_STATUS_OK;}
UmiStatus umi_editor_code_action_registry_remove(UmiEditorCodeActionRegistry*r,const char*id){size_t i;if(r==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;i=find_index(r,id);if(i==SIZE_MAX)return UMI_STATUS_NOT_FOUND;if(i+1U<r->count)memmove(&r->items[i],&r->items[i+1U],(r->count-i-1U)*sizeof(r->items[0]));r->count--;r->revision++;return UMI_STATUS_OK;}
UmiStatus umi_editor_code_action_registry_find(const UmiEditorCodeActionRegistry*r,const char*id,UmiEditorCodeActionSnapshot*out){size_t i;if(r==NULL||id==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;i=find_index(r,id);if(i==SIZE_MAX)return UMI_STATUS_NOT_FOUND;*out=r->items[i];return UMI_STATUS_OK;}
UmiStatus umi_editor_code_action_registry_at(const UmiEditorCodeActionRegistry*r,size_t i,UmiEditorCodeActionSnapshot*out){if(r==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(i>=r->count)return UMI_STATUS_NOT_FOUND;*out=r->items[i];return UMI_STATUS_OK;}
size_t umi_editor_code_action_registry_count(const UmiEditorCodeActionRegistry*r){return r!=NULL?r->count:0U;}
uint64_t umi_editor_code_action_registry_revision(const UmiEditorCodeActionRegistry*r){return r!=NULL?r->revision:0U;}
