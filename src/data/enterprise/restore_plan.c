/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/restore_plan.c
 *
 * PURPOSE:
 *   Describe a restore target and safety gates before destructive data replacement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/restore_plan.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
UmiStatus umi_data_restore_plan_init(UmiDataRestorePlan *item, const char *restore_id, const char *backup_id, uint64_t expected_schema_fingerprint, bool verify_only) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->restore_id,sizeof(item->restore_id),restore_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->backup_id,sizeof(item->backup_id),backup_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->expected_schema_fingerprint=expected_schema_fingerprint;item->verify_only=verify_only;item->preserve_existing=true;item->approved=false;
    return umi_data_restore_plan_validate(item);
}

/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_restore_plan_validate(const UmiDataRestorePlan *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->restore_id, '\0', sizeof(item->restore_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->backup_id, '\0', sizeof(item->backup_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->restore_id[0] != '\0' && item->backup_id[0] != '\0' && item->expected_schema_fingerprint != 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDataRestorePlanArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xf9e35286183840c6);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataRestorePlan *)0)->restore_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataRestorePlan *)0)->backup_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDataRestorePlanArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDataRestorePlan *)0)->restore_id) - 1U +
        8U + sizeof(((UmiDataRestorePlan *)0)->backup_id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDataRestorePlanArchiveWrite(UmiArchiveWriter *writer, const UmiDataRestorePlan *value)
{
    UmiArchiveWriteText(writer, value->restore_id, sizeof(value->restore_id));
    UmiArchiveWriteText(writer, value->backup_id, sizeof(value->backup_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->expected_schema_fingerprint);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->verify_only);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->preserve_existing);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->approved);
}
static void UmiDataRestorePlanArchiveRead(UmiArchiveReader *reader, UmiDataRestorePlan *value)
{
    UmiArchiveReadText(reader, value->restore_id, sizeof(value->restore_id));
    UmiArchiveReadText(reader, value->backup_id, sizeof(value->backup_id));
    value->expected_schema_fingerprint = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->verify_only = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->preserve_existing = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->approved = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDataRestorePlanArchiveValidate(const UmiDataRestorePlan *value)
{
    return umi_data_restore_plan_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_data_restore_plan_archive_encode, umi_data_restore_plan_archive_decode,
    UmiDataRestorePlan, UmiDataRestorePlanArchiveSchema, UmiDataRestorePlanArchiveBound, UmiDataRestorePlanArchiveWrite, UmiDataRestorePlanArchiveRead, UmiDataRestorePlanArchiveValidate)
