/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/command_set.c
 *
 * PURPOSE:
 *   Maintain a bounded editor command enablement table.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/command_set.h"

#include <string.h>
/*
 * Initialise editor wb command set from caller-provided values so later operations receive
 * a known state.
 */
void umi_editor_wb_command_set_init(UmiEditorWbCommandSet *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=NULL)memset(s,0,sizeof *s);}
/*
 * Provide the editor wb command set contains operation used by this module and its client
 * applications.
 */
int umi_editor_wb_command_set_contains(const UmiEditorWbCommandSet *s,const char *id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||id==NULL)return 0;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<s->count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(s->ids[i],id)==0)return 1;return 0;}
/*
 * Add editor wb command set only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_editor_wb_command_set_add(UmiEditorWbCommandSet *s,const char *id){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||!umi_editor_wb_id_valid(id))return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_wb_command_set_contains(s,id))return UMI_STATUS_ALREADY_EXISTS;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->count>=UMI_EDITOR_WB_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_wb_copy_text(s->ids[s->count],sizeof s->ids[0],id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;s->count++;s->revision++;return UMI_STATUS_OK;}
/*
 * Remove editor wb command set while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_editor_wb_command_set_remove(UmiEditorWbCommandSet *s,const char *id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<s->count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(s->ids[i],id)==0){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i+1U<s->count)memmove(&s->ids[i],&s->ids[i+1U],(s->count-i-1U)*sizeof s->ids[0]);s->count--;s->revision++;return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
