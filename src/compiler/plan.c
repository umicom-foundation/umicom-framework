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
void umi_compiler_plan_init(UmiCompilerPlan *plan) { if (plan != NULL) { (void)memset(plan,0,sizeof(*plan)); plan->revision = 1U; } }
UmiStatus umi_compiler_plan_add_argument(UmiCompilerPlan *plan,const char *argument)
{
    int length;
    if (plan == NULL || argument == NULL || argument[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (plan->argument_count >= UMI_COMPILER_MAX_ARGUMENTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    length = snprintf(plan->arguments[plan->argument_count],sizeof(plan->arguments[0]),"%s",argument);
    if (length < 0 || (size_t)length >= sizeof(plan->arguments[0])) return UMI_STATUS_CAPACITY_EXCEEDED;
    plan->argument_view[plan->argument_count] = plan->arguments[plan->argument_count]; plan->argument_count += 1U; plan->revision += 1U; return UMI_STATUS_OK;
}
const char *const *umi_compiler_plan_arguments(UmiCompilerPlan *plan) { size_t index; if (plan == NULL) return NULL; for (index = 0U; index < plan->argument_count; ++index) plan->argument_view[index] = plan->arguments[index]; return plan->argument_view; }
UmiStatus umi_compiler_plan_validate(const UmiCompilerPlan *plan,char *out_reason,size_t capacity)
{
    int length;
    if (plan == NULL || out_reason == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (plan->provider_id[0] == '\0' || plan->executable[0] == '\0' || plan->argument_count == 0U) { length = snprintf(out_reason,capacity,"Provider, executable and arguments are required"); return length < 0 || (size_t)length >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_INVALID_STATE; }
    length = snprintf(out_reason,capacity,"Compiler plan is valid"); return length < 0 || (size_t)length >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}
