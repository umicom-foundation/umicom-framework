/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/editor_location.c
 *
 * PURPOSE:
 *   Represent a validated file, line and column editor location.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_location.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/*
 * Initialise editor wb editor location from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_wb_editor_location_init(UmiEditorWbEditorLocation *l,const char *path,uint32_t line,uint32_t col){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(l==NULL||path==NULL||path[0]=='\0'||line==0U||col==0U)return UMI_STATUS_INVALID_ARGUMENT;memset(l,0,sizeof *l);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_wb_copy_text(l->path,sizeof l->path,path)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;l->position.line=line;l->position.column=col;return UMI_STATUS_OK;}
/*
 * Check that editor wb editor location satisfies its contract before another service
 * relies on it.
 */
int umi_editor_wb_editor_location_valid(const UmiEditorWbEditorLocation *l){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (l == NULL) return 0;
    if (memchr(l->path, '\0', sizeof(l->path)) == NULL) return 0;
return l!=NULL&&l->path[0]!='\0'&&l->position.line>0U&&l->position.column>0U;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiEditorWbEditorLocationArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x0929a6f4dc04169f);
    schema = (schema ^ (uint64_t)sizeof(((UmiEditorWbEditorLocation *)0)->path)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiEditorWbEditorLocationArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiEditorWbEditorLocation *)0)->path) - 1U +
        8U +
        8U;
}
static void UmiEditorWbEditorLocationArchiveWrite(UmiArchiveWriter *writer, const UmiEditorWbEditorLocation *value)
{
    UmiArchiveWriteText(writer, value->path, sizeof(value->path));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->position.line);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->position.column);
}
static void UmiEditorWbEditorLocationArchiveRead(UmiArchiveReader *reader, UmiEditorWbEditorLocation *value)
{
    UmiArchiveReadText(reader, value->path, sizeof(value->path));
    value->position.line = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->position.column = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
}
static UmiStatus UmiEditorWbEditorLocationArchiveValidate(const UmiEditorWbEditorLocation *value)
{
    return umi_editor_wb_editor_location_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_editor_wb_editor_location_archive_encode, umi_editor_wb_editor_location_archive_decode,
    UmiEditorWbEditorLocation, UmiEditorWbEditorLocationArchiveSchema, UmiEditorWbEditorLocationArchiveBound, UmiEditorWbEditorLocationArchiveWrite, UmiEditorWbEditorLocationArchiveRead, UmiEditorWbEditorLocationArchiveValidate)
