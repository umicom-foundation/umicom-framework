/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/explorer_renderer.c
 *
 * PURPOSE:
 *   Render hierarchical explorer/project/navigation data with accessible tree semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/explorer_renderer.h"

UmiStatus umi_native_web_explorer_render(const UmiNativeWebRenderContext *context,const UmiNativeWebSurface *surface,UmiNativeWebRenderResult *result){if(umi_native_web_render_context_validate(context)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;return umi_native_web_render_surface_wrapper(surface,"umicom-explorer","tree",result);}
UmiNativeWebRendererDescriptor umi_native_web_explorer_renderer_descriptor(void){UmiNativeWebRendererDescriptor d={0};(void)umi_native_web_copy_text(d.renderer_id,sizeof(d.renderer_id),"native-web.explorer");(void)umi_native_web_copy_text(d.semantic_contract,sizeof(d.semantic_contract),"Umicom.Ui.Explorer");d.render=umi_native_web_explorer_render;d.capability_flags=UMI_NATIVE_WEB_CAP_DOM_PATCH|UMI_NATIVE_WEB_CAP_ACCESSIBILITY|UMI_NATIVE_WEB_CAP_THEME|UMI_NATIVE_WEB_CAP_DENSITY;return d;}

