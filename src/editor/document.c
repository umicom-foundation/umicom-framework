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
/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiEditorDocumentRegistry*r,const char*id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||id==NULL)return SIZE_MAX;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<r->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(r->items[i].id,id)==0)return i;return SIZE_MAX;}
/*
 * Initialise editor document registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_document_registry_create(UmiEditorDocumentRegistry **out){UmiEditorDocumentRegistry*r;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;r=calloc(1U,sizeof(*r));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL)return UMI_STATUS_OUT_OF_MEMORY;r->revision=1U;*out=r;return UMI_STATUS_OK;}
/*
 * Release or reset state held by editor document registry so the same storage can be
 * reused safely.
 */
void umi_editor_document_registry_destroy(UmiEditorDocumentRegistry*r){free(r);}
/*
 * Provide the editor document registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_document_registry_upsert(UmiEditorDocumentRegistry*r,const UmiEditorDocumentSnapshot*item){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||item==NULL||item->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;i=find_index(r,item->id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r->count>=UMI_EDITOR_DOCUMENT_CAPACITY)return UMI_STATUS_CAPACITY_EXCEEDED;i=r->count++;}r->items[i]=*item;r->items[i].struct_size=(uint32_t)sizeof(UmiEditorDocumentSnapshot);r->items[i].api_version=1U;r->items[i].revision=++r->revision;return UMI_STATUS_OK;}
/*
 * Remove editor document registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_editor_document_registry_remove(UmiEditorDocumentRegistry*r,const char*id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;i=find_index(r,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX)return UMI_STATUS_NOT_FOUND;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i+1U<r->count)memmove(&r->items[i],&r->items[i+1U],(r->count-i-1U)*sizeof(r->items[0]));r->count--;r->revision++;return UMI_STATUS_OK;}
/*
 * Find editor document registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_editor_document_registry_find(const UmiEditorDocumentRegistry*r,const char*id,UmiEditorDocumentSnapshot*out){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||id==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;i=find_index(r,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX)return UMI_STATUS_NOT_FOUND;*out=r->items[i];return UMI_STATUS_OK;}
/*
 * Find editor document registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_editor_document_registry_at(const UmiEditorDocumentRegistry*r,size_t i,UmiEditorDocumentSnapshot*out){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i>=r->count)return UMI_STATUS_NOT_FOUND;*out=r->items[i];return UMI_STATUS_OK;}
/*
 * Return the number of records represented by editor document registry without changing
 * their state.
 */
size_t umi_editor_document_registry_count(const UmiEditorDocumentRegistry*r){return r!=NULL?r->count:0U;}
/*
 * Provide the editor document registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_document_registry_revision(const UmiEditorDocumentRegistry*r){return r!=NULL?r->revision:0U;}
