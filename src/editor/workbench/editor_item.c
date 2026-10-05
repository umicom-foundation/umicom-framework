/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/editor_item.c
 *
 * PURPOSE:
 *   Describe one open editor item independently of any toolkit tab widget.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_item.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/*
 * Initialise editor wb editor item from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_editor_wb_editor_item_init(UmiEditorWbEditorItem *item,const char *id,const char *path,UmiEditorWbOpenMode mode){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL||!umi_editor_wb_id_valid(id)||path==NULL)return UMI_STATUS_INVALID_ARGUMENT; memset(item,0,sizeof *item); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_wb_copy_text(item->item_id,sizeof item->item_id,id)!=UMI_STATUS_OK||umi_editor_wb_copy_text(item->path,sizeof item->path,path)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED; item->open_mode=mode; item->pinned=(mode==UMI_EDITOR_WB_OPEN_PINNED); item->revision=1U; return UMI_STATUS_OK;}
/*
 * Provide the editor wb editor item set dirty operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_wb_editor_item_set_dirty(UmiEditorWbEditorItem *item,bool dirty){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT; item->dirty=dirty; item->revision++; return UMI_STATUS_OK;}
/*
 * Check that editor wb editor item satisfies its contract before another service relies on
 * it.
 */
int umi_editor_wb_editor_item_valid(const UmiEditorWbEditorItem *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->item_id, '\0', sizeof(item->item_id)) == NULL) return 0;
    if (memchr(item->path, '\0', sizeof(item->path)) == NULL) return 0;
return item!=NULL&&umi_editor_wb_id_valid(item->item_id)&&item->path[0]!='\0'&&item->open_mode>=UMI_EDITOR_WB_OPEN_NORMAL&&item->open_mode<=UMI_EDITOR_WB_OPEN_PINNED;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiEditorWbEditorItemArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xdc70937b96c203e7);
    schema = (schema ^ (uint64_t)sizeof(((UmiEditorWbEditorItem *)0)->item_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiEditorWbEditorItem *)0)->path)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiEditorWbEditorItemArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiEditorWbEditorItem *)0)->item_id) - 1U +
        8U + sizeof(((UmiEditorWbEditorItem *)0)->path) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiEditorWbEditorItemArchiveWrite(UmiArchiveWriter *writer, const UmiEditorWbEditorItem *value)
{
    UmiArchiveWriteText(writer, value->item_id, sizeof(value->item_id));
    UmiArchiveWriteText(writer, value->path, sizeof(value->path));
    UmiArchiveWriteSigned(writer, (int64_t)value->open_mode);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->dirty);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->pinned);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiEditorWbEditorItemArchiveRead(UmiArchiveReader *reader, UmiEditorWbEditorItem *value)
{
    UmiArchiveReadText(reader, value->item_id, sizeof(value->item_id));
    UmiArchiveReadText(reader, value->path, sizeof(value->path));
    value->open_mode = (UmiEditorWbOpenMode)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->dirty = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->pinned = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiEditorWbEditorItemArchiveValidate(const UmiEditorWbEditorItem *value)
{
    return umi_editor_wb_editor_item_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_editor_wb_editor_item_archive_encode, umi_editor_wb_editor_item_archive_decode,
    UmiEditorWbEditorItem, UmiEditorWbEditorItemArchiveSchema, UmiEditorWbEditorItemArchiveBound, UmiEditorWbEditorItemArchiveWrite, UmiEditorWbEditorItemArchiveRead, UmiEditorWbEditorItemArchiveValidate)
