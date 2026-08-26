/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/editor_layout_snapshot.c
 *
 * PURPOSE:
 *   Capture immutable editor-layout revision metadata for restore/history.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_layout_snapshot.h"

#include <string.h>
static uint64_t htext_editor_layout_snapshot(const char *p){uint64_t h=1469598103934665603ULL;while(*p!='\0'){h^=(uint64_t)(unsigned char)*p++;h*=1099511628211ULL;}return h;}
UmiStatus umi_editor_wb_editor_layout_snapshot_capture(UmiEditorWbEditorLayoutSnapshot *s,const char *active,size_t items,size_t groups,uint64_t rev){if(s==NULL||active==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof *s);if(umi_editor_wb_copy_text(s->active_id,sizeof s->active_id,active)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;s->item_count=items;s->group_count=groups;s->revision=rev;s->fingerprint=htext_editor_layout_snapshot(active)^(uint64_t)items^((uint64_t)groups<<32U)^rev;return UMI_STATUS_OK;} int umi_editor_wb_editor_layout_snapshot_valid(const UmiEditorWbEditorLayoutSnapshot *s){return s!=NULL&&s->revision>0U&&s->group_count<=UMI_EDITOR_WB_MAX_GROUPS;}
