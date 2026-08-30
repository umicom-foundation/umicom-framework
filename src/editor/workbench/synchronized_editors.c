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
void umi_editor_wb_synchronized_editors_init(UmiEditorWbSynchronizedEditors *s){if(s!=NULL)memset(s,0,sizeof *s);}
UmiStatus umi_editor_wb_synchronized_editors_append(UmiEditorWbSynchronizedEditors *s,const char *text,uint32_t depth){if(s==NULL||text==NULL||text[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;if(s->count>=UMI_EDITOR_WB_MAX_SEGMENTS)return UMI_STATUS_CAPACITY_EXCEEDED;if(umi_editor_wb_copy_text(s->items[s->count],sizeof s->items[0],text)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;s->depth[s->count]=depth;s->count++;return UMI_STATUS_OK;}
UmiStatus umi_editor_wb_synchronized_editors_activate(UmiEditorWbSynchronizedEditors *s,size_t index){if(s==NULL||index>=s->count)return UMI_STATUS_INVALID_ARGUMENT;s->active_index=index;return UMI_STATUS_OK;}
