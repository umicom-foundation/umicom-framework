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

/*
 * Provide the native web chart render operation used by this module and its client
 * applications.
 */
UmiStatus umi_native_web_chart_render(const UmiNativeWebRenderContext *context,const UmiNativeWebSurface *surface,UmiNativeWebRenderResult *result){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_native_web_render_context_validate(context)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;return umi_native_web_render_surface_wrapper(surface,"umicom-chart","img",result);}
/*
 * Provide the native web chart renderer descriptor operation used by this module and its
 * client applications.
 */
UmiNativeWebRendererDescriptor umi_native_web_chart_renderer_descriptor(void){UmiNativeWebRendererDescriptor d={0};(void)umi_native_web_copy_text(d.renderer_id,sizeof(d.renderer_id),"native-web.chart");(void)umi_native_web_copy_text(d.semantic_contract,sizeof(d.semantic_contract),"Umicom.Ui.Chart");d.render=umi_native_web_chart_render;d.capability_flags=UMI_NATIVE_WEB_CAP_DOM_PATCH|UMI_NATIVE_WEB_CAP_ACCESSIBILITY|UMI_NATIVE_WEB_CAP_THEME|UMI_NATIVE_WEB_CAP_DENSITY;return d;}

