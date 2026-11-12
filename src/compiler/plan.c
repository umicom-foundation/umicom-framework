/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/plan.c
 *
 * PURPOSE:
 *   Implement the plan behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/plan.h"
#include <stdio.h>
#include <string.h>
#include "catalogue_validation.h"
#include "umicom/protocol/json_writer.h"
#include <stdlib.h>

/* argument_view is derived, borrowed storage. Copies may contain stale pointer
 * values; do not dereference them during validation. Rebuild them before use. */
bool umi_compiler_plan_storage_valid(const UmiCompilerPlan *plan)
{
    if (plan == NULL || plan->argument_count > UMI_COMPILER_MAX_ARGUMENTS ||
        !UmiCompilerCatalogueText(plan->provider_id,sizeof(plan->provider_id),false) ||
        !UmiCompilerCatalogueText(plan->executable,sizeof(plan->executable),false) ||
        !UmiCompilerCatalogueText(plan->working_directory,sizeof(plan->working_directory),false)) return false;
    for (size_t index = 0U; index < plan->argument_count; ++index)
        if (!UmiCompilerCatalogueText(plan->arguments[index],sizeof(plan->arguments[index]),true)) return false;
    return true;
}

/*
 * Initialise compiler plan from caller-provided values so later operations receive a known
 * state.
 */
void umi_compiler_plan_init(UmiCompilerPlan *plan) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (plan != NULL) { (void)memset(plan,0,sizeof(*plan)); plan->revision = 1U; } }
/*
 * Provide the compiler plan add argument operation used by this module and its client
 * applications.
 */
/* Refuse invalid storage, truncation and exhausted revision before publication; rebuild derived argument pointers in the receiving owner. The previous implementation is retained for review. */
#if 0
UmiStatus umi_compiler_plan_add_argument(UmiCompilerPlan *plan,const char *argument)
{
    int length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || argument == NULL || argument[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (plan->argument_count >= UMI_COMPILER_MAX_ARGUMENTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    length = snprintf(plan->arguments[plan->argument_count],sizeof(plan->arguments[0]),"%s",argument);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length < 0 || (size_t)length >= sizeof(plan->arguments[0])) return UMI_STATUS_CAPACITY_EXCEEDED;
    plan->argument_view[plan->argument_count] = plan->arguments[plan->argument_count]; plan->argument_count += 1U; plan->revision += 1U; return UMI_STATUS_OK;
}
#endif
UmiStatus umi_compiler_plan_add_argument(UmiCompilerPlan *plan,const char *argument)
{
    if (plan == NULL || argument == NULL || argument[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_compiler_plan_storage_valid(plan)) return UMI_STATUS_INVALID_STATE;
    if (plan->argument_count == UMI_COMPILER_MAX_ARGUMENTS || plan->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (!UmiCompilerCatalogueText(argument,sizeof(plan->arguments[0]),true)) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memmove(plan->arguments[plan->argument_count],argument,strlen(argument) + 1U);
    plan->argument_count += 1U;
    plan->revision += 1U;
    /* Rebase all active views, including plans copied by value before this append. */
    for (size_t index = 0U; index < plan->argument_count; ++index)
        plan->argument_view[index] = plan->arguments[index];
    return UMI_STATUS_OK;
}
/*
 * Provide the compiler plan arguments operation used by this module and its client
 * applications.
 */
/* Bound active argument storage before rebuilding borrowed pointers; a malformed owner remains unchanged. The previous implementation is retained for review. */
#if 0
const char *const *umi_compiler_plan_arguments(UmiCompilerPlan *plan) { size_t index; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (plan == NULL) return NULL; /* Visit each bounded item once so every record receives the same rule. */ for (index = 0U; index < plan->argument_count; ++index) plan->argument_view[index] = plan->arguments[index]; return plan->argument_view; }
#endif
const char *const *umi_compiler_plan_arguments(UmiCompilerPlan *plan)
{
    if (!umi_compiler_plan_storage_valid(plan)) return NULL;
    for (size_t index = 0U; index < plan->argument_count; ++index)
        plan->argument_view[index] = plan->arguments[index];
    return plan->argument_view;
}
/* Check that compiler plan satisfies its contract before another service relies on it. */
/* Distinguish bounded draft storage from the existing provider/executable/arguments requirement. The previous implementation is retained for review. */
#if 0
UmiStatus umi_compiler_plan_validate(const UmiCompilerPlan *plan,char *out_reason,size_t capacity)
{
    int length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || out_reason == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (plan->provider_id[0] == '\0' || plan->executable[0] == '\0' || plan->argument_count == 0U) { length = snprintf(out_reason,capacity,"Provider, executable and arguments are required"); return length < 0 || (size_t)length >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_INVALID_STATE; }
    length = snprintf(out_reason,capacity,"Compiler plan is valid"); return length < 0 || (size_t)length >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}
#endif
UmiStatus umi_compiler_plan_validate(const UmiCompilerPlan *plan,char *out_reason,size_t capacity)
{
    if (plan == NULL || out_reason == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    const char *reason = "Compiler plan is valid";
    UmiStatus status = UMI_STATUS_OK;
    if (!umi_compiler_plan_storage_valid(plan)) {
        reason = "Compiler plan storage is malformed"; status = UMI_STATUS_INVALID_STATE;
    } else if (plan->provider_id[0] == '\0' || plan->executable[0] == '\0' || plan->argument_count == 0U) {
        reason = "Provider, executable and arguments are required"; status = UMI_STATUS_INVALID_STATE;
    }
    const int length = snprintf(out_reason,capacity,"%s",reason);
    return length < 0 || (size_t)length >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : status;
}

/* Machine-readable presentation belongs with the shared plan so Studio and CLI
 * clients cannot disagree about escaping, argument order or execution evidence. */
UmiStatus umi_compiler_plan_json(const UmiCompilerPlan *plan,char *output,size_t capacity)
{
    if (plan == NULL || output == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    char reason[128];
    UmiStatus status = umi_compiler_plan_validate(plan,reason,sizeof(reason));
    if (status != UMI_STATUS_OK) return status;
    if (plan->language <= UMI_COMPILER_LANGUAGE_UNKNOWN || plan->language > UMI_COMPILER_LANGUAGE_UAI ||
        plan->action < UMI_COMPILER_ACTION_COMPILE || plan->action > UMI_COMPILER_ACTION_CHECK)
        return UMI_STATUS_INVALID_STATE;
    const size_t bound = capacity < UMI_COMPILER_PLAN_JSON_CAPACITY ? capacity : UMI_COMPILER_PLAN_JSON_CAPACITY;
    char *candidate = malloc(bound);
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiJsonWriter writer;
    UmiJsonWriterInit(&writer,candidate,bound);
    (void)UmiJsonWriterRaw(&writer,"{\"schema_version\":1,\"executed\":false,\"toolchain_verified\":false,\"provider\":");
    (void)UmiJsonWriterString(&writer,plan->provider_id);
    (void)UmiJsonWriterRaw(&writer,",\"executable\":");
    (void)UmiJsonWriterString(&writer,plan->executable);
    (void)UmiJsonWriterRaw(&writer,",\"working_directory\":");
    (void)UmiJsonWriterString(&writer,plan->working_directory);
    (void)UmiJsonWriterRaw(&writer,",\"language\":");
    (void)UmiJsonWriterUInt64(&writer,(uint64_t)plan->language);
    (void)UmiJsonWriterRaw(&writer,",\"action\":");
    (void)UmiJsonWriterUInt64(&writer,(uint64_t)plan->action);
    (void)UmiJsonWriterRaw(&writer,",\"arguments\":[");
    for (size_t index = 0U; index < plan->argument_count; ++index) {
        if (index != 0U) (void)UmiJsonWriterRaw(&writer,",");
        (void)UmiJsonWriterString(&writer,plan->arguments[index]);
    }
    (void)UmiJsonWriterRaw(&writer,"]}\n");
    status = writer.status;
    if (status == UMI_STATUS_OK) (void)memcpy(output,candidate,writer.length + 1U);
    free(candidate);
    return status;
}
