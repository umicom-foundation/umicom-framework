/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/element_index.c
 *
 * PURPOSE:
 *   Build and query a stable element-ID index for fast render/diff operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/element_index.h"

#include <string.h>

/*
 * Provide the native web element index build operation used by this module and its client
 * applications.
 */
UmiStatus umi_native_web_element_index_build(UmiNativeWebElementIndex *index, const UmiNativeWebElementTree *tree)
{
    size_t i; UmiStatus s;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index == NULL || tree == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(index,0,sizeof(*index));
    /* Visit each bounded item once so every record receives the same rule. */
    for(i=0U;i<tree->count;++i){ s=umi_native_web_copy_text(index->entries[i].element_id,sizeof(index->entries[i].element_id),tree->elements[i].element_id); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s!=UMI_STATUS_OK)return s; index->entries[i].index=i; }
    index->count=tree->count; index->source_revision=tree->revision; return UMI_STATUS_OK;
}

/*
 * Find native web element index while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_native_web_element_index_find(const UmiNativeWebElementIndex *index, const char *element_id, size_t *out_index)
{
    size_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(index==NULL||element_id==NULL||out_index==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for(i=0U;i<index->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(index->entries[i].element_id,element_id)==0){*out_index=index->entries[i].index;return UMI_STATUS_OK;}
    return UMI_STATUS_NOT_FOUND;
}

