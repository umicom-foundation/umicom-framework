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
#include "umicom/compiler/policy.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
/* Admit both ends of the optimisation enum and all sanitizer bits before applying policy. Keep the previous checks for review. The previous implementation is retained for review. */
#if 0
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
#endif
/* Admit the mutually exclusive address, thread and memory sanitizer groups before dialect selection. Retain the earlier bit and conflict validation for review. The previous implementation is retained for review. */
#if 0
UmiStatus umi_compiler_policy_validate(const UmiCompilerPolicy *policy,char *out_reason,size_t capacity)
{
    if (policy == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    const char *reason = "ok";
    UmiStatus status = UMI_STATUS_OK;
    const uint32_t known = UMI_COMPILER_SANITIZER_ADDRESS | UMI_COMPILER_SANITIZER_UNDEFINED |
        UMI_COMPILER_SANITIZER_THREAD | UMI_COMPILER_SANITIZER_MEMORY;
    if (policy->optimisation < UMI_COMPILER_OPT_NONE || policy->optimisation > UMI_COMPILER_OPT_AGGRESSIVE) {
        reason = "unknown optimisation level"; status = UMI_STATUS_INVALID_ARGUMENT;
    } else if ((policy->sanitizers & ~known) != 0U) {
        reason = "unknown sanitizer bits"; status = UMI_STATUS_INVALID_ARGUMENT;
    } else if ((policy->sanitizers & UMI_COMPILER_SANITIZER_THREAD) != 0U &&
               (policy->sanitizers & UMI_COMPILER_SANITIZER_ADDRESS) != 0U) {
        reason = "thread and address sanitizers cannot be combined"; status = UMI_STATUS_INVALID_STATE;
    }
    if (out_reason != NULL && capacity > 0U) (void)snprintf(out_reason,capacity,"%s",reason);
    return status;
}
#endif
UmiStatus umi_compiler_policy_validate(const UmiCompilerPolicy *policy,char *out_reason,size_t capacity)
{
    if (policy == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    const char *reason = "ok";
    UmiStatus status = UMI_STATUS_OK;
    const uint32_t known = UMI_COMPILER_SANITIZER_ADDRESS | UMI_COMPILER_SANITIZER_UNDEFINED |
        UMI_COMPILER_SANITIZER_THREAD | UMI_COMPILER_SANITIZER_MEMORY;
    if (policy->optimisation < UMI_COMPILER_OPT_NONE || policy->optimisation > UMI_COMPILER_OPT_AGGRESSIVE) {
        reason = "unknown optimisation level"; status = UMI_STATUS_INVALID_ARGUMENT;
    } else if ((policy->sanitizers & ~known) != 0U) {
        reason = "unknown sanitizer bits"; status = UMI_STATUS_INVALID_ARGUMENT;
    } else if ((policy->sanitizers & UMI_COMPILER_SANITIZER_THREAD) != 0U &&
               (policy->sanitizers & UMI_COMPILER_SANITIZER_ADDRESS) != 0U) {
        reason = "thread and address sanitizers cannot be combined"; status = UMI_STATUS_INVALID_STATE;
    }
    else if ((policy->sanitizers & UMI_COMPILER_SANITIZER_MEMORY) != 0U &&
             (policy->sanitizers & (UMI_COMPILER_SANITIZER_ADDRESS | UMI_COMPILER_SANITIZER_THREAD)) != 0U) {
        reason = "memory sanitizer cannot be combined with address or thread sanitizer"; status = UMI_STATUS_INVALID_STATE;
    }
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
/* Apply the existing option mappings to temporary plan storage and publish only after every append succeeds. Retain the former live mutation for review. The previous implementation is retained for review. */
#if 0
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
#endif
/* Use explicit GNU and MSVC spellings and reject unsupported policy intents before composition. Preserve the older generic mapping, including its ineffective reproducibility approximation, for review. The previous implementation is retained for review. */
#if 0
static UmiStatus UmiCompilerPolicyAppendCandidate(const UmiCompilerPolicy *policy,UmiCompilerFamily family,UmiCompilerPlan *plan)
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
#endif

/* This matrix admits documented option dialects, not installed SDK capability.
 * Keep unsupported families/actions explicit instead of borrowing GNU switches. */
UmiStatus umi_compiler_policy_validate_for_plan(const UmiCompilerPolicy *policy,UmiCompilerFamily family,
    const UmiCompilerPlan *plan,char *out_reason,size_t capacity)
{
    if (policy == NULL || plan == NULL || family <= UMI_COMPILER_FAMILY_UNKNOWN || family > UMI_COMPILER_FAMILY_UMICC)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_compiler_policy_validate(policy,out_reason,capacity);
    if (status != UMI_STATUS_OK) return status;
    const char *reason = "policy option dialect accepted; toolchain and target remain unverified";
    if (!umi_compiler_plan_storage_valid(plan)) {
        reason = "compiler plan storage is malformed"; status = UMI_STATUS_INVALID_STATE;
    } else if ((plan->language != UMI_COMPILER_LANGUAGE_C && plan->language != UMI_COMPILER_LANGUAGE_CPP) ||
               plan->action != UMI_COMPILER_ACTION_COMPILE) {
        reason = "policy mapping currently supports C and C++ compile actions"; status = UMI_STATUS_NOT_IMPLEMENTED;
    } else if (family != UMI_COMPILER_FAMILY_GCC && family != UMI_COMPILER_FAMILY_CLANG && family != UMI_COMPILER_FAMILY_MSVC) {
        reason = "compiler family has no policy option mapping"; status = UMI_STATUS_NOT_IMPLEMENTED;
    } else if (policy->reproducible) {
        /* An identity path map or one compiler flag cannot fulfill this intent.
         * A future reproducibility service must own paths, environment and tools. */
        reason = "reproducible output requires an explicit path, environment and toolchain contract";
        status = UMI_STATUS_NOT_IMPLEMENTED;
    } else if (family == UMI_COMPILER_FAMILY_GCC && (policy->sanitizers & UMI_COMPILER_SANITIZER_MEMORY) != 0U) {
        reason = "memory sanitizer is not mapped for GCC"; status = UMI_STATUS_NOT_IMPLEMENTED;
    } else if (family == UMI_COMPILER_FAMILY_MSVC && (policy->sanitizers & ~(uint32_t)UMI_COMPILER_SANITIZER_ADDRESS) != 0U) {
        reason = "only address sanitizer is mapped for MSVC"; status = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (out_reason != NULL && capacity > 0U) (void)snprintf(out_reason,capacity,"%s",reason);
    return status;
}

static UmiStatus UmiCompilerPolicyAppendCandidate(const UmiCompilerPolicy *policy,UmiCompilerFamily family,UmiCompilerPlan *plan)
{
    static const char *const gnu_optimisation[] = {"-O0","-Og","-Os","-O2","-O3"};
    static const char *const msvc_optimisation[] = {"/Od","/Od","/O1","/O2","/Ox"};
    UmiStatus status = add(plan,gnu_optimisation[policy->optimisation],msvc_optimisation[policy->optimisation],family);
    if (status == UMI_STATUS_OK && policy->debug_information) status = add(plan,"-g","/Zi",family);
    if (status == UMI_STATUS_OK && policy->warnings_as_errors) status = add(plan,"-Werror","/WX",family);
    if (status == UMI_STATUS_OK && policy->link_time_optimisation) status = add(plan,"-flto","/GL",family);
    if (status == UMI_STATUS_OK && policy->stack_protection) status = add(plan,"-fstack-protector-strong","/GS",family);
    if (status == UMI_STATUS_OK && policy->control_flow_protection) status = add(plan,"-fcf-protection=full","/guard:cf",family);
    if (status == UMI_STATUS_OK && (policy->sanitizers & UMI_COMPILER_SANITIZER_ADDRESS) != 0U)
        status = add(plan,"-fsanitize=address","/fsanitize=address",family);
    if (status == UMI_STATUS_OK && (policy->sanitizers & UMI_COMPILER_SANITIZER_UNDEFINED) != 0U)
        status = umi_compiler_plan_add_argument(plan,"-fsanitize=undefined");
    if (status == UMI_STATUS_OK && (policy->sanitizers & UMI_COMPILER_SANITIZER_THREAD) != 0U)
        status = umi_compiler_plan_add_argument(plan,"-fsanitize=thread");
    if (status == UMI_STATUS_OK && (policy->sanitizers & UMI_COMPILER_SANITIZER_MEMORY) != 0U)
        status = umi_compiler_plan_add_argument(plan,"-fsanitize=memory");
    return status;
}

/* Run the family, language and action admission matrix before allocating a candidate. The prior atomic wrapper is retained for review; its preservation guarantee remains. The previous implementation is retained for review. */
#if 0
UmiStatus umi_compiler_policy_append_arguments(const UmiCompilerPolicy *policy,UmiCompilerFamily family,UmiCompilerPlan *plan)
{
    if (plan == NULL || policy == NULL ||
        family <= UMI_COMPILER_FAMILY_UNKNOWN || family > UMI_COMPILER_FAMILY_UMICC)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_compiler_policy_validate(policy,NULL,0U);
    if (status != UMI_STATUS_OK) return status;
    if (!umi_compiler_plan_storage_valid(plan)) return UMI_STATUS_INVALID_STATE;
    /* Policy composition can append several arguments. A late capacity/revision
     * failure must never leave a partially applied policy in the caller's plan. */
    UmiCompilerPlan *candidate = malloc(sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    (void)memcpy(candidate,plan,sizeof(*candidate));
    status = UmiCompilerPolicyAppendCandidate(policy,family,candidate);
    if (status == UMI_STATUS_OK) {
        (void)memcpy(plan,candidate,sizeof(*plan));
        /* Cached pointers belong to the receiving plan, not temporary storage. */
        (void)umi_compiler_plan_arguments(plan);
    }
    free(candidate);
    return status;
}
#endif
UmiStatus umi_compiler_policy_append_arguments(const UmiCompilerPolicy *policy,UmiCompilerFamily family,UmiCompilerPlan *plan)
{
    if (plan == NULL || policy == NULL ||
        family <= UMI_COMPILER_FAMILY_UNKNOWN || family > UMI_COMPILER_FAMILY_UMICC)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_compiler_policy_validate_for_plan(policy,family,plan,NULL,0U);
    if (status != UMI_STATUS_OK) return status;
    if (!umi_compiler_plan_storage_valid(plan)) return UMI_STATUS_INVALID_STATE;
    /* Policy composition can append several arguments. A late capacity/revision
     * failure must never leave a partially applied policy in the caller's plan. */
    UmiCompilerPlan *candidate = malloc(sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    (void)memcpy(candidate,plan,sizeof(*candidate));
    status = UmiCompilerPolicyAppendCandidate(policy,family,candidate);
    if (status == UMI_STATUS_OK) {
        (void)memcpy(plan,candidate,sizeof(*plan));
        /* Cached pointers belong to the receiving plan, not temporary storage. */
        (void)umi_compiler_plan_arguments(plan);
    }
    free(candidate);
    return status;
}
