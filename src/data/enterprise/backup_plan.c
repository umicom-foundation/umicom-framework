/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/backup_plan.c
 *
 * PURPOSE:
 *   Describe a reviewable full/incremental backup request and retention class.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/conditional_archive_internal.h"
#include "umicom/data/enterprise/backup_plan.h"
#include "../../base/value_construction_internal.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_data_backup_plan_init(UmiDataBackupPlan *item, const char *backup_id, const char *destination, uint64_t schema_fingerprint, bool incremental) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->backup_id,sizeof(item->backup_id),backup_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->destination,sizeof(item->destination),destination);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->schema_fingerprint=schema_fingerprint;item->incremental=incremental;item->include_blobs=true;item->encrypted=true;
    return umi_data_backup_plan_validate(item);
}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_data_backup_plan_init_build_value(UmiDataBackupPlan *item, const char *backup_id, const char *destination, uint64_t schema_fingerprint, bool incremental) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->backup_id,sizeof(item->backup_id),backup_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->destination,sizeof(item->destination),destination);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->schema_fingerprint=schema_fingerprint;item->incremental=incremental;item->include_blobs=true;item->encrypted=true;
    return umi_data_backup_plan_validate(item);
}

UMI_DEFINE_STAGED_VALUE_INIT(umi_data_backup_plan_init, UmiDataBackupPlan, item,
    (UmiDataBackupPlan *item, const char *backup_id, const char *destination, uint64_t schema_fingerprint, bool incremental),
    umi_data_backup_plan_init_build_value(candidate, backup_id, destination, schema_fingerprint, incremental))


/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_backup_plan_validate(const UmiDataBackupPlan *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->backup_id, '\0', sizeof(item->backup_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->destination, '\0', sizeof(item->destination)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->backup_id[0] != '\0' && item->destination[0] != '\0' && item->schema_fingerprint != 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDataBackupPlanArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x015b25e701521621);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataBackupPlan *)0)->backup_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataBackupPlan *)0)->destination)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDataBackupPlanArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDataBackupPlan *)0)->backup_id) - 1U +
        8U + sizeof(((UmiDataBackupPlan *)0)->destination) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDataBackupPlanArchiveWrite(UmiArchiveWriter *writer, const UmiDataBackupPlan *value)
{
    UmiArchiveWriteText(writer, value->backup_id, sizeof(value->backup_id));
    UmiArchiveWriteText(writer, value->destination, sizeof(value->destination));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->schema_fingerprint);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->incremental);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->include_blobs);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->encrypted);
}
static void UmiDataBackupPlanArchiveRead(UmiArchiveReader *reader, UmiDataBackupPlan *value)
{
    UmiArchiveReadText(reader, value->backup_id, sizeof(value->backup_id));
    UmiArchiveReadText(reader, value->destination, sizeof(value->destination));
    value->schema_fingerprint = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->incremental = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->include_blobs = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->encrypted = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDataBackupPlanArchiveValidate(const UmiDataBackupPlan *value)
{
    return umi_data_backup_plan_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_data_backup_plan_archive_encode, umi_data_backup_plan_archive_decode,
    UmiDataBackupPlan, UmiDataBackupPlanArchiveSchema, UmiDataBackupPlanArchiveBound, UmiDataBackupPlanArchiveWrite, UmiDataBackupPlanArchiveRead, UmiDataBackupPlanArchiveValidate)

/* Restore this typed value only while its encoded state still matches the
 * caller's reviewed snapshot. The shared guard checks every encoded byte;
 * domain decoding and validation stay with this owner. */
UMI_DEFINE_CONDITIONAL_ARCHIVE(umi_data_backup_plan_archive_decode_if_unchanged, umi_data_backup_plan_archive_encode,
    UmiDataBackupPlan, UmiDataBackupPlanArchiveSchema, UmiDataBackupPlanArchiveBound, UmiDataBackupPlanArchiveRead, UmiDataBackupPlanArchiveValidate)
