/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/upgrade_plan.c
 *
 * PURPOSE:
 *   Plan product upgrades with explicit compatibility, backup and rollback.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/delivery/upgrade_plan.h"
#include "../base/value_archive_internal.h"
#include "delivery_internal.h"
#include <string.h>

/*
 * Initialise upgrade plan from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_upgrade_plan_init(UmiUpgradePlan *plan,
                                    const char *current_version,
                                    const char *target_version,
                                    uint64_t current_generation,
                                    uint64_t target_generation,
                                    int compatible)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || current_version == NULL || target_version == NULL ||
        current_generation == 0U || target_generation == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(plan, 0, sizeof(*plan));
    status = umi_delivery_copy_text(plan->current_version,
                                    sizeof(plan->current_version), current_version);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(plan->target_version,
                                    sizeof(plan->target_version), target_version);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    plan->current_generation = current_generation;
    plan->target_generation = target_generation;
    plan->compatible = compatible != 0;
    plan->backup_required = 1;
    plan->rollback_supported = 1;
    return UMI_STATUS_OK;
}

/*
 * Provide the upgrade plan authorise operation used by this module and its client
 * applications.
 */
UmiStatus umi_upgrade_plan_authorise(UmiUpgradePlan *plan,
                                         int backup_available)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!plan->compatible || plan->target_generation <= plan->current_generation) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (plan->backup_required && !backup_available) {
        return UMI_STATUS_UNAVAILABLE;
    }
    plan->authorised = 1;
    return UMI_STATUS_OK;
}

/* Check that upgrade plan satisfies its contract before another service relies on it. */
UmiStatus umi_upgrade_plan_validate(const UmiUpgradePlan *plan)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(plan->current_version, '\0', sizeof(plan->current_version)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(plan->target_version, '\0', sizeof(plan->target_version)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (plan->current_version[0] == '\0' || plan->target_version[0] == '\0' ||
        strcmp(plan->current_version, plan->target_version) == 0 ||
        plan->target_generation <= plan->current_generation ||
        !plan->compatible || !plan->rollback_supported || !plan->authorised) {
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the upgrade plan rollback generation operation used by this module and its
 * client applications.
 */
uint64_t umi_upgrade_plan_rollback_generation(const UmiUpgradePlan *plan)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || !plan->rollback_supported) return 0U;
    return plan->current_generation;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUpgradePlanArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xa4e915ecfa545da5);
    schema = (schema ^ (uint64_t)sizeof(((UmiUpgradePlan *)0)->current_version)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUpgradePlan *)0)->target_version)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUpgradePlanArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUpgradePlan *)0)->current_version) - 1U +
        8U + sizeof(((UmiUpgradePlan *)0)->target_version) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiUpgradePlanArchiveWrite(UmiArchiveWriter *writer, const UmiUpgradePlan *value)
{
    UmiArchiveWriteText(writer, value->current_version, sizeof(value->current_version));
    UmiArchiveWriteText(writer, value->target_version, sizeof(value->target_version));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->current_generation);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->target_generation);
    UmiArchiveWriteSigned(writer, (int64_t)value->compatible);
    UmiArchiveWriteSigned(writer, (int64_t)value->backup_required);
    UmiArchiveWriteSigned(writer, (int64_t)value->rollback_supported);
    UmiArchiveWriteSigned(writer, (int64_t)value->authorised);
}
static void UmiUpgradePlanArchiveRead(UmiArchiveReader *reader, UmiUpgradePlan *value)
{
    UmiArchiveReadText(reader, value->current_version, sizeof(value->current_version));
    UmiArchiveReadText(reader, value->target_version, sizeof(value->target_version));
    value->current_generation = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->target_generation = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->compatible = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->backup_required = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->rollback_supported = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->authorised = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiUpgradePlanArchiveValidate(const UmiUpgradePlan *value)
{
    return umi_upgrade_plan_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_upgrade_plan_archive_encode, umi_upgrade_plan_archive_decode,
    UmiUpgradePlan, UmiUpgradePlanArchiveSchema, UmiUpgradePlanArchiveBound, UmiUpgradePlanArchiveWrite, UmiUpgradePlanArchiveRead, UmiUpgradePlanArchiveValidate)
