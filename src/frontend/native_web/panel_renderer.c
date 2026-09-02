/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/panel_renderer.c
 *
 * PURPOSE:
 *   Render reusable panel chrome, context-link identity and content slots.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/panel_renderer.h"

/*
 * Provide the native web panel render operation used by this module and its client
 * applications.
 */
UmiStatus umi_native_web_panel_render(const UmiNativeWebRenderContext *context,const UmiNativeWebSurface *surface,UmiNativeWebRenderResult *result){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_native_web_render_context_validate(context)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;return umi_native_web_render_surface_wrapper(surface,"umicom-panel","region",result);}
/*
 * Provide the native web panel renderer descriptor operation used by this module and its
 * client applications.
 */
UmiNativeWebRendererDescriptor umi_native_web_panel_renderer_descriptor(void){UmiNativeWebRendererDescriptor d={0};(void)umi_native_web_copy_text(d.renderer_id,sizeof(d.renderer_id),"native-web.panel");(void)umi_native_web_copy_text(d.semantic_contract,sizeof(d.semantic_contract),"Umicom.Ui.Panel");d.render=umi_native_web_panel_render;d.capability_flags=UMI_NATIVE_WEB_CAP_DOM_PATCH|UMI_NATIVE_WEB_CAP_ACCESSIBILITY|UMI_NATIVE_WEB_CAP_THEME|UMI_NATIVE_WEB_CAP_DENSITY;return d;}

