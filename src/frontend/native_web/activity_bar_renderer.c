/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/activity_bar_renderer.c
 *
 * PURPOSE:
 *   Render edge activity/tool rails from semantic workbench activity descriptors.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/activity_bar_renderer.h"

/*
 * Provide the native web activity bar render operation used by this module and its client
 * applications.
 */
UmiStatus umi_native_web_activity_bar_render(const UmiNativeWebRenderContext *context,const UmiNativeWebSurface *surface,UmiNativeWebRenderResult *result){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_native_web_render_context_validate(context)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;return umi_native_web_render_surface_wrapper(surface,"umicom-activity-bar","navigation",result);}
/*
 * Provide the native web activity bar renderer descriptor operation used by this module
 * and its client applications.
 */
UmiNativeWebRendererDescriptor umi_native_web_activity_bar_renderer_descriptor(void){UmiNativeWebRendererDescriptor d={0};(void)umi_native_web_copy_text(d.renderer_id,sizeof(d.renderer_id),"native-web.activity-bar");(void)umi_native_web_copy_text(d.semantic_contract,sizeof(d.semantic_contract),"Umicom.Ui.ActivityBar");d.render=umi_native_web_activity_bar_render;d.capability_flags=UMI_NATIVE_WEB_CAP_DOM_PATCH|UMI_NATIVE_WEB_CAP_ACCESSIBILITY|UMI_NATIVE_WEB_CAP_THEME|UMI_NATIVE_WEB_CAP_DENSITY;return d;}

