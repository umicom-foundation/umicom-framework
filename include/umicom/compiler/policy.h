/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/policy.h
 *
 * PURPOSE:
 *   Publish the public policy contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_POLICY_H
#define UMICOM_COMPILER_POLICY_H
#include "umicom/compiler/plan.h"
/**
 * List the named compiler optimisation values accepted by this public contract.
 */
typedef enum UmiCompilerOptimisation { UMI_COMPILER_OPT_NONE = 0, UMI_COMPILER_OPT_DEBUG, UMI_COMPILER_OPT_SIZE, UMI_COMPILER_OPT_SPEED, UMI_COMPILER_OPT_AGGRESSIVE } UmiCompilerOptimisation;
/**
 * List the named compiler sanitizer values accepted by this public contract.
 */
typedef enum UmiCompilerSanitizer { UMI_COMPILER_SANITIZER_NONE = 0, UMI_COMPILER_SANITIZER_ADDRESS = 1, UMI_COMPILER_SANITIZER_UNDEFINED = 2, UMI_COMPILER_SANITIZER_THREAD = 4, UMI_COMPILER_SANITIZER_MEMORY = 8 } UmiCompilerSanitizer;
/**
 * Represent the compiler policy data shared with callers of this public contract.
 */
typedef struct UmiCompilerPolicy { UmiCompilerOptimisation optimisation; uint32_t sanitizers; bool debug_information; bool link_time_optimisation; bool warnings_as_errors; bool stack_protection; bool control_flow_protection; bool reproducible; } UmiCompilerPolicy;
/**
 * Provide the compiler policy debug operation used by this module and its client
 * applications.
 */
UmiCompilerPolicy umi_compiler_policy_debug(void);
/**
 * Release or reset state held by compiler policy so the same storage can be reused safely.
 */
UmiCompilerPolicy umi_compiler_policy_release(void);
/**
 * Check that compiler policy satisfies its contract before another service relies on it.
 */
UmiStatus umi_compiler_policy_validate(const UmiCompilerPolicy *policy,char *out_reason,size_t capacity);
/**
 * Provide the compiler policy append arguments operation used by this module and its
 * client applications.
 */
UmiStatus umi_compiler_policy_append_arguments(const UmiCompilerPolicy *policy,UmiCompilerFamily family,UmiCompilerPlan *plan);
#endif
