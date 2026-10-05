/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/duplicate_plan.c
 *
 * PURPOSE:
 *   Describe deterministic component duplication before it is committed.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/duplicate_plan.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer duplicate plan from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_duplicate_plan_init(UmiRadDuplicatePlan *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->source_id, sizeof item->source_id, "duplicate_plan");
    (void)umi_rad_copy_text(item->new_id, sizeof item->new_id, "duplicate_plan");
    (void)umi_rad_copy_text(item->new_parent_id, sizeof item->new_parent_id, "duplicate_plan");
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer duplicate plan satisfies its contract before another service relies on
 * it.
 */
int umi_rad_duplicate_plan_is_valid(const UmiRadDuplicatePlan *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->source_id, '\0', sizeof(item->source_id)) == NULL) return 0;
    if (memchr(item->new_id, '\0', sizeof(item->new_id)) == NULL) return 0;
    if (memchr(item->new_parent_id, '\0', sizeof(item->new_parent_id)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->source_id) && umi_rad_id_valid(item->new_id);}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadDuplicatePlanArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x2232450c1dd9de8e);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadDuplicatePlan *)0)->source_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadDuplicatePlan *)0)->new_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadDuplicatePlan *)0)->new_parent_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRadDuplicatePlanArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiRadDuplicatePlan *)0)->source_id) - 1U +
        8U + sizeof(((UmiRadDuplicatePlan *)0)->new_id) - 1U +
        8U + sizeof(((UmiRadDuplicatePlan *)0)->new_parent_id) - 1U +
        8U +
        8U;
}
static void UmiRadDuplicatePlanArchiveWrite(UmiArchiveWriter *writer, const UmiRadDuplicatePlan *value)
{
    UmiArchiveWriteText(writer, value->source_id, sizeof(value->source_id));
    UmiArchiveWriteText(writer, value->new_id, sizeof(value->new_id));
    UmiArchiveWriteText(writer, value->new_parent_id, sizeof(value->new_parent_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->offset.x);
    UmiArchiveWriteSigned(writer, (int64_t)value->offset.y);
}
static void UmiRadDuplicatePlanArchiveRead(UmiArchiveReader *reader, UmiRadDuplicatePlan *value)
{
    UmiArchiveReadText(reader, value->source_id, sizeof(value->source_id));
    UmiArchiveReadText(reader, value->new_id, sizeof(value->new_id));
    UmiArchiveReadText(reader, value->new_parent_id, sizeof(value->new_parent_id));
    value->offset.x = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->offset.y = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
}
static UmiStatus UmiRadDuplicatePlanArchiveValidate(const UmiRadDuplicatePlan *value)
{
    return umi_rad_duplicate_plan_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_duplicate_plan_archive_encode, umi_rad_duplicate_plan_archive_decode,
    UmiRadDuplicatePlan, UmiRadDuplicatePlanArchiveSchema, UmiRadDuplicatePlanArchiveBound, UmiRadDuplicatePlanArchiveWrite, UmiRadDuplicatePlanArchiveRead, UmiRadDuplicatePlanArchiveValidate)
