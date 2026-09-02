/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/render_node_bridge.c
 *
 * PURPOSE:
 *   Project the existing Umicom frontend render-tree snapshot into native-web semantic elements.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/render_node_bridge.h"

#include <string.h>

/*
 * Provide the native web render node bridge operation used by this module and its client
 * applications.
 */
UmiStatus umi_native_web_render_node_bridge(const UmiFrontendRenderNodeSnapshot *snapshot, UmiNativeWebSemanticElement *out_element)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(snapshot==NULL||out_element==NULL||snapshot->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    status=umi_native_web_semantic_element_init(out_element,snapshot->id,"div"); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(status!=UMI_STATUS_OK)return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(snapshot->parent_id[0]!='\0'){status=umi_native_web_copy_text(out_element->parent_id,sizeof(out_element->parent_id),snapshot->parent_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(status!=UMI_STATUS_OK)return status;}
    status=umi_native_web_copy_text(out_element->text,sizeof(out_element->text),snapshot->markup); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(status!=UMI_STATUS_OK)return status;
    out_element->order=snapshot->order; out_element->revision=snapshot->revision; return UMI_STATUS_OK;
}

