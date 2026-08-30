/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/workbench_renderer.c
 *
 * PURPOSE:
 *   Render the canonical Umicom workstation root, rails, document area and dock regions to web markup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/workbench_renderer.h"

UmiStatus umi_native_web_workbench_render(const UmiNativeWebRenderContext *context,const UmiNativeWebSurface *surface,UmiNativeWebRenderResult *result){if(umi_native_web_render_context_validate(context)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;return umi_native_web_render_surface_wrapper(surface,"umicom-workbench","application",result);}
UmiNativeWebRendererDescriptor umi_native_web_workbench_renderer_descriptor(void){UmiNativeWebRendererDescriptor d={0};(void)umi_native_web_copy_text(d.renderer_id,sizeof(d.renderer_id),"native-web.workbench");(void)umi_native_web_copy_text(d.semantic_contract,sizeof(d.semantic_contract),"Umicom.Ui.Workbench");d.render=umi_native_web_workbench_render;d.capability_flags=UMI_NATIVE_WEB_CAP_DOM_PATCH|UMI_NATIVE_WEB_CAP_ACCESSIBILITY|UMI_NATIVE_WEB_CAP_THEME|UMI_NATIVE_WEB_CAP_DENSITY;return d;}

