/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/code_map_segment.c
 *
 * PURPOSE:
 *   Describe one semantic segment of a code-map overview.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/code_map_segment.h"

#include <string.h>
UmiStatus umi_editor_wb_code_map_segment_init(UmiEditorWbCodeMapSegment *s,const char *id,uint32_t first,uint32_t last,UmiEditorWbSeverity sev){if(s==NULL||!umi_editor_wb_id_valid(id)||first==0U||last<first)return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof *s);(void)umi_editor_wb_copy_text(s->segment_id,sizeof s->segment_id,id);s->first_line=first;s->last_line=last;s->severity=sev;return UMI_STATUS_OK;}
int umi_editor_wb_code_map_segment_contains(const UmiEditorWbCodeMapSegment *s,uint32_t line){return s!=NULL&&line>=s->first_line&&line<=s->last_line;}
