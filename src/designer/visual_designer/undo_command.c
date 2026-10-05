/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/undo_command.c
 *
 * PURPOSE:
 *   Represent a reversible designer mutation without toolkit dependencies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/undo_command.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer undo command from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_undo_command_init(UmiRadUndoCommand *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->command_id, sizeof item->command_id, "undo_command");
    (void)umi_rad_copy_text(item->target_id, sizeof item->target_id, "undo_command");
    (void)umi_rad_copy_text(item->before_value, sizeof item->before_value, "undo_command");
    (void)umi_rad_copy_text(item->after_value, sizeof item->after_value, "undo_command");
    return UMI_STATUS_OK;
}
/* Check that visual designer undo command satisfies its contract before another service relies on it. */
int umi_rad_undo_command_is_valid(const UmiRadUndoCommand *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->command_id, '\0', sizeof(item->command_id)) == NULL) return 0;
    if (memchr(item->target_id, '\0', sizeof(item->target_id)) == NULL) return 0;
    if (memchr(item->before_value, '\0', sizeof(item->before_value)) == NULL) return 0;
    if (memchr(item->after_value, '\0', sizeof(item->after_value)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->command_id) && umi_rad_id_valid(item->target_id);}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadUndoCommandArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xeea25398f201d44a);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadUndoCommand *)0)->command_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadUndoCommand *)0)->target_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadUndoCommand *)0)->before_value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadUndoCommand *)0)->after_value)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRadUndoCommandArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiRadUndoCommand *)0)->command_id) - 1U +
        8U + sizeof(((UmiRadUndoCommand *)0)->target_id) - 1U +
        8U + sizeof(((UmiRadUndoCommand *)0)->before_value) - 1U +
        8U + sizeof(((UmiRadUndoCommand *)0)->after_value) - 1U;
}
static void UmiRadUndoCommandArchiveWrite(UmiArchiveWriter *writer, const UmiRadUndoCommand *value)
{
    UmiArchiveWriteText(writer, value->command_id, sizeof(value->command_id));
    UmiArchiveWriteText(writer, value->target_id, sizeof(value->target_id));
    UmiArchiveWriteText(writer, value->before_value, sizeof(value->before_value));
    UmiArchiveWriteText(writer, value->after_value, sizeof(value->after_value));
}
static void UmiRadUndoCommandArchiveRead(UmiArchiveReader *reader, UmiRadUndoCommand *value)
{
    UmiArchiveReadText(reader, value->command_id, sizeof(value->command_id));
    UmiArchiveReadText(reader, value->target_id, sizeof(value->target_id));
    UmiArchiveReadText(reader, value->before_value, sizeof(value->before_value));
    UmiArchiveReadText(reader, value->after_value, sizeof(value->after_value));
}
static UmiStatus UmiRadUndoCommandArchiveValidate(const UmiRadUndoCommand *value)
{
    return umi_rad_undo_command_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_undo_command_archive_encode, umi_rad_undo_command_archive_decode,
    UmiRadUndoCommand, UmiRadUndoCommandArchiveSchema, UmiRadUndoCommandArchiveBound, UmiRadUndoCommandArchiveWrite, UmiRadUndoCommandArchiveRead, UmiRadUndoCommandArchiveValidate)
