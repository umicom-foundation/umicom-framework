/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/binding_wire.c
 *
 * PURPOSE:
 *   Represent a directed visual binding wire between endpoints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/binding_wire.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer binding wire from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_binding_wire_init(UmiRadBindingWire *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->wire_id, sizeof item->wire_id, "binding_wire");
    (void)umi_rad_copy_text(item->source_node_id, sizeof item->source_node_id, "binding_wire");
    (void)umi_rad_copy_text(item->target_node_id, sizeof item->target_node_id, "binding_wire");
    item->enabled = true;
    return UMI_STATUS_OK;
}
/* Check that visual designer binding wire satisfies its contract before another service relies on it. */
int umi_rad_binding_wire_is_valid(const UmiRadBindingWire *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->wire_id, '\0', sizeof(item->wire_id)) == NULL) return 0;
    if (memchr(item->source_node_id, '\0', sizeof(item->source_node_id)) == NULL) return 0;
    if (memchr(item->target_node_id, '\0', sizeof(item->target_node_id)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->wire_id) && umi_rad_id_valid(item->source_node_id) && umi_rad_id_valid(item->target_node_id);}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadBindingWireArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xb14b3c0135abcf4d);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadBindingWire *)0)->wire_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadBindingWire *)0)->source_node_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadBindingWire *)0)->target_node_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRadBindingWireArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiRadBindingWire *)0)->wire_id) - 1U +
        8U + sizeof(((UmiRadBindingWire *)0)->source_node_id) - 1U +
        8U + sizeof(((UmiRadBindingWire *)0)->target_node_id) - 1U +
        8U;
}
static void UmiRadBindingWireArchiveWrite(UmiArchiveWriter *writer, const UmiRadBindingWire *value)
{
    UmiArchiveWriteText(writer, value->wire_id, sizeof(value->wire_id));
    UmiArchiveWriteText(writer, value->source_node_id, sizeof(value->source_node_id));
    UmiArchiveWriteText(writer, value->target_node_id, sizeof(value->target_node_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
}
static void UmiRadBindingWireArchiveRead(UmiArchiveReader *reader, UmiRadBindingWire *value)
{
    UmiArchiveReadText(reader, value->wire_id, sizeof(value->wire_id));
    UmiArchiveReadText(reader, value->source_node_id, sizeof(value->source_node_id));
    UmiArchiveReadText(reader, value->target_node_id, sizeof(value->target_node_id));
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiRadBindingWireArchiveValidate(const UmiRadBindingWire *value)
{
    return umi_rad_binding_wire_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_binding_wire_archive_encode, umi_rad_binding_wire_archive_decode,
    UmiRadBindingWire, UmiRadBindingWireArchiveSchema, UmiRadBindingWireArchiveBound, UmiRadBindingWireArchiveWrite, UmiRadBindingWireArchiveRead, UmiRadBindingWireArchiveValidate)
