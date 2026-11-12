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

/* Keep the public C symbol names when this header is included directly by a
 * C++ client. Includes remain outside the linkage block; layouts are unchanged. */
#ifdef __cplusplus
extern "C" {
#endif
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

/** Append preserves the complete plan on any refusal, including a late argument
 * capacity/revision failure or allocation failure. On success cached argument
 * pointers are rebased into the destination. Callers serialize access.
 *
 * Validation admits named optimisation levels and sanitizer bits; it is not a
 * compiler capability probe. Optional diagnostic storage must not overlap inputs. */
/** Validate the mapping before publication. Supports C/C++ COMPILE plans with
 * GCC, Clang or MSVC dialects. Other actions, languages and families return
 * NOT_IMPLEMENTED without modifying inputs. GCC memory sanitizer and MSVC
 * non-address sanitizers are refused; address/thread/memory are mutually exclusive.
 *
 * Reproducible intent currently returns NOT_IMPLEMENTED: the previous identity
 * path map and /Brepro approximation did not establish a reproducibility contract.
 * The release factory retains that intent, so clients must explicitly choose a
 * policy whose supported behavior matches their qualification.
 *
 * Accepted mappings still need target, compiler-version, linker and runtime
 * qualification. GCC/Clang control-flow protection is target dependent; MSVC CFG
 * also requires linker support. This service performs no driver probing. */
UmiStatus umi_compiler_policy_validate_for_plan(const UmiCompilerPolicy *policy,UmiCompilerFamily family,
    const UmiCompilerPlan *plan,char *out_reason,size_t capacity);
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif
