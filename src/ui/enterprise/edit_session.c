/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/edit_session.c
 *
 * PURPOSE:
 *   Implement bounded edit staging and cancellation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/edit_session.h"
#include <string.h>
UmiStatus umi_ui_ent_edit_session_begin(UmiUiEntEditSession *s,uint64_t key,const char *col,const UmiUiEntCellValue *v){if(!s||key==0U||!umi_ui_ent_id_valid(col)||!v)return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof *s);s->row_key=key;if(umi_ui_ent_copy_text(s->column_id,sizeof s->column_id,col)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;s->original_value=*v;s->pending_value=*v;s->active=1;s->revision=1U;return UMI_STATUS_OK;}
UmiStatus umi_ui_ent_edit_session_update(UmiUiEntEditSession *s,const UmiUiEntCellValue *v){if(!s||!v)return UMI_STATUS_INVALID_ARGUMENT;if(!s->active)return UMI_STATUS_INVALID_STATE;s->pending_value=*v;s->dirty=!umi_ui_ent_cell_value_equal(&s->original_value,v);s->revision++;return UMI_STATUS_OK;}
void umi_ui_ent_edit_session_cancel(UmiUiEntEditSession *s){if(s){s->pending_value=s->original_value;s->dirty=0;s->active=0;s->revision++;}}
