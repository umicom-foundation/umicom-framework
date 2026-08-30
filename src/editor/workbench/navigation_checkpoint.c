/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/navigation_checkpoint.c
 *
 * PURPOSE:
 *   Capture a named source-location checkpoint and its navigation reason.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/navigation_checkpoint.h"

#include <string.h>
UmiStatus umi_editor_wb_navigation_checkpoint_init(UmiEditorWbNavigationCheckpoint *s,const char *id,const char *text){if(s==NULL||!umi_editor_wb_id_valid(id)||text==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof *s);if(umi_editor_wb_copy_text(s->id,sizeof s->id,id)!=UMI_STATUS_OK||umi_editor_wb_copy_text(s->text,sizeof s->text,text)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;return UMI_STATUS_OK;} UmiStatus umi_editor_wb_navigation_checkpoint_set_values(UmiEditorWbNavigationCheckpoint *s,uint64_t a,uint64_t b,bool e){if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;s->primary=a;s->secondary=b;s->enabled=e;return UMI_STATUS_OK;} int umi_editor_wb_navigation_checkpoint_valid(const UmiEditorWbNavigationCheckpoint *s){return s!=NULL&&umi_editor_wb_id_valid(s->id);}
