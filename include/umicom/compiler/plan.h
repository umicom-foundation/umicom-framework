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
void umi_compiler_plan_init(UmiCompilerPlan *plan);
UmiStatus umi_compiler_plan_add_argument(UmiCompilerPlan *plan,const char *argument);
const char *const *umi_compiler_plan_arguments(UmiCompilerPlan *plan);
UmiStatus umi_compiler_plan_validate(const UmiCompilerPlan *plan,char *out_reason,size_t capacity);
#endif
