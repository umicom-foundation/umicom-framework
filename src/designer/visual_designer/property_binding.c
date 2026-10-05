/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/property_binding.c
 *
 * PURPOSE:
 *   Describe a visual property binding backed by the canonical reactive UI state layer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/property_binding.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer property binding from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_rad_property_binding_init(UmiRadPropertyBinding *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->binding_id, sizeof item->binding_id, "property_binding");
    (void)umi_rad_copy_text(item->source_path, sizeof item->source_path, "property_binding");
    (void)umi_rad_copy_text(item->target_path, sizeof item->target_path, "property_binding");
    item->enabled = true;
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer property binding satisfies its contract before another service relies on
 * it.
 */
int umi_rad_property_binding_is_valid(const UmiRadPropertyBinding *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->binding_id, '\0', sizeof(item->binding_id)) == NULL) return 0;
    if (memchr(item->source_path, '\0', sizeof(item->source_path)) == NULL) return 0;
    if (memchr(item->target_path, '\0', sizeof(item->target_path)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->binding_id) && item->source_path[0] != '\0' && item->target_path[0] != '\0';}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadPropertyBindingArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xc1c45465c8b04906);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadPropertyBinding *)0)->binding_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadPropertyBinding *)0)->source_path)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadPropertyBinding *)0)->target_path)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRadPropertyBindingArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiRadPropertyBinding *)0)->binding_id) - 1U +
        8U + sizeof(((UmiRadPropertyBinding *)0)->source_path) - 1U +
        8U + sizeof(((UmiRadPropertyBinding *)0)->target_path) - 1U +
        8U +
        8U;
}
static void UmiRadPropertyBindingArchiveWrite(UmiArchiveWriter *writer, const UmiRadPropertyBinding *value)
{
    UmiArchiveWriteText(writer, value->binding_id, sizeof(value->binding_id));
    UmiArchiveWriteText(writer, value->source_path, sizeof(value->source_path));
    UmiArchiveWriteText(writer, value->target_path, sizeof(value->target_path));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->two_way);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
}
static void UmiRadPropertyBindingArchiveRead(UmiArchiveReader *reader, UmiRadPropertyBinding *value)
{
    UmiArchiveReadText(reader, value->binding_id, sizeof(value->binding_id));
    UmiArchiveReadText(reader, value->source_path, sizeof(value->source_path));
    UmiArchiveReadText(reader, value->target_path, sizeof(value->target_path));
    value->two_way = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiRadPropertyBindingArchiveValidate(const UmiRadPropertyBinding *value)
{
    return umi_rad_property_binding_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_property_binding_archive_encode, umi_rad_property_binding_archive_decode,
    UmiRadPropertyBinding, UmiRadPropertyBindingArchiveSchema, UmiRadPropertyBindingArchiveBound, UmiRadPropertyBindingArchiveWrite, UmiRadPropertyBindingArchiveRead, UmiRadPropertyBindingArchiveValidate)
