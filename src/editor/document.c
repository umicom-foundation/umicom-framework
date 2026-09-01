/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/document.c
 *
 * PURPOSE:
 *   Implement editor-document metadata independent of the text storage implementation.
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
#include "umicom/editor/document.h"
#include <stdlib.h>
#include <string.h>
struct UmiEditorDocumentRegistry { UmiEditorDocumentSnapshot items[UMI_EDITOR_DOCUMENT_CAPACITY]; size_t count; uint64_t revision; };
static size_t find_index(const UmiEditorDocumentRegistry*r,const char*id){size_t i;if(r==NULL||id==NULL)return SIZE_MAX;for(i=0U;i<r->count;++i)if(strcmp(r->items[i].id,id)==0)return i;return SIZE_MAX;}
UmiStatus umi_editor_document_registry_create(UmiEditorDocumentRegistry **out){UmiEditorDocumentRegistry*r;if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;r=calloc(1U,sizeof(*r));if(r==NULL)return UMI_STATUS_OUT_OF_MEMORY;r->revision=1U;*out=r;return UMI_STATUS_OK;}
void umi_editor_document_registry_destroy(UmiEditorDocumentRegistry*r){free(r);}
UmiStatus umi_editor_document_registry_upsert(UmiEditorDocumentRegistry*r,const UmiEditorDocumentSnapshot*item){size_t i;if(r==NULL||item==NULL||item->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;i=find_index(r,item->id);if(i==SIZE_MAX){if(r->count>=UMI_EDITOR_DOCUMENT_CAPACITY)return UMI_STATUS_CAPACITY_EXCEEDED;i=r->count++;}r->items[i]=*item;r->items[i].struct_size=(uint32_t)sizeof(UmiEditorDocumentSnapshot);r->items[i].api_version=1U;r->items[i].revision=++r->revision;return UMI_STATUS_OK;}
UmiStatus umi_editor_document_registry_remove(UmiEditorDocumentRegistry*r,const char*id){size_t i;if(r==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;i=find_index(r,id);if(i==SIZE_MAX)return UMI_STATUS_NOT_FOUND;if(i+1U<r->count)memmove(&r->items[i],&r->items[i+1U],(r->count-i-1U)*sizeof(r->items[0]));r->count--;r->revision++;return UMI_STATUS_OK;}
UmiStatus umi_editor_document_registry_find(const UmiEditorDocumentRegistry*r,const char*id,UmiEditorDocumentSnapshot*out){size_t i;if(r==NULL||id==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;i=find_index(r,id);if(i==SIZE_MAX)return UMI_STATUS_NOT_FOUND;*out=r->items[i];return UMI_STATUS_OK;}
UmiStatus umi_editor_document_registry_at(const UmiEditorDocumentRegistry*r,size_t i,UmiEditorDocumentSnapshot*out){if(r==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(i>=r->count)return UMI_STATUS_NOT_FOUND;*out=r->items[i];return UMI_STATUS_OK;}
size_t umi_editor_document_registry_count(const UmiEditorDocumentRegistry*r){return r!=NULL?r->count:0U;}
uint64_t umi_editor_document_registry_revision(const UmiEditorDocumentRegistry*r){return r!=NULL?r->revision:0U;}
