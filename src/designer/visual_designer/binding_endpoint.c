/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/binding_endpoint.c
 *
 * PURPOSE:
 *   Represent one source or destination property endpoint in the visual binding editor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/binding_endpoint.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer binding endpoint from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_rad_binding_endpoint_init(UmiRadBindingEndpoint *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->node_id, sizeof item->node_id, "binding_endpoint");
    (void)umi_rad_copy_text(item->property_path, sizeof item->property_path, "binding_endpoint");
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer binding endpoint satisfies its contract before another service relies on
 * it.
 */
int umi_rad_binding_endpoint_is_valid(const UmiRadBindingEndpoint *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->node_id, '\0', sizeof(item->node_id)) == NULL) return 0;
    if (memchr(item->property_path, '\0', sizeof(item->property_path)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->node_id) && item->property_path[0] != '\0';}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadBindingEndpointArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xf860652150170c83);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadBindingEndpoint *)0)->node_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadBindingEndpoint *)0)->property_path)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRadBindingEndpointArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiRadBindingEndpoint *)0)->node_id) - 1U +
        8U + sizeof(((UmiRadBindingEndpoint *)0)->property_path) - 1U +
        8U;
}
static void UmiRadBindingEndpointArchiveWrite(UmiArchiveWriter *writer, const UmiRadBindingEndpoint *value)
{
    UmiArchiveWriteText(writer, value->node_id, sizeof(value->node_id));
    UmiArchiveWriteText(writer, value->property_path, sizeof(value->property_path));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->output);
}
static void UmiRadBindingEndpointArchiveRead(UmiArchiveReader *reader, UmiRadBindingEndpoint *value)
{
    UmiArchiveReadText(reader, value->node_id, sizeof(value->node_id));
    UmiArchiveReadText(reader, value->property_path, sizeof(value->property_path));
    value->output = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiRadBindingEndpointArchiveValidate(const UmiRadBindingEndpoint *value)
{
    return umi_rad_binding_endpoint_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_binding_endpoint_archive_encode, umi_rad_binding_endpoint_archive_decode,
    UmiRadBindingEndpoint, UmiRadBindingEndpointArchiveSchema, UmiRadBindingEndpointArchiveBound, UmiRadBindingEndpointArchiveWrite, UmiRadBindingEndpointArchiveRead, UmiRadBindingEndpointArchiveValidate)
