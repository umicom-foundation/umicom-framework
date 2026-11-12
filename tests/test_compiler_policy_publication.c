/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compiler_policy_publication.c
 *
 * PURPOSE:
 *   Preserve complete compiler plans on late policy failure and rebase successful argument views.
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
#define REQUIRE(condition) do { if (!(condition)) { fprintf(stderr,"policy publication line %d\n",__LINE__); return 1; } } while (0)
static UmiCompilerPlan plan, before;
/* Declare the intended language and action so the fixture exercises atomic publication after dialect admission. Retain the old untyped draft setup for review. The previous implementation is retained for review. */
#if 0
static void seed(size_t count,uint64_t revision)
{
    umi_compiler_plan_init(&plan);
    for (size_t index = 0U; index < count; ++index)
        (void)strcpy(plan.arguments[index],"existing");
    plan.argument_count = count; plan.revision = revision;
    (void)umi_compiler_plan_arguments(&plan);
}
#endif
static void seed(size_t count,uint64_t revision)
{
    umi_compiler_plan_init(&plan);
    plan.language = UMI_COMPILER_LANGUAGE_C; plan.action = UMI_COMPILER_ACTION_COMPILE;
    for (size_t index = 0U; index < count; ++index)
        (void)strcpy(plan.arguments[index],"existing");
    plan.argument_count = count; plan.revision = revision;
    (void)umi_compiler_plan_arguments(&plan);
}
int main(void)
{
    UmiCompilerPolicy policy = umi_compiler_policy_debug();
    /* Debug adds three arguments. The first append succeeds privately, then the
     * second fails; all live strings, counts, revisions and pointers must survive. */
    seed(UMI_COMPILER_MAX_ARGUMENTS-1U,1U); before = plan;
    REQUIRE(umi_compiler_policy_append_arguments(&policy,UMI_COMPILER_FAMILY_GCC,&plan) == UMI_STATUS_CAPACITY_EXCEEDED);
    REQUIRE(memcmp(&plan,&before,sizeof(plan)) == 0);
    seed(0U,UINT64_MAX-1U); before = plan;
    REQUIRE(umi_compiler_policy_append_arguments(&policy,UMI_COMPILER_FAMILY_CLANG,&plan) == UMI_STATUS_CAPACITY_EXCEEDED);
    REQUIRE(memcmp(&plan,&before,sizeof(plan)) == 0);
    seed(UMI_COMPILER_MAX_ARGUMENTS-3U,UINT64_MAX-3U);
    REQUIRE(umi_compiler_policy_append_arguments(&policy,UMI_COMPILER_FAMILY_GCC,&plan) == UMI_STATUS_OK);
    REQUIRE(plan.argument_count == UMI_COMPILER_MAX_ARGUMENTS && plan.revision == UINT64_MAX);
    REQUIRE(strcmp(plan.arguments[plan.argument_count-3U],"-Og") == 0);
    REQUIRE(strcmp(plan.arguments[plan.argument_count-2U],"-g") == 0);
    REQUIRE(strcmp(plan.arguments[plan.argument_count-1U],"-fstack-protector-strong") == 0);
    for (size_t index = 0U; index < plan.argument_count; ++index)
        REQUIRE(plan.argument_view[index] == plan.arguments[index]);
    seed(1U,1U);
    REQUIRE(umi_compiler_policy_append_arguments(&policy,UMI_COMPILER_FAMILY_MSVC,&plan) == UMI_STATUS_OK);
    REQUIRE(strcmp(plan.arguments[1],"/Od") == 0 && strcmp(plan.arguments[2],"/Zi") == 0 && strcmp(plan.arguments[3],"/GS") == 0);
    for (unsigned bad = 0U; bad < 8U; ++bad) {
        seed(0U,1U); policy = umi_compiler_policy_debug();
        UmiCompilerFamily family = UMI_COMPILER_FAMILY_GCC;
        UmiStatus expected = UMI_STATUS_INVALID_ARGUMENT;
        switch (bad) {
        case 0: policy.optimisation = (UmiCompilerOptimisation)-1; break;
        case 1: policy.optimisation = (UmiCompilerOptimisation)99; break;
        case 2: policy.sanitizers = UINT32_C(0x80000000); break;
        case 3: family = UMI_COMPILER_FAMILY_UNKNOWN; break;
        case 4: family = (UmiCompilerFamily)99; break;
        case 5: policy.sanitizers = UMI_COMPILER_SANITIZER_THREAD | UMI_COMPILER_SANITIZER_ADDRESS; expected = UMI_STATUS_INVALID_STATE; break;
        case 6: plan.argument_count = SIZE_MAX; expected = UMI_STATUS_INVALID_STATE; break;
        case 7: (void)memset(plan.executable,'x',sizeof(plan.executable)); expected = UMI_STATUS_INVALID_STATE; break;
        }
        before = plan;
        REQUIRE(umi_compiler_policy_append_arguments(&policy,family,&plan) == expected);
        REQUIRE(memcmp(&plan,&before,sizeof(plan)) == 0);
    }
    char reason[64];
    policy = umi_compiler_policy_debug(); policy.sanitizers = UINT32_C(0x80000000);
    REQUIRE(umi_compiler_policy_validate(&policy,reason,sizeof(reason)) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(strcmp(reason,"unknown sanitizer bits") == 0);
    REQUIRE(umi_compiler_policy_append_arguments(NULL,UMI_COMPILER_FAMILY_GCC,&plan) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(umi_compiler_policy_append_arguments(&policy,UMI_COMPILER_FAMILY_GCC,NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
