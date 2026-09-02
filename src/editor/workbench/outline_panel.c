/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/outline_panel.c
 *
 * PURPOSE:
 *   Compose a flattened document-outline panel from symbol entries and depth.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/outline_panel.h"

#include <string.h>
/*
 * Initialise editor wb outline panel from caller-provided values so later operations
 * receive a known state.
 */
void umi_editor_wb_outline_panel_init(UmiEditorWbOutlinePanel *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=NULL)memset(s,0,sizeof *s);}
/*
 * Add editor wb outline panel only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_editor_wb_outline_panel_append(UmiEditorWbOutlinePanel *s,const char *text,uint32_t depth){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||text==NULL||text[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->count>=UMI_EDITOR_WB_MAX_SEGMENTS)return UMI_STATUS_CAPACITY_EXCEEDED;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_wb_copy_text(s->items[s->count],sizeof s->items[0],text)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;s->depth[s->count]=depth;s->count++;return UMI_STATUS_OK;}
/*
 * Provide the editor wb outline panel activate operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_wb_outline_panel_activate(UmiEditorWbOutlinePanel *s,size_t index){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||index>=s->count)return UMI_STATUS_INVALID_ARGUMENT;s->active_index=index;return UMI_STATUS_OK;}
