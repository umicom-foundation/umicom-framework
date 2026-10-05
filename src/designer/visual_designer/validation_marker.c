/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/validation_marker.c
 *
 * PURPOSE:
 *   Attach a validation severity/message to a component or property.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/validation_marker.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer validation marker from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_rad_validation_marker_init(UmiRadValidationMarker *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->component_id, sizeof item->component_id, "validation_marker");
    (void)umi_rad_copy_text(item->property_id, sizeof item->property_id, "validation_marker");
    (void)umi_rad_copy_text(item->message, sizeof item->message, "validation_marker");
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer validation marker satisfies its contract before another service relies on
 * it.
 */
int umi_rad_validation_marker_is_valid(const UmiRadValidationMarker *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->component_id, '\0', sizeof(item->component_id)) == NULL) return 0;
    if (memchr(item->property_id, '\0', sizeof(item->property_id)) == NULL) return 0;
    if (memchr(item->message, '\0', sizeof(item->message)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->component_id) && item->message[0] != '\0';}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadValidationMarkerArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x579d8373d61202be);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadValidationMarker *)0)->component_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadValidationMarker *)0)->property_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadValidationMarker *)0)->message)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRadValidationMarkerArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiRadValidationMarker *)0)->component_id) - 1U +
        8U + sizeof(((UmiRadValidationMarker *)0)->property_id) - 1U +
        8U +
        8U + sizeof(((UmiRadValidationMarker *)0)->message) - 1U;
}
static void UmiRadValidationMarkerArchiveWrite(UmiArchiveWriter *writer, const UmiRadValidationMarker *value)
{
    UmiArchiveWriteText(writer, value->component_id, sizeof(value->component_id));
    UmiArchiveWriteText(writer, value->property_id, sizeof(value->property_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->severity);
    UmiArchiveWriteText(writer, value->message, sizeof(value->message));
}
static void UmiRadValidationMarkerArchiveRead(UmiArchiveReader *reader, UmiRadValidationMarker *value)
{
    UmiArchiveReadText(reader, value->component_id, sizeof(value->component_id));
    UmiArchiveReadText(reader, value->property_id, sizeof(value->property_id));
    value->severity = (UmiRadSeverity)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    UmiArchiveReadText(reader, value->message, sizeof(value->message));
}
static UmiStatus UmiRadValidationMarkerArchiveValidate(const UmiRadValidationMarker *value)
{
    return umi_rad_validation_marker_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_validation_marker_archive_encode, umi_rad_validation_marker_archive_decode,
    UmiRadValidationMarker, UmiRadValidationMarkerArchiveSchema, UmiRadValidationMarkerArchiveBound, UmiRadValidationMarkerArchiveWrite, UmiRadValidationMarkerArchiveRead, UmiRadValidationMarkerArchiveValidate)
