/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/intelligence_workbench/types.c
 *
 * PURPOSE:
 *   Define stable search/refactoring identifiers, source locations, phases and shared value types.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical editor/language
 *   services; Studio remains a thin frontend and owns no reusable semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/intelligence_workbench/types.h"

#include <string.h>

/*
 * Provide the editor intel copy text operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_intel_copy_text(char *destination,size_t capacity,const char *source){size_t length;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(destination==NULL||capacity==0U||source==NULL)return UMI_STATUS_INVALID_ARGUMENT;length=strlen(source);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(length>=capacity)return UMI_STATUS_CAPACITY_EXCEEDED;memcpy(destination,source,length+1U);return UMI_STATUS_OK;}
/* Check that editor intel id satisfies its contract before another service relies on it. */
int umi_editor_intel_id_valid(const char *identifier){return identifier!=NULL&&identifier[0]!='\0'&&strlen(identifier)<UMI_EDITOR_INTEL_ID_CAPACITY;}
/*
 * Provide the editor intel position compare operation used by this module and its client
 * applications.
 */
int umi_editor_intel_position_compare(UmiEditorIntelPosition left,UmiEditorIntelPosition right){/* Apply this branch only when its contract condition is satisfied. */ if(left.line<right.line)return -1;/* Apply this branch only when its contract condition is satisfied. */ if(left.line>right.line)return 1;/* Apply this branch only when its contract condition is satisfied. */ if(left.column<right.column)return -1;/* Apply this branch only when its contract condition is satisfied. */ if(left.column>right.column)return 1;return 0;}
/*
 * Provide the editor intel range normalize operation used by this module and its client
 * applications.
 */
UmiEditorIntelRange umi_editor_intel_range_normalize(UmiEditorIntelRange range){/* Apply this branch only when its contract condition is satisfied. */ if(umi_editor_intel_position_compare(range.start,range.end)>0){UmiEditorIntelPosition temporary=range.start;range.start=range.end;range.end=temporary;}return range;}
/*
 * Initialise editor intel location from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_editor_intel_location_init(UmiEditorIntelLocation *location,const char *path,UmiEditorIntelRange range){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(location==NULL||path==NULL||path[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;memset(location,0,sizeof *location);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_intel_copy_text(location->path,sizeof location->path,path)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;location->range=umi_editor_intel_range_normalize(range);return UMI_STATUS_OK;}
/*
 * Initialise editor intel entry from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_editor_intel_entry_init(UmiEditorIntelEntry *entry,const char *id,const char *label,const char *path,UmiEditorIntelRange range){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(entry==NULL||!umi_editor_intel_id_valid(id)||label==NULL||label[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;memset(entry,0,sizeof *entry);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_intel_copy_text(entry->id,sizeof entry->id,id)!=UMI_STATUS_OK||umi_editor_intel_copy_text(entry->label,sizeof entry->label,label)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_intel_location_init(&entry->location,path,range)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;entry->revision=1U;return UMI_STATUS_OK;}
/*
 * Check that editor intel entry satisfies its contract before another service relies on
 * it.
 */
int umi_editor_intel_entry_valid(const UmiEditorIntelEntry *entry){return entry!=NULL&&umi_editor_intel_id_valid(entry->id)&&entry->label[0]!='\0'&&entry->location.path[0]!='\0'&&umi_editor_intel_position_compare(entry->location.range.start,entry->location.range.end)<=0;}
