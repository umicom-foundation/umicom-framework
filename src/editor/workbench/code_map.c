/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/code_map.c
 *
 * PURPOSE:
 *   Maintain semantic line-range segments used by a code-map overview.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/code_map.h"

#include <string.h>
void umi_editor_wb_code_map_init(UmiEditorWbCodeMap *m){if(m!=NULL)memset(m,0,sizeof *m);}
UmiStatus umi_editor_wb_code_map_add(UmiEditorWbCodeMap *m,const UmiEditorWbCodeMapSegment *s){if(m==NULL||s==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(m->count>=UMI_EDITOR_WB_MAX_SEGMENTS)return UMI_STATUS_CAPACITY_EXCEEDED;m->segments[m->count++]=*s;return UMI_STATUS_OK;}
const UmiEditorWbCodeMapSegment *umi_editor_wb_code_map_find_line(const UmiEditorWbCodeMap *m,uint32_t line){size_t i;if(m==NULL)return NULL;for(i=0U;i<m->count;i++)if(umi_editor_wb_code_map_segment_contains(&m->segments[i],line))return &m->segments[i];return NULL;}
