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
/*
 * Initialise editor wb editor search session from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_wb_editor_search_session_init(UmiEditorWbEditorSearchSession *s,const char *q){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||q==NULL||q[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof *s);return umi_editor_wb_copy_text(s->query,sizeof s->query,q);}
/*
 * Provide the editor wb editor search session set matches operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_wb_editor_search_session_set_matches(UmiEditorWbEditorSearchSession *s,size_t count){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;s->match_count=count;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(count==0U)s->active_index=0U;else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->active_index>=count)s->active_index=count-1U;return UMI_STATUS_OK;}
/*
 * Provide the editor wb editor search session next operation used by this module and its
 * client applications.
 */
size_t umi_editor_wb_editor_search_session_next(UmiEditorWbEditorSearchSession *s,int direction){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||s->match_count==0U)return 0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(direction>=0)s->active_index=(s->active_index+1U)%s->match_count;/* Use this fallback path when the earlier condition does not apply. */ else s->active_index=s->active_index==0U?s->match_count-1U:s->active_index-1U;return s->active_index;}
