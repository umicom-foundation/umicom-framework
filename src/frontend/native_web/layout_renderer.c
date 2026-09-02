/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/layout_renderer.c
 *
 * PURPOSE:
 *   Render semantic workstation layout nodes without serializing GTK4 or Qt6 native widgets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/layout_renderer.h"

/*
 * Provide the native web layout render operation used by this module and its client
 * applications.
 */
UmiStatus umi_native_web_layout_render(const UmiNativeWebRenderContext *context,const UmiNativeWebSurface *surface,UmiNativeWebRenderResult *result){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_native_web_render_context_validate(context)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;return umi_native_web_render_surface_wrapper(surface,"umicom-layout","group",result);}
/*
 * Provide the native web layout renderer descriptor operation used by this module and its
 * client applications.
 */
UmiNativeWebRendererDescriptor umi_native_web_layout_renderer_descriptor(void){UmiNativeWebRendererDescriptor d={0};(void)umi_native_web_copy_text(d.renderer_id,sizeof(d.renderer_id),"native-web.layout");(void)umi_native_web_copy_text(d.semantic_contract,sizeof(d.semantic_contract),"Umicom.Ui.Layout");d.render=umi_native_web_layout_render;d.capability_flags=UMI_NATIVE_WEB_CAP_DOM_PATCH|UMI_NATIVE_WEB_CAP_ACCESSIBILITY|UMI_NATIVE_WEB_CAP_THEME|UMI_NATIVE_WEB_CAP_DENSITY;return d;}

