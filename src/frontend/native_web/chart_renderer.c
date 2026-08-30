/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/chart_renderer.c
 *
 * PURPOSE:
 *   Render chart viewport scaffolding suitable for later Canvas/SVG/WebGL adapters without coupling chart state to JavaScript.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/chart_renderer.h"

UmiStatus umi_native_web_chart_render(const UmiNativeWebRenderContext *context,const UmiNativeWebSurface *surface,UmiNativeWebRenderResult *result){if(umi_native_web_render_context_validate(context)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;return umi_native_web_render_surface_wrapper(surface,"umicom-chart","img",result);}
UmiNativeWebRendererDescriptor umi_native_web_chart_renderer_descriptor(void){UmiNativeWebRendererDescriptor d={0};(void)umi_native_web_copy_text(d.renderer_id,sizeof(d.renderer_id),"native-web.chart");(void)umi_native_web_copy_text(d.semantic_contract,sizeof(d.semantic_contract),"Umicom.Ui.Chart");d.render=umi_native_web_chart_render;d.capability_flags=UMI_NATIVE_WEB_CAP_DOM_PATCH|UMI_NATIVE_WEB_CAP_ACCESSIBILITY|UMI_NATIVE_WEB_CAP_THEME|UMI_NATIVE_WEB_CAP_DENSITY;return d;}

