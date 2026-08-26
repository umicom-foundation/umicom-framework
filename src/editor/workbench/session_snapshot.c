/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/session_snapshot.c
 *
 * PURPOSE:
 *   Capture editor-session counts, active identity and revision metadata.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/session_snapshot.h"

#include <string.h>
static uint64_t htext_session_snapshot(const char *p){uint64_t h=1469598103934665603ULL;while(*p!='\0'){h^=(uint64_t)(unsigned char)*p++;h*=1099511628211ULL;}return h;}
UmiStatus umi_editor_wb_session_snapshot_capture(UmiEditorWbSessionSnapshot *s,const char *active,size_t items,size_t groups,uint64_t rev){if(s==NULL||active==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof *s);if(umi_editor_wb_copy_text(s->active_id,sizeof s->active_id,active)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;s->item_count=items;s->group_count=groups;s->revision=rev;s->fingerprint=htext_session_snapshot(active)^(uint64_t)items^((uint64_t)groups<<32U)^rev;return UMI_STATUS_OK;} int umi_editor_wb_session_snapshot_valid(const UmiEditorWbSessionSnapshot *s){return s!=NULL&&s->revision>0U&&s->group_count<=UMI_EDITOR_WB_MAX_GROUPS;}
