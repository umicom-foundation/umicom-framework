/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/timeline_renderer.c
 *
 * PURPOSE:
 *   Render timeline tracks, clips and playhead state for video, audio, animation and replay applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/timeline_renderer.h"

/*
 * Provide the native web timeline render operation used by this module and its client
 * applications.
 */
UmiStatus umi_native_web_timeline_render(const UmiNativeWebRenderContext *context,const UmiNativeWebSurface *surface,UmiNativeWebRenderResult *result){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_native_web_render_context_validate(context)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;return umi_native_web_render_surface_wrapper(surface,"umicom-timeline","group",result);}
/*
 * Provide the native web timeline renderer descriptor operation used by this module and
 * its client applications.
 */
UmiNativeWebRendererDescriptor umi_native_web_timeline_renderer_descriptor(void){UmiNativeWebRendererDescriptor d={0};(void)umi_native_web_copy_text(d.renderer_id,sizeof(d.renderer_id),"native-web.timeline");(void)umi_native_web_copy_text(d.semantic_contract,sizeof(d.semantic_contract),"Umicom.Ui.Timeline");d.render=umi_native_web_timeline_render;d.capability_flags=UMI_NATIVE_WEB_CAP_DOM_PATCH|UMI_NATIVE_WEB_CAP_ACCESSIBILITY|UMI_NATIVE_WEB_CAP_THEME|UMI_NATIVE_WEB_CAP_DENSITY;return d;}

