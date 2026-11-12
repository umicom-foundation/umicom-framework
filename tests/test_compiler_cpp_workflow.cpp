/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compiler_cpp_workflow.cpp
 *
 * PURPOSE:
 *   Exercise the C compiler-planning contract from a C++ client while retaining explicit C-style value ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "umicom/compiler/compiler.h"

/* C++ is used only to qualify the consumer boundary. Framework remains C, and
 * the example uses explicit storage and status checks instead of a second
 * implementation of its provider, plan or ownership rules. */
#define CHECK(c) do { if (!(c)) { std::fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c); goto cleanup; } } while (0)
int main()
{
    int result = 1;
    UmiCompilerRequest *request = static_cast<UmiCompilerRequest *>(std::malloc(sizeof(*request)));
    UmiCompilerPlan *plan = static_cast<UmiCompilerPlan *>(std::malloc(sizeof(*plan)));
    UmiCompilerProjectManifest *manifest =
        static_cast<UmiCompilerProjectManifest *>(std::malloc(sizeof(*manifest)));
    UmiCompilerUnitManifest unit = {};
    UmiCompilerProvider provider = umi_compiler_gcc_provider("unexecuted-gcc","x86_64-unknown-linux-gnu");
    uint32_t languages = 0U;
    const char *const *arguments = nullptr;
    CHECK(request != nullptr && plan != nullptr && manifest != nullptr);
    CHECK(umi_compiler_request_init(request,"project","unit",UMI_COMPILER_LANGUAGE_C,
                                   "source.c","object.o") == UMI_STATUS_OK);
    CHECK(umi_compiler_provider_plan(&provider,request,plan) == UMI_STATUS_OK);
    CHECK(std::strcmp(plan->executable,"unexecuted-gcc") == 0);
    arguments = umi_compiler_plan_arguments(plan);
    CHECK(arguments != nullptr && plan->argument_count != 0U);
    for (size_t index = 0U; index < plan->argument_count; ++index)
        CHECK(arguments[index] == plan->arguments[index]);
    CHECK(umi_compiler_manifest_init(manifest,"project","C++ consumer","x86_64-unknown-linux-gnu") == UMI_STATUS_OK);
    std::strcpy(unit.unit_id,"unit");
    std::strcpy(unit.source,"source.c");
    std::strcpy(unit.output,"object.o");
    unit.language = UMI_COMPILER_LANGUAGE_C;
    CHECK(umi_compiler_manifest_add_unit(manifest,&unit) == UMI_STATUS_OK);
    CHECK(umi_compiler_manifest_languages(manifest,&languages) == UMI_STATUS_OK);
    CHECK(languages == UMI_COMPILER_LANGUAGE_BIT(UMI_COMPILER_LANGUAGE_C));
    CHECK(std::strcmp(umi_compiler_language_name(unit.language),"C") == 0);
    result = 0;
cleanup:
    std::free(manifest);
    std::free(plan);
    std::free(request);
    return result;
}
