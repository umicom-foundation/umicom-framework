/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compiler_action_planning.c
 *
 * PURPOSE:
 *   Verify explicit syntax-check plans, action refusal and preservation of prior compiler plans.
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
#include <stdio.h>
#include "umicom/compiler/gcc_provider.h"
#include "umicom/compiler/clang_provider.h"
#include "umicom/compiler/msvc_provider.h"
#include "umicom/compiler/rust_provider.h"
#include "umicom/compiler/zig_provider.h"
#include "umicom/compiler/assembly_provider.h"
#include "umicom/compiler/uai_provider.h"
#include "umicom/compiler/umicc_provider.h"

/* Large inline owners are serial fixtures, not automatic thread-stack storage. */
static UmiCompilerRequest request;
static UmiCompilerPlan plan;
static UmiCompilerPlan saved;

typedef UmiCompilerProvider (*MakeProvider)(const char *, const char *);
typedef struct ProviderCase {
    MakeProvider make;
    UmiCompilerLanguage language;
    bool syntax_check;
    const char *first_compile_argument;
} ProviderCase;

static const ProviderCase cases[] = {
    {umi_compiler_gcc_provider, UMI_COMPILER_LANGUAGE_C, true, "-c"},
    {umi_compiler_clang_provider, UMI_COMPILER_LANGUAGE_C, true, "-c"},
    {umi_compiler_msvc_provider, UMI_COMPILER_LANGUAGE_C, true, "/nologo"},
    {umi_compiler_rust_provider, UMI_COMPILER_LANGUAGE_RUST, false, "source"},
    {umi_compiler_zig_provider, UMI_COMPILER_LANGUAGE_ZIG, false, "build-obj"},
    {umi_compiler_assembly_provider, UMI_COMPILER_LANGUAGE_ASSEMBLY, false, "-c"},
    {umi_compiler_uai_provider, UMI_COMPILER_LANGUAGE_UAI, false, "compile"},
    {umi_compiler_umicc_provider, UMI_COMPILER_LANGUAGE_C, false, "compile"}
};

static bool HasArgument(const char *text)
{
    for (size_t index = 0U; index < plan.argument_count; ++index)
        if (strcmp(plan.arguments[index], text) == 0) return true;
    return false;
}

static void PreviousPlan(void)
{
    umi_compiler_plan_init(&plan);
    (void)strcpy(plan.provider_id, "previous");
    (void)strcpy(plan.executable, "previous-driver");
    assert(umi_compiler_plan_add_argument(&plan, "previous-argument") == UMI_STATUS_OK);
    (void)memcpy(&saved, &plan, sizeof(saved));
}

static void Preserved(void)
{
    assert(memcmp(&saved, &plan, sizeof(plan)) == 0);
    assert(plan.argument_view[0] == plan.arguments[0]);
}

static void ActionMatrix(void)
{
    const UmiCompilerAction unmapped[] = {UMI_COMPILER_ACTION_LINK_EXECUTABLE,
        UMI_COMPILER_ACTION_LINK_SHARED, UMI_COMPILER_ACTION_ARCHIVE};
    for (size_t row = 0U; row < sizeof(cases) / sizeof(cases[0]); ++row) {
        UmiCompilerProvider provider = cases[row].make("driver", "x86_64-unknown-linux-gnu");
        assert(umi_compiler_request_init(&request, "project", "unit", cases[row].language,
                                        "source", "output") == UMI_STATUS_OK);
        assert(umi_compiler_provider_can_plan_action(&provider, request.language, request.action));
        assert(umi_compiler_provider_plan(&provider, &request, &plan) == UMI_STATUS_OK);
        assert(strcmp(plan.arguments[0], cases[row].first_compile_argument) == 0);
        assert(plan.action == request.action);
        for (size_t i = 0U; i < sizeof(unmapped) / sizeof(unmapped[0]); ++i) {
            request.action = unmapped[i];
            PreviousPlan();
            assert(!umi_compiler_provider_can_plan_action(&provider, request.language, request.action));
            assert(umi_compiler_provider_plan(&provider, &request, &plan) == UMI_STATUS_NOT_IMPLEMENTED);
            Preserved();
        }
        request.action = UMI_COMPILER_ACTION_CHECK;
        assert(umi_compiler_provider_can_plan_action(&provider, request.language, request.action) ==
               cases[row].syntax_check);
        if (!cases[row].syntax_check) {
            PreviousPlan();
            assert(umi_compiler_provider_plan(&provider, &request, &plan) == UMI_STATUS_NOT_IMPLEMENTED);
            Preserved();
        }
    }
}

static void SyntaxPlans(void)
{
    for (size_t family = 0U; family < 3U; ++family) {
        UmiCompilerProvider provider = cases[family].make("driver", "x86_64-unknown-linux-gnu");
        bool msvc = family == 2U;
        for (size_t language = 0U; language < 2U; ++language) {
            UmiCompilerLanguage kind = language == 0U ? UMI_COMPILER_LANGUAGE_C : UMI_COMPILER_LANGUAGE_CPP;
            const char *source = language == 0U ? "source with spaces.c" : "source with spaces.cpp";
            assert(umi_compiler_request_init(&request, "project", "unit", kind,
                                            source, "unused-output.o") == UMI_STATUS_OK);
            request.action = UMI_COMPILER_ACTION_CHECK;
            request.debug_information = true;
            request.warnings_as_errors = true;
            (void)strcpy(request.standard, language == 0U ? "c17" : "c++20");
            assert(umi_compiler_request_add_include(&request, "include with spaces") == UMI_STATUS_OK);
            assert(umi_compiler_request_add_definition(&request, "FEATURE=1") == UMI_STATUS_OK);
            assert(umi_compiler_request_add_option(&request, msvc ? "/W4" : "-Wall") == UMI_STATUS_OK);
            assert(umi_compiler_provider_plan(&provider, &request, &plan) == UMI_STATUS_OK);
            assert(plan.action == UMI_COMPILER_ACTION_CHECK && plan.language == kind);
            assert(HasArgument(msvc ? "/Zs" : "-fsyntax-only"));
            assert(HasArgument(source));
            assert(HasArgument(msvc ? "/Iinclude with spaces" : "-Iinclude with spaces"));
            assert(HasArgument(msvc ? "/DFEATURE=1" : "-DFEATURE=1"));
            assert(HasArgument(msvc ? "/WX" : "-Werror"));
            assert(HasArgument(msvc ? "/W4" : "-Wall"));
            assert(!HasArgument("-c") && !HasArgument("/c"));
            assert(!HasArgument("-o") && !HasArgument("/Founused-output.o"));
            assert(!HasArgument("unused-output.o"));
            assert(!HasArgument("-g") && !HasArgument("/Zi"));
            assert(HasArgument(msvc ? (language == 0U ? "/std:c17" : "/std:c++20") :
                                     (language == 0U ? "-std=c17" : "-std=c++20")));
            if (family == 1U) {
                char target[UMI_COMPILER_TEXT_CAPACITY];
                (void)snprintf(target, sizeof(target), "--target=%s", request.target.triple);
                assert(HasArgument(target));
            }
            for (size_t i = 0U; i < plan.argument_count; ++i)
                assert(plan.argument_view[i] == plan.arguments[i]);
        }
        provider.descriptor.available = false;
        assert(umi_compiler_provider_can_plan_action(&provider, request.language, UMI_COMPILER_ACTION_CHECK));
        PreviousPlan();
        assert(umi_compiler_provider_plan(&provider, &request, &plan) == UMI_STATUS_UNAVAILABLE);
        Preserved();
    }
}

static void RefusalsAndCompatibility(void)
{
    UmiCompilerProvider provider = umi_compiler_clang_provider("clang", "x86_64-unknown-linux-gnu");
    assert(!umi_compiler_provider_can_plan_action(NULL, UMI_COMPILER_LANGUAGE_C, UMI_COMPILER_ACTION_CHECK));
    assert(!umi_compiler_provider_can_plan_action(&provider, UMI_COMPILER_LANGUAGE_UNKNOWN, UMI_COMPILER_ACTION_CHECK));
    assert(!umi_compiler_provider_can_plan_action(&provider, (UmiCompilerLanguage)999, UMI_COMPILER_ACTION_CHECK));
    assert(!umi_compiler_provider_can_plan_action(&provider, UMI_COMPILER_LANGUAGE_C, (UmiCompilerAction)0));
    assert(!umi_compiler_provider_can_plan_action(&provider, UMI_COMPILER_LANGUAGE_C, (UmiCompilerAction)999));
    assert(!umi_compiler_provider_can_plan_action(&provider, UMI_COMPILER_LANGUAGE_C, UMI_COMPILER_ACTION_ASSEMBLE));
    assert(!umi_compiler_provider_can_plan_action(&provider, UMI_COMPILER_LANGUAGE_ASSEMBLY, UMI_COMPILER_ACTION_CHECK));
    assert(umi_compiler_provider_can_plan_action(&provider, UMI_COMPILER_LANGUAGE_ASSEMBLY, UMI_COMPILER_ACTION_ASSEMBLE));
    assert(umi_compiler_request_init(&request, "project", "unit", UMI_COMPILER_LANGUAGE_C,
                                    "main.c", "main.o") == UMI_STATUS_OK);
    request.action = UMI_COMPILER_ACTION_ASSEMBLE;
    PreviousPlan();
    assert(umi_compiler_provider_plan(&provider, &request, &plan) == UMI_STATUS_NOT_IMPLEMENTED);
    Preserved();
    request.action = UMI_COMPILER_ACTION_CHECK;
    for (size_t i = 0U; i < UMI_COMPILER_MAX_OPTIONS; ++i) {
        assert(umi_compiler_request_add_include(&request, "include") == UMI_STATUS_OK);
        assert(umi_compiler_request_add_definition(&request, "FEATURE=1") == UMI_STATUS_OK);
        assert(umi_compiler_request_add_option(&request, "-Wall") == UMI_STATUS_OK);
    }
    PreviousPlan();
    assert(umi_compiler_provider_plan(&provider, &request, &plan) == UMI_STATUS_CAPACITY_EXCEEDED);
    Preserved();
    provider.descriptor.family = (UmiCompilerFamily)999;
    assert(!umi_compiler_provider_can_plan_action(&provider, request.language, UMI_COMPILER_ACTION_COMPILE));
    provider = umi_compiler_clang_provider("clang", "x86_64-unknown-linux-gnu");
    (void)memset(provider.descriptor.default_target, 'x', sizeof(provider.descriptor.default_target));
    assert(!umi_compiler_provider_can_plan_action(&provider, request.language, UMI_COMPILER_ACTION_CHECK));
}

int main(void)
{
    ActionMatrix();
    SyntaxPlans();
    RefusalsAndCompatibility();
    return 0;
}
