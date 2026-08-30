/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/bookmark_panel.c
 *
 * PURPOSE:
 *   Maintain a reusable bookmark-panel model over stable source locations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/bookmark_panel.h"

#include <string.h>
void umi_editor_wb_bookmark_panel_init(UmiEditorWbBookmarkPanel *s){if(s!=NULL)memset(s,0,sizeof *s);}
int umi_editor_wb_bookmark_panel_contains(const UmiEditorWbBookmarkPanel *s,const char *id){size_t i;if(s==NULL||id==NULL)return 0;for(i=0U;i<s->count;i++)if(strcmp(s->ids[i],id)==0)return 1;return 0;}
UmiStatus umi_editor_wb_bookmark_panel_add(UmiEditorWbBookmarkPanel *s,const char *id){if(s==NULL||!umi_editor_wb_id_valid(id))return UMI_STATUS_INVALID_ARGUMENT;if(umi_editor_wb_bookmark_panel_contains(s,id))return UMI_STATUS_ALREADY_EXISTS;if(s->count>=UMI_EDITOR_WB_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;if(umi_editor_wb_copy_text(s->ids[s->count],sizeof s->ids[0],id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;s->count++;s->revision++;return UMI_STATUS_OK;}
UmiStatus umi_editor_wb_bookmark_panel_remove(UmiEditorWbBookmarkPanel *s,const char *id){size_t i;if(s==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<s->count;i++)if(strcmp(s->ids[i],id)==0){if(i+1U<s->count)memmove(&s->ids[i],&s->ids[i+1U],(s->count-i-1U)*sizeof s->ids[0]);s->count--;s->revision++;return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
