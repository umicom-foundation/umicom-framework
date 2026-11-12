/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compiler_position_independent.c
 *
 * PURPOSE:
 *   Verify explicit PIC mapping and unchanged destinations when code-generation intent is unsupported.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "support/assertions.h"
#include <assert.h>
#include <string.h>
#include "umicom/compiler/compiler.h"

/* Inline plans are large; serial fixtures avoid consuming a thread's stack. */
static UmiCompilerRequest request;
static UmiCompilerPlan plan;
static UmiCompilerPlan saved;
typedef UmiCompilerProvider (*MakeProvider)(const char *,const char *);

static void PreviousPlan(void)
{
    umi_compiler_plan_init(&plan);
    (void)strcpy(plan.provider_id,"previous");
    (void)strcpy(plan.executable,"previous-driver");
    assert(umi_compiler_plan_add_argument(&plan,"prior") == UMI_STATUS_OK);
    (void)memcpy(&saved,&plan,sizeof(saved));
}

static size_t CountArgument(const char *value)
{
    size_t count = 0U;
    for (size_t i = 0U; i < plan.argument_count; ++i) {
        assert(plan.argument_view[i] == plan.arguments[i]);
        if (strcmp(plan.arguments[i],value) == 0) ++count;
    }
    return count;
}

static void GnuCompatiblePlans(void)
{
    const MakeProvider makers[] = {umi_compiler_gcc_provider,umi_compiler_clang_provider};
    for (size_t family = 0U; family < 2U; ++family) {
        UmiCompilerProvider provider = makers[family]("driver","x86_64-unknown-linux-gnu");
        for (size_t language = 0U; language < 2U; ++language) {
            UmiCompilerLanguage kind = language == 0U ? UMI_COMPILER_LANGUAGE_C : UMI_COMPILER_LANGUAGE_CPP;
            assert(umi_compiler_request_init(&request,"project","unit",kind,"source","object") == UMI_STATUS_OK);
            (void)strcpy(request.standard,language == 0U ? "c17" : "c++20");
            assert(umi_compiler_provider_plan(&provider,&request,&plan) == UMI_STATUS_OK);
            assert(CountArgument("-fPIC") == 0U); /* false retains the driver's default */
            request.position_independent = true;
            request.debug_information = true;
            request.warnings_as_errors = true;
            assert(umi_compiler_request_add_option(&request,"-Wall") == UMI_STATUS_OK);
            assert(umi_compiler_provider_plan(&provider,&request,&plan) == UMI_STATUS_OK);
            assert(CountArgument("-fPIC") == 1U);
            assert(strcmp(plan.arguments[plan.argument_count-4U],"-g") == 0);
            assert(strcmp(plan.arguments[plan.argument_count-3U],"-Werror") == 0);
            assert(strcmp(plan.arguments[plan.argument_count-2U],"-fPIC") == 0);
            assert(strcmp(plan.arguments[plan.argument_count-1U],"-Wall") == 0);
            request.action = UMI_COMPILER_ACTION_CHECK;
            PreviousPlan();
            assert(umi_compiler_provider_plan(&provider,&request,&plan) == UMI_STATUS_NOT_IMPLEMENTED);
            assert(memcmp(&saved,&plan,sizeof(plan)) == 0);
        }
        assert(umi_compiler_request_init(&request,"project","unit",UMI_COMPILER_LANGUAGE_ASSEMBLY,
                                        "source.s","object") == UMI_STATUS_OK);
        request.position_independent = true;
        request.action = UMI_COMPILER_ACTION_ASSEMBLE;
        PreviousPlan();
        assert(umi_compiler_provider_plan(&provider,&request,&plan) == UMI_STATUS_NOT_IMPLEMENTED);
        assert(memcmp(&saved,&plan,sizeof(plan)) == 0);
    }
}

static void OtherFamiliesRefuseIntent(void)
{
    const MakeProvider makers[] = {umi_compiler_msvc_provider,umi_compiler_rust_provider,
        umi_compiler_zig_provider,umi_compiler_assembly_provider,umi_compiler_uai_provider,
        umi_compiler_umicc_provider};
    const UmiCompilerLanguage languages[] = {UMI_COMPILER_LANGUAGE_C,UMI_COMPILER_LANGUAGE_RUST,
        UMI_COMPILER_LANGUAGE_ZIG,UMI_COMPILER_LANGUAGE_ASSEMBLY,UMI_COMPILER_LANGUAGE_UAI,
        UMI_COMPILER_LANGUAGE_C};
    for (size_t row = 0U; row < sizeof(makers)/sizeof(makers[0]); ++row) {
        UmiCompilerProvider provider = makers[row]("driver","x86_64-unknown-linux-gnu");
        assert(umi_compiler_request_init(&request,"project","unit",languages[row],"source","object") == UMI_STATUS_OK);
        request.position_independent = true;
        PreviousPlan();
        assert(umi_compiler_provider_plan(&provider,&request,&plan) == UMI_STATUS_NOT_IMPLEMENTED);
        assert(memcmp(&saved,&plan,sizeof(plan)) == 0);
        assert(plan.argument_view[0] == plan.arguments[0]);
    }
}

int main(void)
{
    GnuCompatiblePlans();
    OtherFamiliesRefuseIntent();
    return 0;
}
