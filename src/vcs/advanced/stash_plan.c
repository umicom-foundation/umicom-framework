/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/stash_plan.c
 *
 * PURPOSE:
 *   Plan stash push/apply/pop/drop/branch operations with explicit conflict and index intent.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/stash_plan.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise vcs advanced stash plan from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_stash_plan_init(UmiVcsAdvancedStashPlan *plan)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) return;
    (void)memset(plan, 0, sizeof(*plan));
    plan->struct_size=(uint32_t)sizeof(*plan); plan->api_version=UMI_VCS_ADVANCED_API_VERSION;
    plan->safety=UMI_VCS_SAFETY_REVIEW;
}
/*
 * Check that vcs advanced stash plan satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_vcs_advanced_stash_plan_validate(const UmiVcsAdvancedStashPlan *plan)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(plan->stash_ref, '\0', sizeof(plan->stash_ref)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(plan->message, '\0', sizeof(plan->message)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(plan->branch_name, '\0', sizeof(plan->branch_name)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan==NULL || plan->struct_size<sizeof(*plan) || plan->api_version!=UMI_VCS_ADVANCED_API_VERSION ||
        plan->action>UMI_VCS_STASH_BRANCH) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (plan->action!=UMI_VCS_STASH_PUSH && !umi_vcs_advanced_text_present(plan->stash_ref))
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (plan->action==UMI_VCS_STASH_BRANCH && !umi_vcs_advanced_text_present(plan->branch_name))
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
/*
 * Provide the vcs advanced stash plan push operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_advanced_stash_plan_push(UmiVcsAdvancedStashPlan *plan, const char *message,
                                            int include_untracked, int keep_index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    plan->action=UMI_VCS_STASH_PUSH; plan->include_untracked=include_untracked!=0; plan->keep_index=keep_index!=0;
    return umi_vcs_advanced_copy_text(plan->message,sizeof(plan->message),message);
}
/*
 * Perform vcs advanced stash plan through the module contract so client applications do
 * not duplicate its policy.
 */
UmiStatus umi_vcs_advanced_stash_plan_apply(UmiVcsAdvancedStashPlan *plan, const char *stash_ref,
                                             int pop, int reinstate_index)
{
    UmiStatus s;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    plan->action=pop?UMI_VCS_STASH_POP:UMI_VCS_STASH_APPLY; plan->reinstate_index=reinstate_index!=0;
    s=umi_vcs_advanced_copy_text(plan->stash_ref,sizeof(plan->stash_ref),stash_ref);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (s!=UMI_STATUS_OK) return s;
    return umi_vcs_advanced_stash_plan_validate(plan);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiVcsAdvancedStashPlanArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xdc8c62be2335629f);
    schema = (schema ^ (uint64_t)sizeof(((UmiVcsAdvancedStashPlan *)0)->stash_ref)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiVcsAdvancedStashPlan *)0)->message)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiVcsAdvancedStashPlan *)0)->branch_name)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiVcsAdvancedStashPlanArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U + sizeof(((UmiVcsAdvancedStashPlan *)0)->stash_ref) - 1U +
        8U + sizeof(((UmiVcsAdvancedStashPlan *)0)->message) - 1U +
        8U + sizeof(((UmiVcsAdvancedStashPlan *)0)->branch_name) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiVcsAdvancedStashPlanArchiveWrite(UmiArchiveWriter *writer, const UmiVcsAdvancedStashPlan *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->api_version);
    UmiArchiveWriteSigned(writer, (int64_t)value->action);
    UmiArchiveWriteText(writer, value->stash_ref, sizeof(value->stash_ref));
    UmiArchiveWriteText(writer, value->message, sizeof(value->message));
    UmiArchiveWriteText(writer, value->branch_name, sizeof(value->branch_name));
    UmiArchiveWriteSigned(writer, (int64_t)value->include_untracked);
    UmiArchiveWriteSigned(writer, (int64_t)value->keep_index);
    UmiArchiveWriteSigned(writer, (int64_t)value->reinstate_index);
    UmiArchiveWriteSigned(writer, (int64_t)value->safety);
}
static void UmiVcsAdvancedStashPlanArchiveRead(UmiArchiveReader *reader, UmiVcsAdvancedStashPlan *value)
{
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->action = (UmiVcsAdvancedStashAction)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    UmiArchiveReadText(reader, value->stash_ref, sizeof(value->stash_ref));
    UmiArchiveReadText(reader, value->message, sizeof(value->message));
    UmiArchiveReadText(reader, value->branch_name, sizeof(value->branch_name));
    value->include_untracked = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->keep_index = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->reinstate_index = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->safety = (UmiVcsSafetyLevel)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiVcsAdvancedStashPlanArchiveValidate(const UmiVcsAdvancedStashPlan *value)
{
    return umi_vcs_advanced_stash_plan_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_vcs_advanced_stash_plan_archive_encode, umi_vcs_advanced_stash_plan_archive_decode,
    UmiVcsAdvancedStashPlan, UmiVcsAdvancedStashPlanArchiveSchema, UmiVcsAdvancedStashPlanArchiveBound, UmiVcsAdvancedStashPlanArchiveWrite, UmiVcsAdvancedStashPlanArchiveRead, UmiVcsAdvancedStashPlanArchiveValidate)
