/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/editor_layout_snapshot.c
 *
 * PURPOSE:
 *   Capture immutable editor-layout revision metadata for restore/history.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_layout_snapshot.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/*
 * Provide the htext editor layout snapshot operation used by this module and its client
 * applications.
 */
static uint64_t htext_editor_layout_snapshot(const char *p){uint64_t h=1469598103934665603ULL;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(*p!='\0'){h^=(uint64_t)(unsigned char)*p++;h*=1099511628211ULL;}return h;}
/*
 * Provide the editor wb editor layout snapshot capture operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_wb_editor_layout_snapshot_capture(UmiEditorWbEditorLayoutSnapshot *s,const char *active,size_t items,size_t groups,uint64_t rev){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||active==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof *s);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_wb_copy_text(s->active_id,sizeof s->active_id,active)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;s->item_count=items;s->group_count=groups;s->revision=rev;s->fingerprint=htext_editor_layout_snapshot(active)^(uint64_t)items^((uint64_t)groups<<32U)^rev;return UMI_STATUS_OK;} int umi_editor_wb_editor_layout_snapshot_valid(const UmiEditorWbEditorLayoutSnapshot *s){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (s == NULL) return 0;
    if (memchr(s->active_id, '\0', sizeof(s->active_id)) == NULL) return 0;
return s!=NULL&&s->revision>0U&&s->group_count<=UMI_EDITOR_WB_MAX_GROUPS;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiEditorWbEditorLayoutSnapshotArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x8ea569793b60674b);
    schema = (schema ^ (uint64_t)sizeof(((UmiEditorWbEditorLayoutSnapshot *)0)->active_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiEditorWbEditorLayoutSnapshotArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiEditorWbEditorLayoutSnapshot *)0)->active_id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiEditorWbEditorLayoutSnapshotArchiveWrite(UmiArchiveWriter *writer, const UmiEditorWbEditorLayoutSnapshot *value)
{
    UmiArchiveWriteText(writer, value->active_id, sizeof(value->active_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->item_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->group_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->fingerprint);
}
static void UmiEditorWbEditorLayoutSnapshotArchiveRead(UmiArchiveReader *reader, UmiEditorWbEditorLayoutSnapshot *value)
{
    UmiArchiveReadText(reader, value->active_id, sizeof(value->active_id));
    value->item_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->group_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->fingerprint = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiEditorWbEditorLayoutSnapshotArchiveValidate(const UmiEditorWbEditorLayoutSnapshot *value)
{
    return umi_editor_wb_editor_layout_snapshot_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_editor_wb_editor_layout_snapshot_archive_encode, umi_editor_wb_editor_layout_snapshot_archive_decode,
    UmiEditorWbEditorLayoutSnapshot, UmiEditorWbEditorLayoutSnapshotArchiveSchema, UmiEditorWbEditorLayoutSnapshotArchiveBound, UmiEditorWbEditorLayoutSnapshotArchiveWrite, UmiEditorWbEditorLayoutSnapshotArchiveRead, UmiEditorWbEditorLayoutSnapshotArchiveValidate)
