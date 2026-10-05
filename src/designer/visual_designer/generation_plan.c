/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/generation_plan.c
 *
 * PURPOSE:
 *   Describe generated declarative/source artifacts before filesystem writes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/generation_plan.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer generation plan from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_generation_plan_init(UmiRadGenerationPlan *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->application_id, sizeof item->application_id, "generation_plan");
    (void)umi_rad_copy_text(item->output_root, sizeof item->output_root, "generation_plan");
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer generation plan satisfies its contract before another service relies on
 * it.
 */
int umi_rad_generation_plan_is_valid(const UmiRadGenerationPlan *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->application_id, '\0', sizeof(item->application_id)) == NULL) return 0;
    if (memchr(item->output_root, '\0', sizeof(item->output_root)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->application_id) && item->output_root[0] != '\0';}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadGenerationPlanArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xece2ee0b32b5e9a3);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadGenerationPlan *)0)->application_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadGenerationPlan *)0)->output_root)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRadGenerationPlanArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiRadGenerationPlan *)0)->application_id) - 1U +
        8U + sizeof(((UmiRadGenerationPlan *)0)->output_root) - 1U +
        8U +
        8U +
        8U;
}
static void UmiRadGenerationPlanArchiveWrite(UmiArchiveWriter *writer, const UmiRadGenerationPlan *value)
{
    UmiArchiveWriteText(writer, value->application_id, sizeof(value->application_id));
    UmiArchiveWriteText(writer, value->output_root, sizeof(value->output_root));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->file_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->declarative_enabled);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->source_enabled);
}
static void UmiRadGenerationPlanArchiveRead(UmiArchiveReader *reader, UmiRadGenerationPlan *value)
{
    UmiArchiveReadText(reader, value->application_id, sizeof(value->application_id));
    UmiArchiveReadText(reader, value->output_root, sizeof(value->output_root));
    value->file_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->declarative_enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->source_enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiRadGenerationPlanArchiveValidate(const UmiRadGenerationPlan *value)
{
    return umi_rad_generation_plan_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_generation_plan_archive_encode, umi_rad_generation_plan_archive_decode,
    UmiRadGenerationPlan, UmiRadGenerationPlanArchiveSchema, UmiRadGenerationPlanArchiveBound, UmiRadGenerationPlanArchiveWrite, UmiRadGenerationPlanArchiveRead, UmiRadGenerationPlanArchiveValidate)
