/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/render_node_bridge.c
 *
 * PURPOSE:
 *   Project the existing Umicom frontend render-tree snapshot into native-web semantic elements.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/render_node_bridge.h"

#include <string.h>

UmiStatus umi_native_web_render_node_bridge(const UmiFrontendRenderNodeSnapshot *snapshot, UmiNativeWebSemanticElement *out_element)
{
    UmiStatus status;
    if(snapshot==NULL||out_element==NULL||snapshot->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    status=umi_native_web_semantic_element_init(out_element,snapshot->id,"div"); if(status!=UMI_STATUS_OK)return status;
    if(snapshot->parent_id[0]!='\0'){status=umi_native_web_copy_text(out_element->parent_id,sizeof(out_element->parent_id),snapshot->parent_id);if(status!=UMI_STATUS_OK)return status;}
    status=umi_native_web_copy_text(out_element->text,sizeof(out_element->text),snapshot->markup); if(status!=UMI_STATUS_OK)return status;
    out_element->order=snapshot->order; out_element->revision=snapshot->revision; return UMI_STATUS_OK;
}

