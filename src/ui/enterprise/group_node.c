/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/group_node.c
 *
 * PURPOSE:
 *   Represent one grouped range projected by an enterprise grid.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/group_node.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise ui ent group node from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_ent_group_node_init(UmiUiEntGroupNode *value){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!value)return UMI_STATUS_INVALID_ARGUMENT;memset(value,0,sizeof *value);value->group_id[0]='\0';value->label[0]='\0';value->first_row=0;value->row_count=0;value->depth=0;value->expanded=0;return UMI_STATUS_OK;}
/* Check that ui ent group node satisfies its contract before another service relies on it. */
int umi_ui_ent_group_node_validate(const UmiUiEntGroupNode *value){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->group_id, '\0', sizeof(value->group_id)) == NULL) return 0;
    if (memchr(value->label, '\0', sizeof(value->label)) == NULL) return 0;
return value!=NULL&&umi_ui_ent_id_valid(value->group_id)&&value->row_count>0U;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiEntGroupNodeArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x5fcde5c67d50b471);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiEntGroupNode *)0)->group_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiEntGroupNode *)0)->label)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiEntGroupNodeArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiEntGroupNode *)0)->group_id) - 1U +
        8U + sizeof(((UmiUiEntGroupNode *)0)->label) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiUiEntGroupNodeArchiveWrite(UmiArchiveWriter *writer, const UmiUiEntGroupNode *value)
{
    UmiArchiveWriteText(writer, value->group_id, sizeof(value->group_id));
    UmiArchiveWriteText(writer, value->label, sizeof(value->label));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->first_row);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->row_count);
    UmiArchiveWriteSigned(writer, (int64_t)value->depth);
    UmiArchiveWriteSigned(writer, (int64_t)value->expanded);
}
static void UmiUiEntGroupNodeArchiveRead(UmiArchiveReader *reader, UmiUiEntGroupNode *value)
{
    UmiArchiveReadText(reader, value->group_id, sizeof(value->group_id));
    UmiArchiveReadText(reader, value->label, sizeof(value->label));
    value->first_row = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->row_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->depth = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->expanded = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiUiEntGroupNodeArchiveValidate(const UmiUiEntGroupNode *value)
{
    return umi_ui_ent_group_node_validate(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_ent_group_node_archive_encode, umi_ui_ent_group_node_archive_decode,
    UmiUiEntGroupNode, UmiUiEntGroupNodeArchiveSchema, UmiUiEntGroupNodeArchiveBound, UmiUiEntGroupNodeArchiveWrite, UmiUiEntGroupNodeArchiveRead, UmiUiEntGroupNodeArchiveValidate)
