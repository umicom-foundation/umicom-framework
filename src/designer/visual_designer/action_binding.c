/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/action_binding.c
 *
 * PURPOSE:
 *   Bind a designer action surface to a Framework command and target.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/action_binding.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer action binding from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_action_binding_init(UmiRadActionBinding *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->action_id, sizeof item->action_id, "action_binding");
    (void)umi_rad_copy_text(item->command_id, sizeof item->command_id, "action_binding");
    (void)umi_rad_copy_text(item->target_id, sizeof item->target_id, "action_binding");
    item->enabled = true;
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer action binding satisfies its contract before another service relies on
 * it.
 */
int umi_rad_action_binding_is_valid(const UmiRadActionBinding *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->action_id, '\0', sizeof(item->action_id)) == NULL) return 0;
    if (memchr(item->command_id, '\0', sizeof(item->command_id)) == NULL) return 0;
    if (memchr(item->target_id, '\0', sizeof(item->target_id)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->action_id) && umi_rad_id_valid(item->command_id);}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadActionBindingArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x025008765973e035);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadActionBinding *)0)->action_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadActionBinding *)0)->command_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadActionBinding *)0)->target_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRadActionBindingArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiRadActionBinding *)0)->action_id) - 1U +
        8U + sizeof(((UmiRadActionBinding *)0)->command_id) - 1U +
        8U + sizeof(((UmiRadActionBinding *)0)->target_id) - 1U +
        8U;
}
static void UmiRadActionBindingArchiveWrite(UmiArchiveWriter *writer, const UmiRadActionBinding *value)
{
    UmiArchiveWriteText(writer, value->action_id, sizeof(value->action_id));
    UmiArchiveWriteText(writer, value->command_id, sizeof(value->command_id));
    UmiArchiveWriteText(writer, value->target_id, sizeof(value->target_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
}
static void UmiRadActionBindingArchiveRead(UmiArchiveReader *reader, UmiRadActionBinding *value)
{
    UmiArchiveReadText(reader, value->action_id, sizeof(value->action_id));
    UmiArchiveReadText(reader, value->command_id, sizeof(value->command_id));
    UmiArchiveReadText(reader, value->target_id, sizeof(value->target_id));
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiRadActionBindingArchiveValidate(const UmiRadActionBinding *value)
{
    return umi_rad_action_binding_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_action_binding_archive_encode, umi_rad_action_binding_archive_decode,
    UmiRadActionBinding, UmiRadActionBindingArchiveSchema, UmiRadActionBindingArchiveBound, UmiRadActionBindingArchiveWrite, UmiRadActionBindingArchiveRead, UmiRadActionBindingArchiveValidate)
