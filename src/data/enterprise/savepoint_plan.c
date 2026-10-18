/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/savepoint_plan.c
 *
 * PURPOSE:
 *   Describe explicit savepoints for backend adapters that support nested recovery.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/conditional_archive_internal.h"
#include "umicom/data/enterprise/savepoint_plan.h"
#include "../../base/value_construction_internal.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_data_savepoint_plan_init(UmiDataSavepointPlan *item, const char *savepoint_id, uint32_t ordinal) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->savepoint_id,sizeof(item->savepoint_id),savepoint_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->ordinal=ordinal;item->release_on_success=true;item->rollback_on_failure=true;
    return umi_data_savepoint_plan_validate(item);
}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_data_savepoint_plan_init_build_value(UmiDataSavepointPlan *item, const char *savepoint_id, uint32_t ordinal) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->savepoint_id,sizeof(item->savepoint_id),savepoint_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->ordinal=ordinal;item->release_on_success=true;item->rollback_on_failure=true;
    return umi_data_savepoint_plan_validate(item);
}

UMI_DEFINE_STAGED_VALUE_INIT(umi_data_savepoint_plan_init, UmiDataSavepointPlan, item,
    (UmiDataSavepointPlan *item, const char *savepoint_id, uint32_t ordinal),
    umi_data_savepoint_plan_init_build_value(candidate, savepoint_id, ordinal))


/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_savepoint_plan_validate(const UmiDataSavepointPlan *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->savepoint_id, '\0', sizeof(item->savepoint_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->savepoint_id[0] != '\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDataSavepointPlanArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xb8f6995b9a0fbd69);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataSavepointPlan *)0)->savepoint_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDataSavepointPlanArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDataSavepointPlan *)0)->savepoint_id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiDataSavepointPlanArchiveWrite(UmiArchiveWriter *writer, const UmiDataSavepointPlan *value)
{
    UmiArchiveWriteText(writer, value->savepoint_id, sizeof(value->savepoint_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->ordinal);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->release_on_success);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->rollback_on_failure);
}
static void UmiDataSavepointPlanArchiveRead(UmiArchiveReader *reader, UmiDataSavepointPlan *value)
{
    UmiArchiveReadText(reader, value->savepoint_id, sizeof(value->savepoint_id));
    value->ordinal = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->release_on_success = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->rollback_on_failure = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDataSavepointPlanArchiveValidate(const UmiDataSavepointPlan *value)
{
    return umi_data_savepoint_plan_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_data_savepoint_plan_archive_encode, umi_data_savepoint_plan_archive_decode,
    UmiDataSavepointPlan, UmiDataSavepointPlanArchiveSchema, UmiDataSavepointPlanArchiveBound, UmiDataSavepointPlanArchiveWrite, UmiDataSavepointPlanArchiveRead, UmiDataSavepointPlanArchiveValidate)

/* SDK callers choose staging storage independently of their thread stack.
 * Reuse this owner\'s exact schema, reader and validation before publishing
 * a destination; decoded values remain data, never execution authority. */
UMI_DEFINE_VALUE_ARCHIVE_WORKSPACE(umi_data_savepoint_plan_archive_decode_with_workspace, UmiDataSavepointPlan,
    UmiDataSavepointPlanArchiveSchema, UmiDataSavepointPlanArchiveBound, UmiDataSavepointPlanArchiveRead, UmiDataSavepointPlanArchiveValidate)

/* Restore this typed value only while its encoded state still matches the
 * caller's reviewed snapshot. The shared guard checks every encoded byte;
 * domain decoding and validation stay with this owner. */
UMI_DEFINE_CONDITIONAL_ARCHIVE(umi_data_savepoint_plan_archive_decode_if_unchanged, umi_data_savepoint_plan_archive_encode,
    UmiDataSavepointPlan, UmiDataSavepointPlanArchiveSchema, UmiDataSavepointPlanArchiveBound, UmiDataSavepointPlanArchiveRead, UmiDataSavepointPlanArchiveValidate)
