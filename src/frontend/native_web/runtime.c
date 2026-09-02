/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/runtime.c
 *
 * PURPOSE:
 *   Aggregate renderers, browser capabilities, sessions, event queues and patch state for one native-web frontend runtime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/runtime.h"
#include "umicom/frontend/native_web/workbench_renderer.h"
#include "umicom/frontend/native_web/application_shell_renderer.h"
#include "umicom/frontend/native_web/layout_renderer.h"
#include "umicom/frontend/native_web/panel_renderer.h"
#include "umicom/frontend/native_web/tabs_renderer.h"
#include "umicom/frontend/native_web/split_renderer.h"
#include "umicom/frontend/native_web/dock_renderer.h"
#include "umicom/frontend/native_web/toolbar_renderer.h"
#include "umicom/frontend/native_web/status_renderer.h"
#include "umicom/frontend/native_web/command_bar_renderer.h"
#include "umicom/frontend/native_web/workspace_strip_renderer.h"
#include "umicom/frontend/native_web/activity_bar_renderer.h"
#include "umicom/frontend/native_web/explorer_renderer.h"
#include "umicom/frontend/native_web/editor_renderer.h"
#include "umicom/frontend/native_web/data_grid_renderer.h"
#include "umicom/frontend/native_web/chart_renderer.h"
#include "umicom/frontend/native_web/timeline_renderer.h"
#include "umicom/frontend/native_web/node_graph_renderer.h"
#include "umicom/frontend/native_web/media_renderer.h"
#include "umicom/frontend/native_web/canvas_renderer.h"
#include "umicom/frontend/native_web/inspector_renderer.h"
#include "umicom/frontend/native_web/palette_renderer.h"

#include <string.h>
/* Provide the add defaults operation used by this module and its client applications. */
static UmiStatus add_defaults(UmiNativeWebRendererRegistry *r){UmiStatus s;
{ UmiNativeWebRendererDescriptor d=umi_native_web_workbench_renderer_descriptor(); s=umi_native_web_renderer_registry_add(r,&d); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s; }
{ UmiNativeWebRendererDescriptor d=umi_native_web_application_shell_renderer_descriptor(); s=umi_native_web_renderer_registry_add(r,&d); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s; }
{ UmiNativeWebRendererDescriptor d=umi_native_web_layout_renderer_descriptor(); s=umi_native_web_renderer_registry_add(r,&d); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s; }
{ UmiNativeWebRendererDescriptor d=umi_native_web_panel_renderer_descriptor(); s=umi_native_web_renderer_registry_add(r,&d); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s; }
{ UmiNativeWebRendererDescriptor d=umi_native_web_tabs_renderer_descriptor(); s=umi_native_web_renderer_registry_add(r,&d); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s; }
{ UmiNativeWebRendererDescriptor d=umi_native_web_split_renderer_descriptor(); s=umi_native_web_renderer_registry_add(r,&d); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s; }
{ UmiNativeWebRendererDescriptor d=umi_native_web_dock_renderer_descriptor(); s=umi_native_web_renderer_registry_add(r,&d); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s; }
{ UmiNativeWebRendererDescriptor d=umi_native_web_toolbar_renderer_descriptor(); s=umi_native_web_renderer_registry_add(r,&d); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s; }
{ UmiNativeWebRendererDescriptor d=umi_native_web_status_renderer_descriptor(); s=umi_native_web_renderer_registry_add(r,&d); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s; }
{ UmiNativeWebRendererDescriptor d=umi_native_web_command_bar_renderer_descriptor(); s=umi_native_web_renderer_registry_add(r,&d); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s; }
{ UmiNativeWebRendererDescriptor d=umi_native_web_workspace_strip_renderer_descriptor(); s=umi_native_web_renderer_registry_add(r,&d); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s; }
{ UmiNativeWebRendererDescriptor d=umi_native_web_activity_bar_renderer_descriptor(); s=umi_native_web_renderer_registry_add(r,&d); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s; }
{ UmiNativeWebRendererDescriptor d=umi_native_web_explorer_renderer_descriptor(); s=umi_native_web_renderer_registry_add(r,&d); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s; }
{ UmiNativeWebRendererDescriptor d=umi_native_web_editor_renderer_descriptor(); s=umi_native_web_renderer_registry_add(r,&d); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s; }
{ UmiNativeWebRendererDescriptor d=umi_native_web_data_grid_renderer_descriptor(); s=umi_native_web_renderer_registry_add(r,&d); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s; }
{ UmiNativeWebRendererDescriptor d=umi_native_web_chart_renderer_descriptor(); s=umi_native_web_renderer_registry_add(r,&d); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s; }
{ UmiNativeWebRendererDescriptor d=umi_native_web_timeline_renderer_descriptor(); s=umi_native_web_renderer_registry_add(r,&d); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s; }
{ UmiNativeWebRendererDescriptor d=umi_native_web_node_graph_renderer_descriptor(); s=umi_native_web_renderer_registry_add(r,&d); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s; }
{ UmiNativeWebRendererDescriptor d=umi_native_web_media_renderer_descriptor(); s=umi_native_web_renderer_registry_add(r,&d); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s; }
{ UmiNativeWebRendererDescriptor d=umi_native_web_canvas_renderer_descriptor(); s=umi_native_web_renderer_registry_add(r,&d); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s; }
{ UmiNativeWebRendererDescriptor d=umi_native_web_inspector_renderer_descriptor(); s=umi_native_web_renderer_registry_add(r,&d); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s; }
{ UmiNativeWebRendererDescriptor d=umi_native_web_palette_renderer_descriptor(); s=umi_native_web_renderer_registry_add(r,&d); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s; }
return UMI_STATUS_OK;}
/*
 * Initialise native web runtime from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_native_web_runtime_init(UmiNativeWebRuntime *runtime){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(runtime==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(runtime,0,sizeof(*runtime));umi_native_web_renderer_registry_init(&runtime->renderers);umi_native_web_browser_capability_init(&runtime->browser);umi_native_web_event_queue_init(&runtime->events);umi_native_web_patch_batch_init(&runtime->patches,1U);s=add_defaults(&runtime->renderers);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;runtime->revision=1U;runtime->started=true;return UMI_STATUS_OK;}
/*
 * Provide the native web runtime render operation used by this module and its client
 * applications.
 */
UmiStatus umi_native_web_runtime_render(UmiNativeWebRuntime *runtime,const UmiNativeWebRenderContext *context,const UmiNativeWebSurface *surface,UmiNativeWebRenderResult *result){const UmiNativeWebRendererDescriptor *d;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(runtime==NULL||!runtime->started||surface==NULL||result==NULL)return UMI_STATUS_INVALID_STATE;d=umi_native_web_renderer_registry_resolve(&runtime->renderers,surface->semantic_contract);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d==NULL)return UMI_STATUS_NOT_FOUND;umi_native_web_render_result_init(result,++runtime->revision);return d->render(context,surface,result);}

