/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/component_catalogue_bridge.c
 *
 * PURPOSE:
 *   Map Design System component identifiers to canonical designer component types.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/component_catalogue_bridge.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer component catalogue bridge from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_rad_component_catalogue_bridge_init(UmiRadComponentCatalogueBridge *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->design_component_id, sizeof item->design_component_id, "component_catalogue_bridge");
    (void)umi_rad_copy_text(item->designer_type, sizeof item->designer_type, "component_catalogue_bridge");
    (void)umi_rad_copy_text(item->family, sizeof item->family, "component_catalogue_bridge");
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer component catalogue bridge satisfies its contract before another service
 * relies on it.
 */
int umi_rad_component_catalogue_bridge_is_valid(const UmiRadComponentCatalogueBridge *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->design_component_id, '\0', sizeof(item->design_component_id)) == NULL) return 0;
    if (memchr(item->designer_type, '\0', sizeof(item->designer_type)) == NULL) return 0;
    if (memchr(item->family, '\0', sizeof(item->family)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->design_component_id) && umi_rad_id_valid(item->designer_type);}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadComponentCatalogueBridgeArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x5911dbb8204a2d67);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadComponentCatalogueBridge *)0)->design_component_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadComponentCatalogueBridge *)0)->designer_type)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadComponentCatalogueBridge *)0)->family)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRadComponentCatalogueBridgeArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiRadComponentCatalogueBridge *)0)->design_component_id) - 1U +
        8U + sizeof(((UmiRadComponentCatalogueBridge *)0)->designer_type) - 1U +
        8U + sizeof(((UmiRadComponentCatalogueBridge *)0)->family) - 1U +
        8U;
}
static void UmiRadComponentCatalogueBridgeArchiveWrite(UmiArchiveWriter *writer, const UmiRadComponentCatalogueBridge *value)
{
    UmiArchiveWriteText(writer, value->design_component_id, sizeof(value->design_component_id));
    UmiArchiveWriteText(writer, value->designer_type, sizeof(value->designer_type));
    UmiArchiveWriteText(writer, value->family, sizeof(value->family));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->available);
}
static void UmiRadComponentCatalogueBridgeArchiveRead(UmiArchiveReader *reader, UmiRadComponentCatalogueBridge *value)
{
    UmiArchiveReadText(reader, value->design_component_id, sizeof(value->design_component_id));
    UmiArchiveReadText(reader, value->designer_type, sizeof(value->designer_type));
    UmiArchiveReadText(reader, value->family, sizeof(value->family));
    value->available = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiRadComponentCatalogueBridgeArchiveValidate(const UmiRadComponentCatalogueBridge *value)
{
    return umi_rad_component_catalogue_bridge_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_component_catalogue_bridge_archive_encode, umi_rad_component_catalogue_bridge_archive_decode,
    UmiRadComponentCatalogueBridge, UmiRadComponentCatalogueBridgeArchiveSchema, UmiRadComponentCatalogueBridgeArchiveBound, UmiRadComponentCatalogueBridgeArchiveWrite, UmiRadComponentCatalogueBridgeArchiveRead, UmiRadComponentCatalogueBridgeArchiveValidate)
