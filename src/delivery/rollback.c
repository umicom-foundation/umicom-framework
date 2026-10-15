/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/rollback.c
 *
 * PURPOSE:
 *   Implement and validate rollback requests between immutable installed generations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Rollback is an explicit operation with a reason and approval state rather than an ad-hoc file copy.
 */

#include "../base/conditional_archive_internal.h"
#include "umicom/delivery/rollback.h"
#include "../base/value_construction_internal.h"
#include "../base/value_archive_internal.h"
#include "delivery_internal.h"
#include <string.h>

/*
 * Initialise rollback plan from caller-provided values so later operations receive a known
 * state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_rollback_plan_init(UmiRollbackPlan *plan,
                                 uint64_t current_generation,
                                 uint64_t target_generation,
                                 const char *reason)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || reason == NULL || target_generation >= current_generation) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(plan, 0, sizeof(*plan));
    plan->current_generation = current_generation;
    plan->target_generation = target_generation;
    return umi_delivery_copy_text(plan->reason, sizeof(plan->reason), reason);
}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_rollback_plan_init_build_value(UmiRollbackPlan *plan,
                                 uint64_t current_generation,
                                 uint64_t target_generation,
                                 const char *reason)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || reason == NULL || target_generation >= current_generation) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(plan, 0, sizeof(*plan));
    plan->current_generation = current_generation;
    plan->target_generation = target_generation;
    return umi_delivery_copy_text(plan->reason, sizeof(plan->reason), reason);
}

UMI_DEFINE_STAGED_VALUE_INIT(umi_rollback_plan_init, UmiRollbackPlan, plan,
    (UmiRollbackPlan *plan, uint64_t current_generation, uint64_t target_generation, const char *reason),
    umi_rollback_plan_init_build_value(candidate, current_generation, target_generation, reason))


/*
 * Provide the rollback plan approve operation used by this module and its client
 * applications.
 */
UmiStatus umi_rollback_plan_approve(UmiRollbackPlan *plan)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    plan->approved = 1;
    return UMI_STATUS_OK;
}

/* Check that rollback plan satisfies its contract before another service relies on it. */
int umi_rollback_plan_valid(const UmiRollbackPlan *plan)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (plan == NULL) return 0;
    if (memchr(plan->reason, '\0', sizeof(plan->reason)) == NULL) return 0;

    return plan != NULL && plan->approved != 0 &&
           plan->target_generation < plan->current_generation;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRollbackPlanArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xc905e5321e96af81);
    schema = (schema ^ (uint64_t)sizeof(((UmiRollbackPlan *)0)->reason)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRollbackPlanArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U + sizeof(((UmiRollbackPlan *)0)->reason) - 1U +
        8U;
}
static void UmiRollbackPlanArchiveWrite(UmiArchiveWriter *writer, const UmiRollbackPlan *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->current_generation);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->target_generation);
    UmiArchiveWriteText(writer, value->reason, sizeof(value->reason));
    UmiArchiveWriteSigned(writer, (int64_t)value->approved);
}
static void UmiRollbackPlanArchiveRead(UmiArchiveReader *reader, UmiRollbackPlan *value)
{
    value->current_generation = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->target_generation = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    UmiArchiveReadText(reader, value->reason, sizeof(value->reason));
    value->approved = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiRollbackPlanArchiveValidate(const UmiRollbackPlan *value)
{
    return umi_rollback_plan_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rollback_plan_archive_encode, umi_rollback_plan_archive_decode,
    UmiRollbackPlan, UmiRollbackPlanArchiveSchema, UmiRollbackPlanArchiveBound, UmiRollbackPlanArchiveWrite, UmiRollbackPlanArchiveRead, UmiRollbackPlanArchiveValidate)

/* Guard restoration with the previously reviewed encoded value. Domain field
 * decoding and validation remain here; the shared helper owns temporary memory. */
UMI_DEFINE_CONDITIONAL_ARCHIVE(umi_rollback_plan_archive_decode_if_unchanged, umi_rollback_plan_archive_encode,
    UmiRollbackPlan, UmiRollbackPlanArchiveSchema, UmiRollbackPlanArchiveBound, UmiRollbackPlanArchiveRead, UmiRollbackPlanArchiveValidate)
