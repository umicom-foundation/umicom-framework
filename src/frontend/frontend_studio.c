/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/frontend_studio.c
 *
 * PURPOSE:
 *   Implement the frontend studio behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework - Web and Frontend Studio orchestration implementation
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/frontend_studio.h"
#include <string.h>
/* Provide the add default build operation used by this module and its client applications. */
static UmiStatus add_default_build(UmiFrontendBuildProfiles *profiles){UmiFrontendBuildProfile p={0};UmiStatus s=umi_frontend_dev_copy_text(p.id,sizeof(p.id),"development");/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s==UMI_STATUS_OK)s=umi_frontend_dev_copy_text(p.name,sizeof(p.name),"Development");/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s==UMI_STATUS_OK)s=umi_frontend_dev_copy_text(p.source_root,sizeof(p.source_root),"web");/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s==UMI_STATUS_OK)s=umi_frontend_dev_copy_text(p.entry_file,sizeof(p.entry_file),"web/index.html");/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s==UMI_STATUS_OK)s=umi_frontend_dev_copy_text(p.output_directory,sizeof(p.output_directory),"build/web");/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s==UMI_STATUS_OK)s=umi_frontend_dev_copy_text(p.browser_targets,sizeof(p.browser_targets),"defaults");p.source_maps=1;p.strict=1;return s==UMI_STATUS_OK?umi_frontend_build_profiles_add(profiles,&p):s;}
/*
 * Initialise frontend studio from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_frontend_studio_init(UmiFrontendStudio *s,const UmiFrontendDevServerConfig *config){UmiFrontendDevicePreset d;UmiFrontendDevServerConfig c;UmiStatus st;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(s,0,sizeof(*s));c=config!=NULL?*config:umi_frontend_dev_server_config_default();st=umi_frontend_device_registry_init(&s->devices);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st==UMI_STATUS_OK)st=umi_frontend_device_registry_add_defaults(&s->devices);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st==UMI_STATUS_OK)st=umi_frontend_document_store_init(&s->documents);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st==UMI_STATUS_OK)st=umi_frontend_dev_server_init(&s->server,&c);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st==UMI_STATUS_OK)st=umi_frontend_device_registry_find(&s->devices,"desktop-1440",&d);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st==UMI_STATUS_OK)st=umi_frontend_preview_session_init(&s->preview,"primary",s->server.base_url,&d);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st==UMI_STATUS_OK)st=umi_frontend_dom_inspector_init(&s->dom);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st==UMI_STATUS_OK)st=umi_frontend_network_log_init(&s->network);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st==UMI_STATUS_OK)st=umi_frontend_console_log_init(&s->console);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st==UMI_STATUS_OK)st=umi_frontend_build_profiles_init(&s->builds);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st==UMI_STATUS_OK)st=add_default_build(&s->builds);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st==UMI_STATUS_OK)st=umi_frontend_hot_reload_init(&s->reload,1);s->budget=umi_frontend_quality_budget_default();s->revision=1U;return st;}
/*
 * Provide the frontend studio snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_frontend_studio_snapshot(const UmiFrontendStudio *s,UmiFrontendStudioSnapshot *out){UmiStatus st;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(out,0,sizeof(*out));out->device_count=s->devices.count;out->document_count=s->documents.count;out->dom_node_count=s->dom.count;out->network_entry_count=s->network.count;out->console_entry_count=s->console.count;out->build_profile_count=s->builds.count;out->pending_changes=s->reload.pending_count;out->revision=s->revision;out->server_running=s->server.running;st=umi_frontend_dev_copy_text(out->preview_url,sizeof(out->preview_url),s->preview.url);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st==UMI_STATUS_OK)st=umi_frontend_dev_copy_text(out->active_device,sizeof(out->active_device),s->preview.device.id);return st;}
/*
 * Provide the frontend studio open document operation used by this module and its client
 * applications.
 */
UmiStatus umi_frontend_studio_open_document(UmiFrontendStudio *s,const UmiFrontendDocument *d){UmiStatus st;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||d==NULL)return UMI_STATUS_INVALID_ARGUMENT;st=umi_frontend_document_store_upsert(&s->documents,d);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st==UMI_STATUS_OK)st=umi_frontend_dev_copy_text(s->preview.active_document,sizeof(s->preview.active_document),d->path);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st==UMI_STATUS_OK)st=umi_frontend_hot_reload_queue(&s->reload,d->path,s->documents.revision);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st==UMI_STATUS_OK){s->preview.revision+=1U;s->revision+=1U;}return st;}
/*
 * Provide the frontend studio publish changes operation used by this module and its client
 * applications.
 */
UmiStatus umi_frontend_studio_publish_changes(UmiFrontendStudio *s,uint64_t *out){UmiStatus st;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;st=umi_frontend_hot_reload_complete(&s->reload,out);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st==UMI_STATUS_OK){st=umi_frontend_preview_session_reload(&s->preview);s->revision+=1U;}return st;}
