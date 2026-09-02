/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_event/builders.c
 *
 * PURPOSE:
 *   Construct strongly typed normalised interaction events without exposing frontend-specific callback types.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_event/builders.h"

#include <stdio.h>
#include <string.h>

/* Provide the initialise operation used by this module and its client applications. */
static UmiStatus initialise(
    UmiWorkbenchContextEvent *event,
    UmiWorkbenchContextEventKind kind,
    UmiContextKind context_kind,
    const char *event_id,
    const char *source_id,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id,
    uint64_t timestamp_ms)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (event == NULL || event_id == NULL || source_id == NULL ||
        application_id == NULL || panel_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_workbench_context_event_init(event, kind, event_id);
    event->context_kind = context_kind;
    event->timestamp_ms = timestamp_ms;
    status = umi_workbench_context_event_copy_text(
        event->source_id,sizeof(event->source_id),source_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_context_event_copy_text(
        event->application_id,sizeof(event->application_id),application_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_context_event_copy_text(
        event->panel_id,sizeof(event->panel_id),panel_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace_id != NULL) {
        status = umi_workbench_context_event_copy_text(
            event->workspace_id,sizeof(event->workspace_id),workspace_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

/* Provide the meta operation used by this module and its client applications. */
static UmiStatus meta(
    UmiWorkbenchContextEvent *event,
    const char *name,
    const char *value)
{
    return umi_workbench_context_event_add_metadata(
        event,name,value != NULL ? value : "");
}

/*
 * Provide the workbench context event build project operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_event_build_project(
    UmiWorkbenchContextEvent *event,const char *event_id,const char *source_id,
    const char *application_id,const char *panel_id,const char *workspace_id,
    const char *project_id,const char *root_path,const char *repository_id,
    const char *language_id,uint64_t timestamp_ms)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(project_id==NULL||root_path==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    status=initialise(event,UMI_WORKBENCH_CONTEXT_EVENT_PROJECT_SELECTION,
        UMI_CONTEXT_KIND_PROJECT,event_id,source_id,application_id,panel_id,
        workspace_id,timestamp_ms);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status!=UMI_STATUS_OK)return status;
    status=umi_workbench_context_event_copy_text(event->subject_id,sizeof(event->subject_id),project_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status!=UMI_STATUS_OK)return status;
    status=umi_workbench_context_event_copy_text(event->path,sizeof(event->path),root_path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status!=UMI_STATUS_OK)return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if((status=meta(event,"repository-id",repository_id))!=UMI_STATUS_OK)return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if((status=meta(event,"language-id",language_id))!=UMI_STATUS_OK)return status;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench context event build editor location operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_event_build_editor_location(
    UmiWorkbenchContextEvent *event,const char *event_id,const char *source_id,
    const char *application_id,const char *panel_id,const char *workspace_id,
    const char *file_path,const char *symbol,uint32_t line,uint32_t column,
    uint32_t selection_length,uint64_t timestamp_ms)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(file_path==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    status=initialise(event,selection_length>0U?UMI_WORKBENCH_CONTEXT_EVENT_EDITOR_SELECTION:
        UMI_WORKBENCH_CONTEXT_EVENT_EDITOR_CARET,UMI_CONTEXT_KIND_SOURCE_LOCATION,
        event_id,source_id,application_id,panel_id,workspace_id,timestamp_ms);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status!=UMI_STATUS_OK)return status;
    status=umi_workbench_context_event_copy_text(event->path,sizeof(event->path),file_path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status!=UMI_STATUS_OK)return status;
    event->line=line;event->column=column;event->selection_length=selection_length;
    return meta(event,"symbol",symbol);
}

/*
 * Provide the workbench context event build diagnostic operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_event_build_diagnostic(
    UmiWorkbenchContextEvent *event,const char *event_id,const char *source_id,
    const char *application_id,const char *panel_id,const char *workspace_id,
    const char *file_path,uint32_t line,uint32_t column,const char *severity,
    const char *diagnostic_code,const char *message,uint64_t timestamp_ms)
{
    UmiStatus status=initialise(event,UMI_WORKBENCH_CONTEXT_EVENT_DIAGNOSTIC_SELECTION,
        UMI_CONTEXT_KIND_SOURCE_LOCATION,event_id,source_id,application_id,panel_id,
        workspace_id,timestamp_ms);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status!=UMI_STATUS_OK)return status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(file_path==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    status=umi_workbench_context_event_copy_text(event->path,sizeof(event->path),file_path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status!=UMI_STATUS_OK)return status;
    event->line=line;event->column=column;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if((status=meta(event,"severity",severity))!=UMI_STATUS_OK)return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if((status=meta(event,"diagnostic-code",diagnostic_code))!=UMI_STATUS_OK)return status;
    return meta(event,"message",message);
}

/*
 * Provide the workbench context event build source control operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_event_build_source_control(
    UmiWorkbenchContextEvent *event,const char *event_id,const char *source_id,
    const char *application_id,const char *panel_id,const char *workspace_id,
    const char *project_id,const char *root_path,const char *repository_id,
    const char *branch,const char *path,const char *change_kind,uint64_t timestamp_ms)
{
    UmiStatus status=initialise(event,UMI_WORKBENCH_CONTEXT_EVENT_SOURCE_CONTROL_SELECTION,
        UMI_CONTEXT_KIND_PROJECT,event_id,source_id,application_id,panel_id,
        workspace_id,timestamp_ms);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status!=UMI_STATUS_OK)return status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(project_id==NULL||root_path==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    status=umi_workbench_context_event_copy_text(event->subject_id,sizeof(event->subject_id),project_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status!=UMI_STATUS_OK)return status;
    status=umi_workbench_context_event_copy_text(event->path,sizeof(event->path),root_path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status!=UMI_STATUS_OK)return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if((status=meta(event,"repository-id",repository_id))!=UMI_STATUS_OK)return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if((status=meta(event,"branch",branch))!=UMI_STATUS_OK)return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if((status=meta(event,"path",path))!=UMI_STATUS_OK)return status;
    return meta(event,"change-kind",change_kind);
}

/*
 * Provide the workbench context event build test operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_event_build_test(
    UmiWorkbenchContextEvent *event,const char *event_id,const char *source_id,
    const char *application_id,const char *panel_id,const char *workspace_id,
    const char *test_id,const char *suite_id,const char *outcome,
    const char *source_uri,uint64_t source_line,uint64_t duration_ms,uint64_t timestamp_ms)
{
    UmiStatus status=initialise(event,UMI_WORKBENCH_CONTEXT_EVENT_TEST_SELECTION,
        UMI_CONTEXT_KIND_SELECTION,event_id,source_id,application_id,panel_id,
        workspace_id,timestamp_ms);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status!=UMI_STATUS_OK)return status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(test_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    status=umi_workbench_context_event_copy_text(event->subject_id,sizeof(event->subject_id),test_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status!=UMI_STATUS_OK)return status;
    event->unsigned_value=source_line;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if((status=meta(event,"suite-id",suite_id))!=UMI_STATUS_OK)return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if((status=meta(event,"outcome",outcome))!=UMI_STATUS_OK)return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if((status=meta(event,"source-uri",source_uri))!=UMI_STATUS_OK)return status;
    {
        char duration[64U];
        int written=snprintf(duration,sizeof(duration),"%llu",(unsigned long long)duration_ms);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if(written<0||(size_t)written>=sizeof(duration))return UMI_STATUS_CAPACITY_EXCEEDED;
        return meta(event,"duration-ms",duration);
    }
}

/*
 * Provide the workbench context event build ai operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_event_build_ai(
    UmiWorkbenchContextEvent *event,const char *event_id,const char *source_id,
    const char *application_id,const char *panel_id,const char *workspace_id,
    const char *message_id,const char *conversation_id,const char *provider_id,
    const char *model_id,const char *evidence_id,uint64_t timestamp_ms)
{
    UmiStatus status=initialise(event,UMI_WORKBENCH_CONTEXT_EVENT_AI_SELECTION,
        UMI_CONTEXT_KIND_SELECTION,event_id,source_id,application_id,panel_id,
        workspace_id,timestamp_ms);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status!=UMI_STATUS_OK)return status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(message_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    status=umi_workbench_context_event_copy_text(event->subject_id,sizeof(event->subject_id),message_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status!=UMI_STATUS_OK)return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if((status=meta(event,"conversation-id",conversation_id))!=UMI_STATUS_OK)return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if((status=meta(event,"provider-id",provider_id))!=UMI_STATUS_OK)return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if((status=meta(event,"model-id",model_id))!=UMI_STATUS_OK)return status;
    return meta(event,"evidence-id",evidence_id);
}

/*
 * Provide the workbench context event build instrument operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_event_build_instrument(
    UmiWorkbenchContextEvent *event,const char *event_id,const char *source_id,
    const char *application_id,const char *panel_id,const char *instrument_id,
    const char *symbol,const char *venue,const char *currency,const char *asset_class,
    const char *contract_id,uint64_t timestamp_ms)
{
    UmiStatus status=initialise(event,UMI_WORKBENCH_CONTEXT_EVENT_INSTRUMENT_SELECTION,
        UMI_CONTEXT_KIND_INSTRUMENT,event_id,source_id,application_id,panel_id,NULL,timestamp_ms);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status!=UMI_STATUS_OK)return status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(instrument_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    status=umi_workbench_context_event_copy_text(event->subject_id,sizeof(event->subject_id),instrument_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status!=UMI_STATUS_OK)return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if((status=meta(event,"symbol",symbol))!=UMI_STATUS_OK)return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if((status=meta(event,"venue",venue))!=UMI_STATUS_OK)return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if((status=meta(event,"currency",currency))!=UMI_STATUS_OK)return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if((status=meta(event,"asset-class",asset_class))!=UMI_STATUS_OK)return status;
    return meta(event,"contract-id",contract_id);
}

/*
 * Provide the workbench context event build account operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_event_build_account(
    UmiWorkbenchContextEvent *event,const char *event_id,const char *source_id,
    const char *application_id,const char *panel_id,const char *account_id,
    const char *organisation_id,const char *book_id,const char *currency,
    const char *account_type,const char *environment,uint64_t timestamp_ms)
{
    UmiStatus status=initialise(event,UMI_WORKBENCH_CONTEXT_EVENT_ACCOUNT_SELECTION,
        UMI_CONTEXT_KIND_ACCOUNT,event_id,source_id,application_id,panel_id,NULL,timestamp_ms);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status!=UMI_STATUS_OK)return status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(account_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    status=umi_workbench_context_event_copy_text(event->subject_id,sizeof(event->subject_id),account_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status!=UMI_STATUS_OK)return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if((status=meta(event,"organisation-id",organisation_id))!=UMI_STATUS_OK)return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if((status=meta(event,"book-id",book_id))!=UMI_STATUS_OK)return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if((status=meta(event,"currency",currency))!=UMI_STATUS_OK)return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if((status=meta(event,"account-type",account_type))!=UMI_STATUS_OK)return status;
    return meta(event,"environment",environment);
}

/*
 * Provide the workbench context event build trade operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_event_build_trade(
    UmiWorkbenchContextEvent *event,const char *event_id,const char *source_id,
    const char *application_id,const char *panel_id,const char *trade_id,
    const char *source_system,const char *product_type,const char *book_id,
    const char *counterparty_id,uint64_t version,uint64_t timestamp_ms)
{
    UmiStatus status=initialise(event,UMI_WORKBENCH_CONTEXT_EVENT_TRADE_SELECTION,
        UMI_CONTEXT_KIND_TRADE,event_id,source_id,application_id,panel_id,NULL,timestamp_ms);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status!=UMI_STATUS_OK)return status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(trade_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    status=umi_workbench_context_event_copy_text(event->subject_id,sizeof(event->subject_id),trade_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status!=UMI_STATUS_OK)return status;
    event->unsigned_value=version;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if((status=meta(event,"source-system",source_system))!=UMI_STATUS_OK)return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if((status=meta(event,"product-type",product_type))!=UMI_STATUS_OK)return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if((status=meta(event,"book-id",book_id))!=UMI_STATUS_OK)return status;
    return meta(event,"counterparty-id",counterparty_id);
}
