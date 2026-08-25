/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/workspace_strip_renderer.c
 *
 * PURPOSE:
 *   Render named workspace/layout switching controls for desktop-like web applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/workspace_strip_renderer.h"

UmiStatus umi_native_web_workspace_strip_render(const UmiNativeWebRenderContext *context,const UmiNativeWebSurface *surface,UmiNativeWebRenderResult *result){if(umi_native_web_render_context_validate(context)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;return umi_native_web_render_surface_wrapper(surface,"umicom-workspace-strip","navigation",result);}
UmiNativeWebRendererDescriptor umi_native_web_workspace_strip_renderer_descriptor(void){UmiNativeWebRendererDescriptor d={0};(void)umi_native_web_copy_text(d.renderer_id,sizeof(d.renderer_id),"native-web.workspace-strip");(void)umi_native_web_copy_text(d.semantic_contract,sizeof(d.semantic_contract),"Umicom.Ui.WorkspaceStrip");d.render=umi_native_web_workspace_strip_render;d.capability_flags=UMI_NATIVE_WEB_CAP_DOM_PATCH|UMI_NATIVE_WEB_CAP_ACCESSIBILITY|UMI_NATIVE_WEB_CAP_THEME|UMI_NATIVE_WEB_CAP_DENSITY;return d;}

