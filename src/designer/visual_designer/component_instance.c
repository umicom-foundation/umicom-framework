/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/component_instance.c
 *
 * PURPOSE:
 *   Represent one semantic component instance on a designer document.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/component_instance.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer component instance from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_rad_component_instance_init(UmiRadComponentInstance *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->component_id, sizeof item->component_id, "component_instance");
    (void)umi_rad_copy_text(item->component_type, sizeof item->component_type, "component_instance");
    (void)umi_rad_copy_text(item->parent_id, sizeof item->parent_id, "component_instance");
    item->visible = true;
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer component instance satisfies its contract before another service relies
 * on it.
 */
int umi_rad_component_instance_is_valid(const UmiRadComponentInstance *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->component_id, '\0', sizeof(item->component_id)) == NULL) return 0;
    if (memchr(item->component_type, '\0', sizeof(item->component_type)) == NULL) return 0;
    if (memchr(item->parent_id, '\0', sizeof(item->parent_id)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->component_id) && umi_rad_id_valid(item->component_type);}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadComponentInstanceArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xfcb234ec29d5da1c);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadComponentInstance *)0)->component_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadComponentInstance *)0)->component_type)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadComponentInstance *)0)->parent_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRadComponentInstanceArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiRadComponentInstance *)0)->component_id) - 1U +
        8U + sizeof(((UmiRadComponentInstance *)0)->component_type) - 1U +
        8U + sizeof(((UmiRadComponentInstance *)0)->parent_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiRadComponentInstanceArchiveWrite(UmiArchiveWriter *writer, const UmiRadComponentInstance *value)
{
    UmiArchiveWriteText(writer, value->component_id, sizeof(value->component_id));
    UmiArchiveWriteText(writer, value->component_type, sizeof(value->component_type));
    UmiArchiveWriteText(writer, value->parent_id, sizeof(value->parent_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->bounds.x);
    UmiArchiveWriteSigned(writer, (int64_t)value->bounds.y);
    UmiArchiveWriteSigned(writer, (int64_t)value->bounds.width);
    UmiArchiveWriteSigned(writer, (int64_t)value->bounds.height);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->visible);
}
static void UmiRadComponentInstanceArchiveRead(UmiArchiveReader *reader, UmiRadComponentInstance *value)
{
    UmiArchiveReadText(reader, value->component_id, sizeof(value->component_id));
    UmiArchiveReadText(reader, value->component_type, sizeof(value->component_type));
    UmiArchiveReadText(reader, value->parent_id, sizeof(value->parent_id));
    value->bounds.x = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->bounds.y = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->bounds.width = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->bounds.height = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->visible = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiRadComponentInstanceArchiveValidate(const UmiRadComponentInstance *value)
{
    return umi_rad_component_instance_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_component_instance_archive_encode, umi_rad_component_instance_archive_decode,
    UmiRadComponentInstance, UmiRadComponentInstanceArchiveSchema, UmiRadComponentInstanceArchiveBound, UmiRadComponentInstanceArchiveWrite, UmiRadComponentInstanceArchiveRead, UmiRadComponentInstanceArchiveValidate)
