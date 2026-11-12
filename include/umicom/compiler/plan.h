/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/plan.h
 *
 * PURPOSE:
 *   Publish the public plan contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_PLAN_H
#define UMICOM_COMPILER_PLAN_H
#include "umicom/compiler/request.h"

/* Keep the public C symbol names when this header is included directly by a
 * C++ client. Includes remain outside the linkage block; layouts are unchanged. */
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the compiler plan data shared with callers of this public contract.
 */
typedef struct UmiCompilerPlan {
    char provider_id[UMI_COMPILER_ID_CAPACITY];
    char executable[UMI_COMPILER_PATH_CAPACITY];
    char working_directory[UMI_COMPILER_PATH_CAPACITY];
    char arguments[UMI_COMPILER_MAX_ARGUMENTS][UMI_COMPILER_TEXT_CAPACITY];
    const char *argument_view[UMI_COMPILER_MAX_ARGUMENTS];
    size_t argument_count;
    UmiCompilerLanguage language;
    UmiCompilerAction action;
    uint64_t revision;
} UmiCompilerPlan;
/**
 * Initialise compiler plan from caller-provided values so later operations receive a known
 * state.
 */
void umi_compiler_plan_init(UmiCompilerPlan *plan);
/**
 * Provide the compiler plan add argument operation used by this module and its client
 * applications.
 */
UmiStatus umi_compiler_plan_add_argument(UmiCompilerPlan *plan,const char *argument);
/**
 * Provide the compiler plan arguments operation used by this module and its client
 * applications.
 */
const char *const *umi_compiler_plan_arguments(UmiCompilerPlan *plan);
/**
 * Check that compiler plan satisfies its contract before another service relies on it.
 */
UmiStatus umi_compiler_plan_validate(const UmiCompilerPlan *plan,char *out_reason,size_t capacity);
/**
 * Validate inline text and active argument extent without reading argument_view.
 * Empty metadata is permitted for an initialized draft. Append refusals leave
 * all bytes unchanged, including unused slots and cached pointers. Revision
 * exhaustion refuses a new argument but does not prevent reading the plan.
 * After copying a plan by value, call plan_arguments to rebase its borrowed
 * pointers. Consume exactly argument_count entries: no NULL sentinel is promised.
 * Returned pointers require the plan lifetime and serialized access. A diagnostic
 * buffer passed to plan_validate must not overlap the plan.
 */
bool umi_compiler_plan_storage_valid(const UmiCompilerPlan *plan);
/* Worst-case JSON storage, including control-byte escaping and metadata syntax.
 * This is an output allocation bound, not a command-line or process limit. */
#define UMI_COMPILER_PLAN_JSON_CAPACITY (1024U + 6U * (UMI_COMPILER_ID_CAPACITY + \
    2U * UMI_COMPILER_PATH_CAPACITY + UMI_COMPILER_MAX_ARGUMENTS * UMI_COMPILER_TEXT_CAPACITY))
/** Serialize a complete, bounded plan as schema-version-one UTF-8 JSON.
 * The document describes a preview only: executed and toolchain_verified are false.
 * Reuses Protocol's JSON writer; neither shell quoting nor process execution occurs.
 * Every failure leaves output bytes unchanged. Capacity includes the final NUL.
 * Output must not overlap plan; callers serialize access for the whole operation.
 * Invalid UTF-8 is refused. Cached argument_view is neither read nor changed. */
UmiStatus umi_compiler_plan_json(const UmiCompilerPlan *plan,char *output,size_t capacity);
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif
