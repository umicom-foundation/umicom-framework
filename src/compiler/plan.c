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
/* Umicom Framework | Compiler plans | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/plan.h"
#include <stdio.h>
#include <string.h>
/*
 * Initialise compiler plan from caller-provided values so later operations receive a known
 * state.
 */
void umi_compiler_plan_init(UmiCompilerPlan *plan) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (plan != NULL) { (void)memset(plan,0,sizeof(*plan)); plan->revision = 1U; } }
/*
 * Provide the compiler plan add argument operation used by this module and its client
 * applications.
 */
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
/*
 * Provide the compiler plan arguments operation used by this module and its client
 * applications.
 */
const char *const *umi_compiler_plan_arguments(UmiCompilerPlan *plan) { size_t index; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (plan == NULL) return NULL; /* Visit each bounded item once so every record receives the same rule. */ for (index = 0U; index < plan->argument_count; ++index) plan->argument_view[index] = plan->arguments[index]; return plan->argument_view; }
/* Check that compiler plan satisfies its contract before another service relies on it. */
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
