/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media_generation/comfy_palette.c
 *
 * PURPOSE:
 *   Filter and order immutable node observations with copied ownership and exact catalogue binding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_palette.h"
#include "umicom/base/utf8.h"
#include "../base/ascii_search_internal.h"
#include <stdlib.h>
#include <string.h>
struct UmiComfyNodeSelection {
    UmiComfyNodeSelectionReview review;
    UmiComfyNodeInfo *items;
};
static int PaletteText(const char *text,size_t capacity)
{
    const char *end=memchr(text,'\0',capacity);
    return end!=NULL && UmiUtf8Validate((const unsigned char *)text,(size_t)(end-text),NULL);
}
static int PaletteCategory(const char *category)
{
    size_t length=strlen(category);
    return length==0U || (category[0]!='/' && category[length-1U]!='/' && strstr(category,"//")==NULL);
}
static int PaletteMatches(const UmiComfyNodeInfo *item,const UmiComfyNodeQuery *query)
{
    unsigned required=query->required_true_flags|query->required_false_flags;
    if((item->known_flags&required)!=required ||
       (item->enabled_flags&query->required_true_flags)!=query->required_true_flags ||
       (item->enabled_flags&query->required_false_flags)!=0U)return 0;
    size_t prefix=strlen(query->category);
    if(prefix!=0U && (strncmp(item->category,query->category,prefix)!=0 ||
        (item->category[prefix]!='\0' && item->category[prefix]!='/')))return 0;
    return UmiAsciiSearchContains(item->class_type,query->search) ||
        UmiAsciiSearchContains(item->display_name,query->search) ||
        UmiAsciiSearchContains(item->category,query->search);
}
static int PaletteClass(const void *left,const void *right)
{
    return strcmp(((const UmiComfyNodeInfo *)left)->class_type,((const UmiComfyNodeInfo *)right)->class_type);
}
static int PaletteDisplay(const void *left,const void *right)
{
    const UmiComfyNodeInfo *a=left,*b=right;int order=strcmp(a->display_name,b->display_name);
    return order!=0?order:strcmp(a->class_type,b->class_type);
}
static int PaletteCategoryOrder(const void *left,const void *right)
{
    const UmiComfyNodeInfo *a=left,*b=right;int order=strcmp(a->category,b->category);
    return order!=0?order:strcmp(a->class_type,b->class_type);
}
void UmiComfyNodeSelectionDestroy(UmiComfyNodeSelection *selection)
{
    if(selection==NULL)return;
    if(selection->items!=NULL){
        umi_secret_clear(selection->items,selection->review.source_count*sizeof(*selection->items));
        free(selection->items);
    }
    umi_secret_clear(selection,sizeof(*selection));free(selection);
}
UmiStatus UmiComfyNodeSelect(const UmiComfyNodeCatalogue *source,
    const UmiComfyNodeQuery *query,const UmiCancellationToken *cancel,UmiComfyNodeSelection **out)
{
    if(source==NULL || query==NULL || out==NULL || *out!=NULL)return UMI_STATUS_INVALID_ARGUMENT;
    const unsigned flags=UMI_COMFY_NODE_OUTPUT_NODE|UMI_COMFY_NODE_DEPRECATED|UMI_COMFY_NODE_EXPERIMENTAL|UMI_COMFY_NODE_API_NODE;
    if(!PaletteText(query->search,sizeof(query->search)) || !PaletteText(query->category,sizeof(query->category)) ||
        !PaletteCategory(query->category) || ((query->required_true_flags|query->required_false_flags)&~flags)!=0U ||
        (query->required_true_flags&query->required_false_flags)!=0U ||
        query->order<UMI_COMFY_NODE_ORDER_SOURCE || query->order>UMI_COMFY_NODE_ORDER_CATEGORY)return UMI_STATUS_INVALID_ARGUMENT;
    if(umi_cancellation_token_is_requested(cancel))return UMI_STATUS_CANCELLED;
    UmiComfyNodeSelection *selection=calloc(1U,sizeof(*selection));
    if(selection==NULL)return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status=UmiComfyNodeCatalogueInspect(source,&selection->review.catalogue);
    if(status!=UMI_STATUS_OK)goto done;
    selection->review.source_count=UmiComfyNodeCatalogueCount(source);
    if(selection->review.source_count>UMI_COMFY_NODES_LIMIT){status=UMI_STATUS_INVALID_STATE;goto done;}
    if(selection->review.source_count!=0U){
        selection->items=calloc(selection->review.source_count,sizeof(*selection->items));
        if(selection->items==NULL){status=UMI_STATUS_OUT_OF_MEMORY;goto done;}
    }
    for(size_t i=0U;i<selection->review.source_count;++i){
        if(umi_cancellation_token_is_requested(cancel)){status=UMI_STATUS_CANCELLED;goto done;}
        UmiComfyNodeInfo item;status=UmiComfyNodeCatalogueAt(source,i,&item);
        if(status!=UMI_STATUS_OK)goto done;
        if(PaletteMatches(&item,query))selection->items[selection->review.selected_count++]=item;
    }
    int (*compare)(const void *,const void *)=NULL;
    switch(query->order){
    case UMI_COMFY_NODE_ORDER_CLASS:compare=PaletteClass;break;
    case UMI_COMFY_NODE_ORDER_DISPLAY:compare=PaletteDisplay;break;
    case UMI_COMFY_NODE_ORDER_CATEGORY:compare=PaletteCategoryOrder;break;
    default:break;
    }
    if(umi_cancellation_token_is_requested(cancel)){status=UMI_STATUS_CANCELLED;goto done;}
    if(compare!=NULL && selection->review.selected_count>1U)
        qsort(selection->items,selection->review.selected_count,sizeof(*selection->items),compare);
    if(umi_cancellation_token_is_requested(cancel)){status=UMI_STATUS_CANCELLED;goto done;}
    *out=selection;selection=NULL;
done:
    UmiComfyNodeSelectionDestroy(selection);return status;
}
UmiStatus UmiComfyNodeSelectionInspect(const UmiComfyNodeSelection *selection,UmiComfyNodeSelectionReview *out)
{
    if(selection==NULL || out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    *out=selection->review;return UMI_STATUS_OK;
}
UmiStatus UmiComfyNodeSelectionAt(const UmiComfyNodeSelection *selection,size_t index,UmiComfyNodeInfo *out)
{
    if(selection==NULL || out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if(index>=selection->review.selected_count)return UMI_STATUS_NOT_FOUND;
    *out=selection->items[index];return UMI_STATUS_OK;
}
UmiStatus UmiComfyNodeSelectionCheckCatalogue(const UmiComfyNodeSelection *selection,const UmiComfyNodeCatalogue *catalogue)
{
    if(selection==NULL || catalogue==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    UmiComfyNodeCatalogueReview current;UmiStatus status=UmiComfyNodeCatalogueInspect(catalogue,&current);
    if(status!=UMI_STATUS_OK)return status;
    const UmiComfyNodeCatalogueReview *saved=&selection->review.catalogue;
    if(saved->connection_revision!=current.connection_revision ||
       memcmp(saved->connection_sha256,current.connection_sha256,UMI_SHA256_BYTES)!=0 ||
       memcmp(saved->response_sha256,current.response_sha256,UMI_SHA256_BYTES)!=0)return UMI_STATUS_BUSY;
    return UMI_STATUS_OK;
}
