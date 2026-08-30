/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/editor_search_session.c
 *
 * PURPOSE:
 *   Track in-editor search query, options and active-result state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_search_session.h"

#include <string.h>
UmiStatus umi_editor_wb_editor_search_session_init(UmiEditorWbEditorSearchSession *s,const char *q){if(s==NULL||q==NULL||q[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof *s);return umi_editor_wb_copy_text(s->query,sizeof s->query,q);}
UmiStatus umi_editor_wb_editor_search_session_set_matches(UmiEditorWbEditorSearchSession *s,size_t count){if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;s->match_count=count;if(count==0U)s->active_index=0U;else if(s->active_index>=count)s->active_index=count-1U;return UMI_STATUS_OK;}
size_t umi_editor_wb_editor_search_session_next(UmiEditorWbEditorSearchSession *s,int direction){if(s==NULL||s->match_count==0U)return 0U;if(direction>=0)s->active_index=(s->active_index+1U)%s->match_count;else s->active_index=s->active_index==0U?s->match_count-1U:s->active_index-1U;return s->active_index;}
