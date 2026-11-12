/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compiler_policy_dialects.c
 *
 * PURPOSE:
 *   Verify explicit compiler policy dialects and preserve plans when a requested mapping is unsupported.
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
#define REQUIRE(condition) do { if (!(condition)) { fprintf(stderr,"policy dialect line %d\n",__LINE__); return 1; } } while (0)
static UmiCompilerPlan plan, before;
static void seed(UmiCompilerLanguage language,UmiCompilerAction action)
{
    umi_compiler_plan_init(&plan);
    plan.language = language; plan.action = action;
}
static int compare_arguments(const char *const *expected,size_t count)
{
    REQUIRE(plan.argument_count == count);
    for (size_t index = 0U; index < count; ++index) {
        REQUIRE(strcmp(plan.arguments[index],expected[index]) == 0);
        REQUIRE(plan.argument_view[index] == plan.arguments[index]);
    }
    return 0;
}
int main(void)
{
    static const char *const gnu[] = {"-O0","-Og","-Os","-O2","-O3"};
    static const char *const msvc[] = {"/Od","/Od","/O1","/O2","/Ox"};
    for (unsigned language = UMI_COMPILER_LANGUAGE_C; language <= UMI_COMPILER_LANGUAGE_CPP; ++language) {
        for (unsigned family = UMI_COMPILER_FAMILY_GCC; family <= UMI_COMPILER_FAMILY_MSVC; ++family) {
            for (unsigned level = UMI_COMPILER_OPT_NONE; level <= UMI_COMPILER_OPT_AGGRESSIVE; ++level) {
                seed((UmiCompilerLanguage)language,UMI_COMPILER_ACTION_COMPILE);
                UmiCompilerPolicy policy = {0}; policy.optimisation = (UmiCompilerOptimisation)level;
                REQUIRE(umi_compiler_policy_append_arguments(&policy,(UmiCompilerFamily)family,&plan) == UMI_STATUS_OK);
                const char *expected[] = {family == UMI_COMPILER_FAMILY_MSVC ? msvc[level] : gnu[level]};
                REQUIRE(compare_arguments(expected,1U) == 0);
            }
        }
    }
    UmiCompilerPolicy policy = umi_compiler_policy_debug();
    policy.warnings_as_errors = policy.link_time_optimisation = policy.control_flow_protection = true;
    policy.sanitizers = UMI_COMPILER_SANITIZER_ADDRESS;
    seed(UMI_COMPILER_LANGUAGE_CPP,UMI_COMPILER_ACTION_COMPILE);
    REQUIRE(umi_compiler_policy_append_arguments(&policy,UMI_COMPILER_FAMILY_MSVC,&plan) == UMI_STATUS_OK);
    const char *msvc_flags[] = {"/Od","/Zi","/WX","/GL","/GS","/guard:cf","/fsanitize=address"};
    REQUIRE(compare_arguments(msvc_flags,7U) == 0);
    seed(UMI_COMPILER_LANGUAGE_C,UMI_COMPILER_ACTION_COMPILE);
    policy.sanitizers |= UMI_COMPILER_SANITIZER_UNDEFINED;
    REQUIRE(umi_compiler_policy_append_arguments(&policy,UMI_COMPILER_FAMILY_CLANG,&plan) == UMI_STATUS_OK);
    const char *clang_flags[] = {"-Og","-g","-Werror","-flto","-fstack-protector-strong","-fcf-protection=full","-fsanitize=address","-fsanitize=undefined"};
    REQUIRE(compare_arguments(clang_flags,8U) == 0);
    policy = (UmiCompilerPolicy){0}; policy.sanitizers = UMI_COMPILER_SANITIZER_MEMORY;
    seed(UMI_COMPILER_LANGUAGE_C,UMI_COMPILER_ACTION_COMPILE);
    REQUIRE(umi_compiler_policy_append_arguments(&policy,UMI_COMPILER_FAMILY_CLANG,&plan) == UMI_STATUS_OK);
    const char *memory_flags[] = {"-O0","-fsanitize=memory"};
    REQUIRE(compare_arguments(memory_flags,2U) == 0);
    for (unsigned refusal = 0U; refusal < 13U; ++refusal) {
        seed(UMI_COMPILER_LANGUAGE_C,UMI_COMPILER_ACTION_COMPILE);
        policy = umi_compiler_policy_debug();
        UmiCompilerFamily family = UMI_COMPILER_FAMILY_GCC;
        UmiStatus expected = UMI_STATUS_NOT_IMPLEMENTED;
        switch (refusal) {
        case 0: family = UMI_COMPILER_FAMILY_RUSTC; break;
        case 1: family = UMI_COMPILER_FAMILY_ZIG; break;
        case 2: family = UMI_COMPILER_FAMILY_ASSEMBLER; break;
        case 3: family = UMI_COMPILER_FAMILY_UAI; break;
        case 4: family = UMI_COMPILER_FAMILY_UMICC; break;
        case 5: plan.language = UMI_COMPILER_LANGUAGE_ASSEMBLY; break;
        case 6: plan.action = UMI_COMPILER_ACTION_CHECK; break;
        case 7: plan.action = UMI_COMPILER_ACTION_LINK_EXECUTABLE; break;
        case 8: policy.reproducible = true; break;
        case 9: policy.sanitizers = UMI_COMPILER_SANITIZER_MEMORY; break;
        case 10: family = UMI_COMPILER_FAMILY_MSVC; policy.sanitizers = UMI_COMPILER_SANITIZER_UNDEFINED; break;
        case 11: family = UMI_COMPILER_FAMILY_MSVC; policy.sanitizers = UMI_COMPILER_SANITIZER_THREAD; break;
        case 12: family = UMI_COMPILER_FAMILY_MSVC; policy.sanitizers = UMI_COMPILER_SANITIZER_MEMORY; break;
        }
        (void)memcpy(&before,&plan,sizeof(plan));
        char reason[128];
        REQUIRE(umi_compiler_policy_validate_for_plan(&policy,family,&plan,reason,sizeof(reason)) == expected);
        REQUIRE(reason[0] != '\0');
        REQUIRE(umi_compiler_policy_append_arguments(&policy,family,&plan) == expected);
        REQUIRE(memcmp(&plan,&before,sizeof(plan)) == 0);
    }
    static const uint32_t conflicts[] = {
        UMI_COMPILER_SANITIZER_ADDRESS | UMI_COMPILER_SANITIZER_THREAD,
        UMI_COMPILER_SANITIZER_ADDRESS | UMI_COMPILER_SANITIZER_MEMORY,
        UMI_COMPILER_SANITIZER_THREAD | UMI_COMPILER_SANITIZER_MEMORY,
        UMI_COMPILER_SANITIZER_ADDRESS | UMI_COMPILER_SANITIZER_THREAD | UMI_COMPILER_SANITIZER_MEMORY
    };
    for (size_t index = 0U; index < sizeof(conflicts)/sizeof(conflicts[0]); ++index) {
        policy = (UmiCompilerPolicy){0}; policy.sanitizers = conflicts[index];
        seed(UMI_COMPILER_LANGUAGE_C,UMI_COMPILER_ACTION_COMPILE); before = plan;
        REQUIRE(umi_compiler_policy_append_arguments(&policy,UMI_COMPILER_FAMILY_CLANG,&plan) == UMI_STATUS_INVALID_STATE);
        REQUIRE(memcmp(&plan,&before,sizeof(plan)) == 0);
    }
    /* Mapping refusal precedes capacity or allocation: no partial omission. */
    seed(UMI_COMPILER_LANGUAGE_C,UMI_COMPILER_ACTION_COMPILE);
    policy = umi_compiler_policy_release(); before = plan;
    REQUIRE(umi_compiler_policy_append_arguments(&policy,UMI_COMPILER_FAMILY_CLANG,&plan) == UMI_STATUS_NOT_IMPLEMENTED);
    REQUIRE(memcmp(&plan,&before,sizeof(plan)) == 0);
    REQUIRE(umi_compiler_policy_validate_for_plan(NULL,UMI_COMPILER_FAMILY_GCC,&plan,NULL,0U) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
