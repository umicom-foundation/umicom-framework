/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/toolbar_projection.c
 *
 * PURPOSE:
 *   Build navigation and pinning actions from current host and history state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/toolbar_projection.h"
#include <string.h>
static void add_action(UmiWorkbenchContextHostToolbarProjection *p,const char *id,const char *label,
                       const char *tip,UmiWorkbenchContextHostCommandKind kind,bool enabled)
{
    UmiWorkbenchContextHostToolbarAction *a;if(p->count>=UMI_WORKBENCH_CONTEXT_HOST_MAX_TOOLBAR_ACTIONS)return;
    a=&p->actions[p->count++];(void)umi_workbench_context_host_copy_text(a->action_id,sizeof(a->action_id),id);
    (void)umi_workbench_context_host_copy_text(a->label,sizeof(a->label),label);
    (void)umi_workbench_context_host_copy_text(a->tooltip,sizeof(a->tooltip),tip);a->command_kind=kind;a->enabled=enabled;
}
UmiStatus umi_workbench_context_host_toolbar_projection_build(
    const UmiWorkbenchContextHost *host,UmiWorkbenchContextHostToolbarProjection *out_projection)
{
    bool has_group,has_context,has_history;if(!host||!out_projection||!host->link_service)return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_projection,0,sizeof(*out_projection));has_group=host->active_group_id[0]!='\0';
    has_context=has_group&&umi_workbench_context_link_service_current(host->link_service,host->active_group_id)!=NULL;
    has_history=host->link_service->history.count>0U;
    add_action(out_projection,"context.back","Back","Previous context in active group",UMI_WORKBENCH_CONTEXT_HOST_COMMAND_BACK,has_history&&!host->suspended);
    add_action(out_projection,"context.forward","Forward","Next context in active group",UMI_WORKBENCH_CONTEXT_HOST_COMMAND_FORWARD,has_history&&!host->suspended);
    add_action(out_projection,"context.pin","Pin","Pin the active context",UMI_WORKBENCH_CONTEXT_HOST_COMMAND_PIN,has_context&&!host->suspended);
    add_action(out_projection,host->suspended?"context.resume":"context.suspend",host->suspended?"Resume":"Suspend",
        host->suspended?"Resume context routing":"Suspend context routing",
        host->suspended?UMI_WORKBENCH_CONTEXT_HOST_COMMAND_RESUME:UMI_WORKBENCH_CONTEXT_HOST_COMMAND_SUSPEND,true);
    out_projection->revision=host->revision;return UMI_STATUS_OK;
}
