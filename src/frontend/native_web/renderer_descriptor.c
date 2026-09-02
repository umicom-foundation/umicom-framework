/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/renderer_descriptor.c
 *
 * PURPOSE:
 *   Describe semantic-contract renderers and invoke them through a stable C function table.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/renderer_descriptor.h"

#include "umicom/frontend/native_web/html_escape.h"

/*
 * Check that native web renderer descriptor satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_native_web_renderer_descriptor_validate(const UmiNativeWebRendererDescriptor *descriptor){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(descriptor==NULL||descriptor->renderer_id[0]=='\0'||descriptor->semantic_contract[0]=='\0'||descriptor->render==NULL)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}

/*
 * Provide the native web render surface wrapper operation used by this module and its
 * client applications.
 */
UmiStatus umi_native_web_render_surface_wrapper(const UmiNativeWebSurface *surface,const char *css_class,const char *role,UmiNativeWebRenderResult *result)
{
    UmiStatus s; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(surface==NULL||css_class==NULL||role==NULL||result==NULL||surface->surface_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_native_web_markup_buffer_append(&result->markup,"<section id=\"");/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_native_web_html_escape_attribute(&result->markup,surface->surface_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;
    s=umi_native_web_markup_buffer_append(&result->markup,"\" class=\"");/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_native_web_html_escape_attribute(&result->markup,css_class);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;
    s=umi_native_web_markup_buffer_append(&result->markup,"\" role=\"");/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_native_web_html_escape_attribute(&result->markup,role);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_native_web_markup_buffer_append(&result->markup,"\">");/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(surface->title[0]!='\0'){s=umi_native_web_markup_buffer_append(&result->markup,"<header>");/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_native_web_html_escape_text(&result->markup,surface->title);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_native_web_markup_buffer_append(&result->markup,"</header>");/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;}
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(surface->content[0]!='\0'){s=umi_native_web_markup_buffer_append(&result->markup,"<div class=\"umicom-surface-content\">");/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_native_web_html_escape_text(&result->markup,surface->content);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_native_web_markup_buffer_append(&result->markup,"</div>");/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;}
    s=umi_native_web_markup_buffer_append(&result->markup,"</section>");/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s==UMI_STATUS_OK)umi_native_web_render_result_note_element(result);return s;
}

