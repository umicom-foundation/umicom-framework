/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/editor_split.c
 *
 * PURPOSE:
 *   Describe one horizontal or vertical editor split with a bounded ratio.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_split.h"

#include <string.h>
UmiStatus umi_editor_wb_editor_split_init(UmiEditorWbEditorSplit *s,const char *id,UmiEditorWbOrientation o,double ratio){if(s==NULL||!umi_editor_wb_id_valid(id))return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof *s);if(umi_editor_wb_copy_text(s->split_id,sizeof s->split_id,id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;s->orientation=o;return umi_editor_wb_editor_split_set_ratio(s,ratio);}
UmiStatus umi_editor_wb_editor_split_set_ratio(UmiEditorWbEditorSplit *s,double ratio){if(s==NULL||ratio<0.1||ratio>0.9)return UMI_STATUS_INVALID_ARGUMENT;s->ratio=ratio;return UMI_STATUS_OK;}
int umi_editor_wb_editor_split_valid(const UmiEditorWbEditorSplit *s){return s!=NULL&&umi_editor_wb_id_valid(s->split_id)&&(s->orientation==UMI_EDITOR_WB_HORIZONTAL||s->orientation==UMI_EDITOR_WB_VERTICAL)&&s->ratio>=0.1&&s->ratio<=0.9;}
