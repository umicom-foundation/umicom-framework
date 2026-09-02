/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/synchronized_editors.c
 *
 * PURPOSE:
 *   Maintain a bounded set of editor surfaces participating in synchronization.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/synchronized_editors.h"

#include <string.h>
/*
 * Initialise editor wb synchronized editors from caller-provided values so later
 * operations receive a known state.
 */
void umi_editor_wb_synchronized_editors_init(UmiEditorWbSynchronizedEditors *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=NULL)memset(s,0,sizeof *s);}
/*
 * Add editor wb synchronized editors only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_editor_wb_synchronized_editors_append(UmiEditorWbSynchronizedEditors *s,const char *text,uint32_t depth){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||text==NULL||text[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->count>=UMI_EDITOR_WB_MAX_SEGMENTS)return UMI_STATUS_CAPACITY_EXCEEDED;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_wb_copy_text(s->items[s->count],sizeof s->items[0],text)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;s->depth[s->count]=depth;s->count++;return UMI_STATUS_OK;}
/*
 * Provide the editor wb synchronized editors activate operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_wb_synchronized_editors_activate(UmiEditorWbSynchronizedEditors *s,size_t index){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||index>=s->count)return UMI_STATUS_INVALID_ARGUMENT;s->active_index=index;return UMI_STATUS_OK;}
