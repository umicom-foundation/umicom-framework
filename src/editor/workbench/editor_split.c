/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/editor_split.c
 *
 * PURPOSE:
 *   Describe one horizontal or vertical editor split with a bounded ratio.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_split.h"

#include <string.h>
/*
 * Initialise editor wb editor split from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_wb_editor_split_init(UmiEditorWbEditorSplit *s,const char *id,UmiEditorWbOrientation o,double ratio){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||!umi_editor_wb_id_valid(id))return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof *s);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_wb_copy_text(s->split_id,sizeof s->split_id,id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;s->orientation=o;return umi_editor_wb_editor_split_set_ratio(s,ratio);}
/*
 * Provide the editor wb editor split set ratio operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_wb_editor_split_set_ratio(UmiEditorWbEditorSplit *s,double ratio){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||ratio<0.1||ratio>0.9)return UMI_STATUS_INVALID_ARGUMENT;s->ratio=ratio;return UMI_STATUS_OK;}
/*
 * Check that editor wb editor split satisfies its contract before another service relies
 * on it.
 */
int umi_editor_wb_editor_split_valid(const UmiEditorWbEditorSplit *s){return s!=NULL&&umi_editor_wb_id_valid(s->split_id)&&(s->orientation==UMI_EDITOR_WB_HORIZONTAL||s->orientation==UMI_EDITOR_WB_VERTICAL)&&s->ratio>=0.1&&s->ratio<=0.9;}
