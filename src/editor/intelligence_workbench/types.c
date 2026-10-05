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
#include "../../base/value_archive_internal.h"

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
int umi_editor_intel_entry_valid(const UmiEditorIntelEntry *entry){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (entry == NULL) return 0;
    if (memchr(entry->id, '\0', sizeof(entry->id)) == NULL) return 0;
    if (memchr(entry->label, '\0', sizeof(entry->label)) == NULL) return 0;
    if (memchr(entry->detail, '\0', sizeof(entry->detail)) == NULL) return 0;
    if (memchr(entry->location.path, '\0', sizeof(entry->location.path)) == NULL) return 0;
return entry!=NULL&&umi_editor_intel_id_valid(entry->id)&&entry->label[0]!='\0'&&entry->location.path[0]!='\0'&&umi_editor_intel_position_compare(entry->location.range.start,entry->location.range.end)<=0;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiEditorIntelEntryArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x00a49308a37e0087);
    schema = (schema ^ (uint64_t)sizeof(((UmiEditorIntelEntry *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiEditorIntelEntry *)0)->label)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiEditorIntelEntry *)0)->detail)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiEditorIntelEntry *)0)->location.path)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiEditorIntelEntryArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiEditorIntelEntry *)0)->id) - 1U +
        8U + sizeof(((UmiEditorIntelEntry *)0)->label) - 1U +
        8U + sizeof(((UmiEditorIntelEntry *)0)->detail) - 1U +
        8U + sizeof(((UmiEditorIntelEntry *)0)->location.path) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiEditorIntelEntryArchiveWrite(UmiArchiveWriter *writer, const UmiEditorIntelEntry *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->label, sizeof(value->label));
    UmiArchiveWriteText(writer, value->detail, sizeof(value->detail));
    UmiArchiveWriteText(writer, value->location.path, sizeof(value->location.path));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->location.range.start.line);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->location.range.start.column);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->location.range.end.line);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->location.range.end.column);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->score);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->flags);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiEditorIntelEntryArchiveRead(UmiArchiveReader *reader, UmiEditorIntelEntry *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->label, sizeof(value->label));
    UmiArchiveReadText(reader, value->detail, sizeof(value->detail));
    UmiArchiveReadText(reader, value->location.path, sizeof(value->location.path));
    value->location.range.start.line = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->location.range.start.column = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->location.range.end.line = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->location.range.end.column = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->score = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->flags = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiEditorIntelEntryArchiveValidate(const UmiEditorIntelEntry *value)
{
    return umi_editor_intel_entry_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_editor_intel_entry_archive_encode, umi_editor_intel_entry_archive_decode,
    UmiEditorIntelEntry, UmiEditorIntelEntryArchiveSchema, UmiEditorIntelEntryArchiveBound, UmiEditorIntelEntryArchiveWrite, UmiEditorIntelEntryArchiveRead, UmiEditorIntelEntryArchiveValidate)
