/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compiler_policy_profile.c
 *
 * PURPOSE:
 *   Implement the test compiler policy profile behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Batch 58 test | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/compiler.h"
#include <assert.h>
int main(void) { UmiCompilerRegistry registry; UmiCompilerProvider provider; UmiCompilerProfile profile; UmiCompilerTarget target; UmiCompilerPlan plan; char reason[256]; umi_compiler_registry_init(&registry); provider = umi_compiler_clang_provider("clang","x86_64-unknown-linux-gnu"); assert(umi_compiler_registry_register(&registry,&provider) == UMI_STATUS_OK); assert(umi_compiler_target_host(&target) == UMI_STATUS_OK); assert(umi_compiler_profile_init(&profile,"release","Release","clang",UMI_COMPILER_LANGUAGE_C,&target) == UMI_STATUS_OK); profile.policy = umi_compiler_policy_release(); assert(umi_compiler_profile_validate(&profile,&registry,reason,sizeof(reason)) == UMI_STATUS_OK); umi_compiler_plan_init(&plan); assert(umi_compiler_policy_append_arguments(&profile.policy,UMI_COMPILER_FAMILY_CLANG,&plan) == UMI_STATUS_OK); assert(plan.argument_count >= 4U); return 0; }
