/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/property_commit.c
 *
 * PURPOSE:
 *   Record before/after property values for review, undo and audit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/property_commit.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer property commit from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_property_commit_init(UmiRadPropertyCommit *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->component_id, sizeof item->component_id, "property_commit");
    (void)umi_rad_copy_text(item->property_id, sizeof item->property_id, "property_commit");
    (void)umi_rad_copy_text(item->before_value, sizeof item->before_value, "property_commit");
    (void)umi_rad_copy_text(item->after_value, sizeof item->after_value, "property_commit");
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer property commit satisfies its contract before another service relies on
 * it.
 */
int umi_rad_property_commit_is_valid(const UmiRadPropertyCommit *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->component_id, '\0', sizeof(item->component_id)) == NULL) return 0;
    if (memchr(item->property_id, '\0', sizeof(item->property_id)) == NULL) return 0;
    if (memchr(item->before_value, '\0', sizeof(item->before_value)) == NULL) return 0;
    if (memchr(item->after_value, '\0', sizeof(item->after_value)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->component_id) && umi_rad_id_valid(item->property_id);}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadPropertyCommitArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xcca2a160afb4b518);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadPropertyCommit *)0)->component_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadPropertyCommit *)0)->property_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadPropertyCommit *)0)->before_value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadPropertyCommit *)0)->after_value)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRadPropertyCommitArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiRadPropertyCommit *)0)->component_id) - 1U +
        8U + sizeof(((UmiRadPropertyCommit *)0)->property_id) - 1U +
        8U + sizeof(((UmiRadPropertyCommit *)0)->before_value) - 1U +
        8U + sizeof(((UmiRadPropertyCommit *)0)->after_value) - 1U +
        8U;
}
static void UmiRadPropertyCommitArchiveWrite(UmiArchiveWriter *writer, const UmiRadPropertyCommit *value)
{
    UmiArchiveWriteText(writer, value->component_id, sizeof(value->component_id));
    UmiArchiveWriteText(writer, value->property_id, sizeof(value->property_id));
    UmiArchiveWriteText(writer, value->before_value, sizeof(value->before_value));
    UmiArchiveWriteText(writer, value->after_value, sizeof(value->after_value));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiRadPropertyCommitArchiveRead(UmiArchiveReader *reader, UmiRadPropertyCommit *value)
{
    UmiArchiveReadText(reader, value->component_id, sizeof(value->component_id));
    UmiArchiveReadText(reader, value->property_id, sizeof(value->property_id));
    UmiArchiveReadText(reader, value->before_value, sizeof(value->before_value));
    UmiArchiveReadText(reader, value->after_value, sizeof(value->after_value));
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiRadPropertyCommitArchiveValidate(const UmiRadPropertyCommit *value)
{
    return umi_rad_property_commit_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_property_commit_archive_encode, umi_rad_property_commit_archive_decode,
    UmiRadPropertyCommit, UmiRadPropertyCommitArchiveSchema, UmiRadPropertyCommitArchiveBound, UmiRadPropertyCommitArchiveWrite, UmiRadPropertyCommitArchiveRead, UmiRadPropertyCommitArchiveValidate)
