/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/policy.c
 *
 * PURPOSE:
 *   Implement the policy behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Compiler build policy | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/policy.h"
#include <stdio.h>
#include <string.h>

/*
 * Provide the compiler policy debug operation used by this module and its client
 * applications.
 */
UmiCompilerPolicy umi_compiler_policy_debug(void)
{ UmiCompilerPolicy policy; (void)memset(&policy,0,sizeof(policy)); policy.optimisation = UMI_COMPILER_OPT_DEBUG; policy.debug_information = true; policy.stack_protection = true; return policy; }
/* Release or reset state held by compiler policy so the same storage can be reused safely. */
UmiCompilerPolicy umi_compiler_policy_release(void)
{ UmiCompilerPolicy policy; (void)memset(&policy,0,sizeof(policy)); policy.optimisation = UMI_COMPILER_OPT_SPEED; policy.link_time_optimisation = true; policy.stack_protection = true; policy.control_flow_protection = true; policy.reproducible = true; return policy; }
/* Check that compiler policy satisfies its contract before another service relies on it. */
UmiStatus umi_compiler_policy_validate(const UmiCompilerPolicy *policy,char *out_reason,size_t capacity)
{
    const char *reason = "ok"; UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (policy->optimisation > UMI_COMPILER_OPT_AGGRESSIVE) { reason = "unknown optimisation level"; status = UMI_STATUS_INVALID_ARGUMENT; }
    else /* Preserve the original failure result so the caller can respond to the correct cause. */ if ((policy->sanitizers & UMI_COMPILER_SANITIZER_THREAD) != 0U && (policy->sanitizers & UMI_COMPILER_SANITIZER_ADDRESS) != 0U) { reason = "thread and address sanitizers cannot be combined"; status = UMI_STATUS_INVALID_STATE; }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_reason != NULL && capacity > 0U) (void)snprintf(out_reason,capacity,"%s",reason);
    return status;
}
/* Provide the add operation used by this module and its client applications. */
static UmiStatus add(UmiCompilerPlan *plan,const char *gcc_value,const char *msvc_value,UmiCompilerFamily family)
{ return umi_compiler_plan_add_argument(plan,family == UMI_COMPILER_FAMILY_MSVC ? msvc_value : gcc_value); }
/*
 * Provide the compiler policy append arguments operation used by this module and its
 * client applications.
 */
UmiStatus umi_compiler_policy_append_arguments(const UmiCompilerPolicy *policy,UmiCompilerFamily family,UmiCompilerPlan *plan)
{
    UmiStatus status; char reason[UMI_COMPILER_TEXT_CAPACITY];
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_compiler_policy_validate(policy,reason,sizeof(reason));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Select the behaviour associated with the requested command or state value. */
    switch (policy->optimisation) { case UMI_COMPILER_OPT_NONE: status = add(plan,"-O0","/Od",family); break; case UMI_COMPILER_OPT_DEBUG: status = add(plan,"-Og","/Od",family); break; case UMI_COMPILER_OPT_SIZE: status = add(plan,"-Os","/O1",family); break; case UMI_COMPILER_OPT_SPEED: status = add(plan,"-O2","/O2",family); break; case UMI_COMPILER_OPT_AGGRESSIVE: status = add(plan,"-O3","/Ox",family); break; default: return UMI_STATUS_INVALID_ARGUMENT; }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && policy->debug_information) status = add(plan,"-g","/Zi",family);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && policy->warnings_as_errors) status = add(plan,"-Werror","/WX",family);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && policy->link_time_optimisation) status = add(plan,"-flto","/GL",family);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && policy->stack_protection) status = add(plan,"-fstack-protector-strong","/GS",family);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && policy->control_flow_protection && family != UMI_COMPILER_FAMILY_MSVC) status = umi_compiler_plan_add_argument(plan,"-fcf-protection=full");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && policy->reproducible) status = add(plan,"-ffile-prefix-map=.=.","/Brepro",family);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && (policy->sanitizers & UMI_COMPILER_SANITIZER_ADDRESS) != 0U) status = umi_compiler_plan_add_argument(plan,"-fsanitize=address");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && (policy->sanitizers & UMI_COMPILER_SANITIZER_UNDEFINED) != 0U) status = umi_compiler_plan_add_argument(plan,"-fsanitize=undefined");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && (policy->sanitizers & UMI_COMPILER_SANITIZER_THREAD) != 0U) status = umi_compiler_plan_add_argument(plan,"-fsanitize=thread");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && (policy->sanitizers & UMI_COMPILER_SANITIZER_MEMORY) != 0U) status = umi_compiler_plan_add_argument(plan,"-fsanitize=memory");
    return status;
}
