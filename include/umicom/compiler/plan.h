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
#endif
