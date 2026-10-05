/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/event_binding.c
 *
 * PURPOSE:
 *   Bind a semantic component event to a Framework command identifier.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/event_binding.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer event binding from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_event_binding_init(UmiRadEventBinding *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->binding_id, sizeof item->binding_id, "event_binding");
    (void)umi_rad_copy_text(item->component_id, sizeof item->component_id, "event_binding");
    (void)umi_rad_copy_text(item->event_id, sizeof item->event_id, "event_binding");
    (void)umi_rad_copy_text(item->command_id, sizeof item->command_id, "event_binding");
    item->enabled = true;
    return UMI_STATUS_OK;
}
/* Check that visual designer event binding satisfies its contract before another service relies on it. */
int umi_rad_event_binding_is_valid(const UmiRadEventBinding *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->binding_id, '\0', sizeof(item->binding_id)) == NULL) return 0;
    if (memchr(item->component_id, '\0', sizeof(item->component_id)) == NULL) return 0;
    if (memchr(item->event_id, '\0', sizeof(item->event_id)) == NULL) return 0;
    if (memchr(item->command_id, '\0', sizeof(item->command_id)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->binding_id) && umi_rad_id_valid(item->event_id) && umi_rad_id_valid(item->command_id);}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadEventBindingArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x1bee0d69779ca2e7);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadEventBinding *)0)->binding_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadEventBinding *)0)->component_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadEventBinding *)0)->event_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadEventBinding *)0)->command_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRadEventBindingArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiRadEventBinding *)0)->binding_id) - 1U +
        8U + sizeof(((UmiRadEventBinding *)0)->component_id) - 1U +
        8U + sizeof(((UmiRadEventBinding *)0)->event_id) - 1U +
        8U + sizeof(((UmiRadEventBinding *)0)->command_id) - 1U +
        8U;
}
static void UmiRadEventBindingArchiveWrite(UmiArchiveWriter *writer, const UmiRadEventBinding *value)
{
    UmiArchiveWriteText(writer, value->binding_id, sizeof(value->binding_id));
    UmiArchiveWriteText(writer, value->component_id, sizeof(value->component_id));
    UmiArchiveWriteText(writer, value->event_id, sizeof(value->event_id));
    UmiArchiveWriteText(writer, value->command_id, sizeof(value->command_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
}
static void UmiRadEventBindingArchiveRead(UmiArchiveReader *reader, UmiRadEventBinding *value)
{
    UmiArchiveReadText(reader, value->binding_id, sizeof(value->binding_id));
    UmiArchiveReadText(reader, value->component_id, sizeof(value->component_id));
    UmiArchiveReadText(reader, value->event_id, sizeof(value->event_id));
    UmiArchiveReadText(reader, value->command_id, sizeof(value->command_id));
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiRadEventBindingArchiveValidate(const UmiRadEventBinding *value)
{
    return umi_rad_event_binding_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_event_binding_archive_encode, umi_rad_event_binding_archive_decode,
    UmiRadEventBinding, UmiRadEventBindingArchiveSchema, UmiRadEventBindingArchiveBound, UmiRadEventBindingArchiveWrite, UmiRadEventBindingArchiveRead, UmiRadEventBindingArchiveValidate)
