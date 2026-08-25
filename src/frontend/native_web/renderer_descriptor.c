/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/renderer_descriptor.c
 *
 * PURPOSE:
 *   Describe semantic-contract renderers and invoke them through a stable C function table.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/renderer_descriptor.h"

#include "umicom/frontend/native_web/html_escape.h"

UmiStatus umi_native_web_renderer_descriptor_validate(const UmiNativeWebRendererDescriptor *descriptor){if(descriptor==NULL||descriptor->renderer_id[0]=='\0'||descriptor->semantic_contract[0]=='\0'||descriptor->render==NULL)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}

UmiStatus umi_native_web_render_surface_wrapper(const UmiNativeWebSurface *surface,const char *css_class,const char *role,UmiNativeWebRenderResult *result)
{
    UmiStatus s; if(surface==NULL||css_class==NULL||role==NULL||result==NULL||surface->surface_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_native_web_markup_buffer_append(&result->markup,"<section id=\"");if(s!=UMI_STATUS_OK)return s;s=umi_native_web_html_escape_attribute(&result->markup,surface->surface_id);if(s!=UMI_STATUS_OK)return s;
    s=umi_native_web_markup_buffer_append(&result->markup,"\" class=\"");if(s!=UMI_STATUS_OK)return s;s=umi_native_web_html_escape_attribute(&result->markup,css_class);if(s!=UMI_STATUS_OK)return s;
    s=umi_native_web_markup_buffer_append(&result->markup,"\" role=\"");if(s!=UMI_STATUS_OK)return s;s=umi_native_web_html_escape_attribute(&result->markup,role);if(s!=UMI_STATUS_OK)return s;s=umi_native_web_markup_buffer_append(&result->markup,"\">");if(s!=UMI_STATUS_OK)return s;
    if(surface->title[0]!='\0'){s=umi_native_web_markup_buffer_append(&result->markup,"<header>");if(s!=UMI_STATUS_OK)return s;s=umi_native_web_html_escape_text(&result->markup,surface->title);if(s!=UMI_STATUS_OK)return s;s=umi_native_web_markup_buffer_append(&result->markup,"</header>");if(s!=UMI_STATUS_OK)return s;}
    if(surface->content[0]!='\0'){s=umi_native_web_markup_buffer_append(&result->markup,"<div class=\"umicom-surface-content\">");if(s!=UMI_STATUS_OK)return s;s=umi_native_web_html_escape_text(&result->markup,surface->content);if(s!=UMI_STATUS_OK)return s;s=umi_native_web_markup_buffer_append(&result->markup,"</div>");if(s!=UMI_STATUS_OK)return s;}
    s=umi_native_web_markup_buffer_append(&result->markup,"</section>");if(s==UMI_STATUS_OK)umi_native_web_render_result_note_element(result);return s;
}

