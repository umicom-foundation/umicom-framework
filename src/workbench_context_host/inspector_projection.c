/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/inspector_projection.c
 *
 * PURPOSE:
 *   Build readable inspector rows for canonical context identity and generic typed values.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/inspector_projection.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
/* Provide the add row operation used by this module and its client applications. */
static void add_row(UmiWorkbenchContextHostInspectorProjection *p,const char *name,const char *value)
{
    UmiWorkbenchContextHostInspectorRow *row;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(p->row_count>=UMI_WORKBENCH_CONTEXT_HOST_MAX_INSPECTOR_ROWS)return;
    row=&p->rows[p->row_count++];(void)umi_workbench_context_host_copy_text(row->name,sizeof(row->name),name);
    (void)umi_workbench_context_host_copy_text(row->value,sizeof(row->value),value?value:"");
}
/* Provide the add value operation used by this module and its client applications. */
static void add_value(UmiWorkbenchContextHostInspectorProjection *p,const UmiContextValue *v)
{
    char text[UMI_WORKBENCH_CONTEXT_HOST_TEXT_CAPACITY];
    /* Select the behaviour associated with the requested command or state value. */
    switch(v->kind){
    case UMI_CONTEXT_VALUE_TEXT:(void)snprintf(text,sizeof(text),"%s",v->text);break;
    case UMI_CONTEXT_VALUE_INTEGER:(void)snprintf(text,sizeof(text),"%" PRId64,v->integer_value);break;
    case UMI_CONTEXT_VALUE_UNSIGNED:(void)snprintf(text,sizeof(text),"%" PRIu64,v->unsigned_value);break;
    case UMI_CONTEXT_VALUE_DECIMAL:(void)snprintf(text,sizeof(text),"%.12g",v->decimal_value);break;
    case UMI_CONTEXT_VALUE_BOOLEAN:(void)snprintf(text,sizeof(text),"%s",v->boolean_value?"true":"false");break;
    default:(void)snprintf(text,sizeof(text),"(none)");break;
    }
    add_row(p,v->name,text);
}
/*
 * Provide the workbench context host inspector projection build operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_host_inspector_projection_build(
    const UmiWorkbenchContextHost *host,const char *group_id,
    UmiWorkbenchContextHostInspectorProjection *out_projection)
{
    const UmiWorkbenchContextLinkActiveSlot *active;size_t i;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!host||!group_id||!out_projection||!host->link_service)return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_projection,0,sizeof(*out_projection));
    active=umi_workbench_context_link_service_current(host->link_service,group_id);
    (void)umi_workbench_context_host_copy_text(out_projection->group_id,sizeof(out_projection->group_id),group_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!active){out_projection->revision=host->revision;return UMI_STATUS_OK;}
    out_projection->available=true;out_projection->kind=active->payload.kind;
    (void)umi_workbench_context_host_copy_text(out_projection->context_id,sizeof(out_projection->context_id),active->payload.identity.context_id);
    (void)umi_workbench_context_host_copy_text(out_projection->schema_id,sizeof(out_projection->schema_id),active->payload.identity.schema_id);
    (void)umi_workbench_context_host_copy_text(out_projection->source_application_id,sizeof(out_projection->source_application_id),active->payload.identity.source_application_id);
    (void)umi_workbench_context_host_copy_text(out_projection->source_panel_id,sizeof(out_projection->source_panel_id),active->payload.identity.source_panel_id);
    add_row(out_projection,"kind",umi_context_kind_text(active->payload.kind));
    add_row(out_projection,"context-id",active->payload.identity.context_id);
    add_row(out_projection,"schema",active->payload.identity.schema_id);
    add_row(out_projection,"source-application",active->payload.identity.source_application_id);
    add_row(out_projection,"source-panel",active->payload.identity.source_panel_id);
    /* Visit each bounded item once so every record receives the same rule. */
    for(i=0U;i<active->payload.value_count&&out_projection->row_count<UMI_WORKBENCH_CONTEXT_HOST_MAX_INSPECTOR_ROWS;++i)
        add_value(out_projection,&active->payload.values[i]);
    out_projection->revision=host->revision;return UMI_STATUS_OK;
}
