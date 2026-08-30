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
typedef enum UmiCompilerOptimisation { UMI_COMPILER_OPT_NONE = 0, UMI_COMPILER_OPT_DEBUG, UMI_COMPILER_OPT_SIZE, UMI_COMPILER_OPT_SPEED, UMI_COMPILER_OPT_AGGRESSIVE } UmiCompilerOptimisation;
typedef enum UmiCompilerSanitizer { UMI_COMPILER_SANITIZER_NONE = 0, UMI_COMPILER_SANITIZER_ADDRESS = 1, UMI_COMPILER_SANITIZER_UNDEFINED = 2, UMI_COMPILER_SANITIZER_THREAD = 4, UMI_COMPILER_SANITIZER_MEMORY = 8 } UmiCompilerSanitizer;
typedef struct UmiCompilerPolicy { UmiCompilerOptimisation optimisation; uint32_t sanitizers; bool debug_information; bool link_time_optimisation; bool warnings_as_errors; bool stack_protection; bool control_flow_protection; bool reproducible; } UmiCompilerPolicy;
UmiCompilerPolicy umi_compiler_policy_debug(void);
UmiCompilerPolicy umi_compiler_policy_release(void);
UmiStatus umi_compiler_policy_validate(const UmiCompilerPolicy *policy,char *out_reason,size_t capacity);
UmiStatus umi_compiler_policy_append_arguments(const UmiCompilerPolicy *policy,UmiCompilerFamily family,UmiCompilerPlan *plan);
#endif
