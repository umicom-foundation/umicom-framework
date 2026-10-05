/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/insertion_marker.c
 *
 * PURPOSE:
 *   Represent insertion feedback within ordered containers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/insertion_marker.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer insertion marker from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_rad_insertion_marker_init(UmiRadInsertionMarker *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->parent_id, sizeof item->parent_id, "insertion_marker");
    item->visible = true;
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer insertion marker satisfies its contract before another service relies on
 * it.
 */
int umi_rad_insertion_marker_is_valid(const UmiRadInsertionMarker *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->parent_id, '\0', sizeof(item->parent_id)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->parent_id) && item->index >= 0;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadInsertionMarkerArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xb50101f9cc955739);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadInsertionMarker *)0)->parent_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRadInsertionMarkerArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiRadInsertionMarker *)0)->parent_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiRadInsertionMarkerArchiveWrite(UmiArchiveWriter *writer, const UmiRadInsertionMarker *value)
{
    UmiArchiveWriteText(writer, value->parent_id, sizeof(value->parent_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->index);
    UmiArchiveWriteSigned(writer, (int64_t)value->bounds.x);
    UmiArchiveWriteSigned(writer, (int64_t)value->bounds.y);
    UmiArchiveWriteSigned(writer, (int64_t)value->bounds.width);
    UmiArchiveWriteSigned(writer, (int64_t)value->bounds.height);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->visible);
}
static void UmiRadInsertionMarkerArchiveRead(UmiArchiveReader *reader, UmiRadInsertionMarker *value)
{
    UmiArchiveReadText(reader, value->parent_id, sizeof(value->parent_id));
    value->index = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->bounds.x = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->bounds.y = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->bounds.width = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->bounds.height = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->visible = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiRadInsertionMarkerArchiveValidate(const UmiRadInsertionMarker *value)
{
    return umi_rad_insertion_marker_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_insertion_marker_archive_encode, umi_rad_insertion_marker_archive_decode,
    UmiRadInsertionMarker, UmiRadInsertionMarkerArchiveSchema, UmiRadInsertionMarkerArchiveBound, UmiRadInsertionMarkerArchiveWrite, UmiRadInsertionMarkerArchiveRead, UmiRadInsertionMarkerArchiveValidate)
