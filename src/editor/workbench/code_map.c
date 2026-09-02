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
/*
 * Initialise editor wb code map from caller-provided values so later operations receive a
 * known state.
 */
void umi_editor_wb_code_map_init(UmiEditorWbCodeMap *m){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m!=NULL)memset(m,0,sizeof *m);}
/* Add editor wb code map only after its inputs and available capacity have been checked. */
UmiStatus umi_editor_wb_code_map_add(UmiEditorWbCodeMap *m,const UmiEditorWbCodeMapSegment *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||s==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m->count>=UMI_EDITOR_WB_MAX_SEGMENTS)return UMI_STATUS_CAPACITY_EXCEEDED;m->segments[m->count++]=*s;return UMI_STATUS_OK;}
/*
 * Provide the editor wb code map find line operation used by this module and its client
 * applications.
 */
const UmiEditorWbCodeMapSegment *umi_editor_wb_code_map_find_line(const UmiEditorWbCodeMap *m,uint32_t line){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL)return NULL;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<m->count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_wb_code_map_segment_contains(&m->segments[i],line))return &m->segments[i];return NULL;}
