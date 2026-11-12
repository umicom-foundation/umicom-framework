/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compiler_request_plan_admission.c
 *
 * PURPOSE:
 *   Check compiler request extents, argument mutation refusal and staged plan pointer ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "support/assertions.h"
#include "umicom/compiler/compiler.h"
#include <assert.h>
#include <string.h>

/* Large public records live in static fixture storage to keep the owner test
 * independent of a platform's default thread-stack allowance. Tests are serial. */
static UmiCompilerRequest request, request_before;
static UmiCompilerPlan plan, plan_before, copied;

static void RequestInit(void)
{
    assert(umi_compiler_request_init(&request,"demo","main",UMI_COMPILER_LANGUAGE_C,"main.c","main.o") == UMI_STATUS_OK);
}

static void PlanInit(void)
{
    umi_compiler_plan_init(&plan);
    (void)strcpy(plan.provider_id,"retained-provider");
    (void)strcpy(plan.executable,"retained-compiler");
    assert(umi_compiler_plan_add_argument(&plan,"retained-argument") == UMI_STATUS_OK);
}

static void CorruptRequestText(unsigned field)
{
    switch (field) {
    case 0U: (void)memset(request.project_id,'x',sizeof(request.project_id)); break;
    case 1U: (void)memset(request.unit_id,'x',sizeof(request.unit_id)); break;
    case 2U: (void)memset(request.source_path,'x',sizeof(request.source_path)); break;
    case 3U: (void)memset(request.output_path,'x',sizeof(request.output_path)); break;
    case 4U: (void)memset(request.working_directory,'x',sizeof(request.working_directory)); break;
    case 5U: (void)memset(request.standard,'x',sizeof(request.standard)); break;
    case 6U: (void)memset(request.target.triple,'x',sizeof(request.target.triple)); break;
    case 7U: (void)memset(request.target.architecture,'x',sizeof(request.target.architecture)); break;
    case 8U: (void)memset(request.target.vendor,'x',sizeof(request.target.vendor)); break;
    case 9U: (void)memset(request.target.operating_system,'x',sizeof(request.target.operating_system)); break;
    case 10U: (void)memset(request.target.environment,'x',sizeof(request.target.environment)); break;
    case 11U: request.include_directory_count = 1U; (void)memset(request.include_directories[0],'x',sizeof(request.include_directories[0])); break;
    case 12U: request.definition_count = 1U; (void)memset(request.definitions[0],'x',sizeof(request.definitions[0])); break;
    case 13U: request.option_count = 1U; (void)memset(request.options[0],'x',sizeof(request.options[0])); break;
    case 14U: request.include_directory_count = 1U; break;
    case 15U: request.definition_count = 1U; break;
    case 16U: request.option_count = 1U; break;
    default: assert(false); break;
    }
}

static void RejectRequest(void)
{
    char reason[UMI_COMPILER_TEXT_CAPACITY];
    const UmiCompilerProvider provider = umi_compiler_gcc_provider("gcc","x86_64-unknown-linux-gnu");
    (void)memcpy(&request_before,&request,sizeof(request));
    PlanInit();
    (void)memcpy(&plan_before,&plan,sizeof(plan));
    assert(!umi_compiler_request_storage_valid(&request));
    assert(umi_compiler_request_validate(&request,reason,sizeof(reason)) == UMI_STATUS_INVALID_STATE);
    assert(umi_compiler_request_add_include(&request,"include") == UMI_STATUS_INVALID_STATE);
    assert(umi_compiler_request_add_definition(&request,"DEMO=1") == UMI_STATUS_INVALID_STATE);
    assert(umi_compiler_request_add_option(&request,"-O2") == UMI_STATUS_INVALID_STATE);
    assert(umi_compiler_provider_plan(&provider,&request,&plan) == UMI_STATUS_INVALID_STATE);
    assert(memcmp(&request,&request_before,sizeof(request)) == 0);
    assert(memcmp(&plan,&plan_before,sizeof(plan)) == 0);
}

static void RequestStorage(void)
{
    assert(!umi_compiler_request_storage_valid(NULL));
    for (unsigned field = 0U; field < 17U; ++field) {
        RequestInit(); CorruptRequestText(field); RejectRequest();
    }
    for (unsigned field = 0U; field < 3U; ++field) {
        const size_t bad_counts[] = {UMI_COMPILER_MAX_OPTIONS + 1U,SIZE_MAX};
        for (size_t index = 0U; index < sizeof(bad_counts)/sizeof(bad_counts[0]); ++index) {
            RequestInit();
            if (field == 0U) request.include_directory_count = bad_counts[index];
            else if (field == 1U) request.definition_count = bad_counts[index];
            else request.option_count = bad_counts[index];
            RejectRequest();
        }
    }
    RequestInit();
    char reason[UMI_COMPILER_TEXT_CAPACITY];
    assert(umi_compiler_request_storage_valid(&request));
    assert(umi_compiler_request_validate(&request,reason,sizeof(reason)) == UMI_STATUS_OK);
    const UmiCompilerLanguage bad_languages[] = {UMI_COMPILER_LANGUAGE_UNKNOWN,(UmiCompilerLanguage)-1,(UmiCompilerLanguage)(UMI_COMPILER_LANGUAGE_UAI + 1)};
    for (size_t index = 0U; index < sizeof(bad_languages)/sizeof(bad_languages[0]); ++index) {
        request.language = bad_languages[index];
        assert(umi_compiler_request_storage_valid(&request));
        assert(umi_compiler_request_validate(&request,reason,sizeof(reason)) == UMI_STATUS_INVALID_STATE);
    }
    RequestInit();
    const UmiCompilerAction bad_actions[] = {(UmiCompilerAction)0,(UmiCompilerAction)(UMI_COMPILER_ACTION_CHECK + 1)};
    for (size_t index = 0U; index < sizeof(bad_actions)/sizeof(bad_actions[0]); ++index) {
        request.action = bad_actions[index];
        assert(umi_compiler_request_validate(&request,reason,sizeof(reason)) == UMI_STATUS_INVALID_STATE);
    }
    for (unsigned field = 0U; field < 4U; ++field) {
        RequestInit();
        if (field == 0U) request.project_id[0] = '\0';
        else if (field == 1U) request.unit_id[0] = '\0';
        else if (field == 2U) request.source_path[0] = '\0';
        else request.output_path[0] = '\0';
        assert(umi_compiler_request_storage_valid(&request));
        assert(umi_compiler_request_validate(&request,reason,sizeof(reason)) == UMI_STATUS_INVALID_STATE);
    }
    assert(umi_compiler_request_validate(NULL,reason,sizeof(reason)) == UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_compiler_request_validate(&request,NULL,sizeof(reason)) == UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_compiler_request_validate(&request,reason,0U) == UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_compiler_request_validate(&request,reason,1U) == UMI_STATUS_CAPACITY_EXCEEDED);
}

typedef UmiStatus (*AddRequest)(UmiCompilerRequest *,const char *);

static void IncrementalRequest(void)
{
    const AddRequest add[] = {umi_compiler_request_add_include,umi_compiler_request_add_definition,umi_compiler_request_add_option};
    for (size_t field = 0U; field < sizeof(add)/sizeof(add[0]); ++field) {
        RequestInit();
        char unclosed[UMI_COMPILER_PATH_CAPACITY];
        (void)memset(unclosed,'x',sizeof(unclosed));
        (void)memcpy(&request_before,&request,sizeof(request));
        assert(add[field](&request,unclosed) == UMI_STATUS_CAPACITY_EXCEEDED);
        assert(memcmp(&request,&request_before,sizeof(request)) == 0);
        assert(add[field](&request,NULL) == UMI_STATUS_INVALID_ARGUMENT);
        assert(add[field](&request,"") == UMI_STATUS_INVALID_ARGUMENT);
        assert(memcmp(&request,&request_before,sizeof(request)) == 0);
        assert(add[field](NULL,"item") == UMI_STATUS_INVALID_ARGUMENT);
        assert(add[field](&request,"first") == UMI_STATUS_OK);
        const char *borrowed = field == 0U ? request.include_directories[0] : field == 1U ? request.definitions[0] : request.options[0];
        assert(add[field](&request,borrowed) == UMI_STATUS_OK);
        const char *second = field == 0U ? request.include_directories[1] : field == 1U ? request.definitions[1] : request.options[1];
        assert(strcmp(second,"first") == 0);
        for (size_t index = 2U; index < UMI_COMPILER_MAX_OPTIONS; ++index)
            assert(add[field](&request,"item") == UMI_STATUS_OK);
        (void)memcpy(&request_before,&request,sizeof(request));
        assert(add[field](&request,"overflow") == UMI_STATUS_CAPACITY_EXCEEDED);
        assert(memcmp(&request,&request_before,sizeof(request)) == 0);
    }
}

static void RejectPlan(void)
{
    char reason[UMI_COMPILER_TEXT_CAPACITY];
    (void)memcpy(&plan_before,&plan,sizeof(plan));
    assert(!umi_compiler_plan_storage_valid(&plan));
    assert(umi_compiler_plan_validate(&plan,reason,sizeof(reason)) == UMI_STATUS_INVALID_STATE);
    assert(umi_compiler_plan_add_argument(&plan,"new") == UMI_STATUS_INVALID_STATE);
    assert(umi_compiler_plan_arguments(&plan) == NULL);
    assert(memcmp(&plan,&plan_before,sizeof(plan)) == 0);
}

static void ArgumentPlans(void)
{
    assert(!umi_compiler_plan_storage_valid(NULL));
    const size_t bad_counts[] = {UMI_COMPILER_MAX_ARGUMENTS + 1U,SIZE_MAX};
    for (size_t index = 0U; index < sizeof(bad_counts)/sizeof(bad_counts[0]); ++index) {
        PlanInit(); plan.argument_count = bad_counts[index]; RejectPlan();
    }
    for (unsigned field = 0U; field < 5U; ++field) {
        PlanInit();
        if (field == 0U) (void)memset(plan.provider_id,'x',sizeof(plan.provider_id));
        else if (field == 1U) (void)memset(plan.executable,'x',sizeof(plan.executable));
        else if (field == 2U) (void)memset(plan.working_directory,'x',sizeof(plan.working_directory));
        else if (field == 3U) (void)memset(plan.arguments[0],'x',sizeof(plan.arguments[0]));
        else plan.arguments[0][0] = '\0';
        RejectPlan();
    }
    PlanInit();
    char unclosed[UMI_COMPILER_TEXT_CAPACITY];
    (void)memset(unclosed,'x',sizeof(unclosed));
    (void)memcpy(&plan_before,&plan,sizeof(plan));
    assert(umi_compiler_plan_add_argument(&plan,unclosed) == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(umi_compiler_plan_add_argument(&plan,NULL) == UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_compiler_plan_add_argument(&plan,"") == UMI_STATUS_INVALID_ARGUMENT);
    assert(memcmp(&plan,&plan_before,sizeof(plan)) == 0);
    assert(umi_compiler_plan_add_argument(NULL,"x") == UMI_STATUS_INVALID_ARGUMENT);
    plan.revision = UINT64_MAX - 1U;
    assert(umi_compiler_plan_add_argument(&plan,plan.arguments[0]) == UMI_STATUS_OK);
    assert(plan.revision == UINT64_MAX && plan.argument_count == 2U);
    assert(strcmp(plan.arguments[1],"retained-argument") == 0);
    (void)memcpy(&plan_before,&plan,sizeof(plan));
    assert(umi_compiler_plan_add_argument(&plan,"exhausted") == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(memcmp(&plan,&plan_before,sizeof(plan)) == 0);
    assert(umi_compiler_plan_arguments(&plan) != NULL);
    copied = plan;
    assert(umi_compiler_plan_storage_valid(&copied));
    const char *const *view = umi_compiler_plan_arguments(&copied);
    assert(view == copied.argument_view);
    for (size_t index = 0U; index < copied.argument_count; ++index) {
        assert(view[index] == copied.arguments[index]);
        assert(view[index] != plan.arguments[index]);
    }
    PlanInit(); copied = plan;
    assert(umi_compiler_plan_add_argument(&copied,"next") == UMI_STATUS_OK);
    assert(copied.argument_view[0] == copied.arguments[0]);
    assert(copied.argument_view[1] == copied.arguments[1]);
    umi_compiler_plan_init(&plan);
    for (size_t index = 0U; index < UMI_COMPILER_MAX_ARGUMENTS; ++index)
        assert(umi_compiler_plan_add_argument(&plan,"item") == UMI_STATUS_OK);
    (void)memcpy(&plan_before,&plan,sizeof(plan));
    assert(umi_compiler_plan_add_argument(&plan,"overflow") == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(memcmp(&plan,&plan_before,sizeof(plan)) == 0);
    view = umi_compiler_plan_arguments(&plan);
    assert(view[UMI_COMPILER_MAX_ARGUMENTS - 1U] == plan.arguments[UMI_COMPILER_MAX_ARGUMENTS - 1U]);
    char reason[UMI_COMPILER_TEXT_CAPACITY];
    assert(umi_compiler_plan_storage_valid(&plan));
    assert(umi_compiler_plan_validate(&plan,reason,sizeof(reason)) == UMI_STATUS_INVALID_STATE);
    (void)strcpy(plan.provider_id,"gcc"); (void)strcpy(plan.executable,"gcc");
    assert(umi_compiler_plan_validate(&plan,reason,sizeof(reason)) == UMI_STATUS_OK);
    assert(umi_compiler_plan_validate(&plan,reason,1U) == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(umi_compiler_plan_arguments(NULL) == NULL);
}

static void ProviderPublication(void)
{
    UmiCompilerProvider provider = umi_compiler_gcc_provider("gcc","x86_64-unknown-linux-gnu");
    RequestInit(); PlanInit();
    /* Long but properly terminated request paths fit their owner, yet cannot
     * fit one argument. Failure occurs after earlier arguments were prepared. */
    (void)memset(request.source_path,'a',UMI_COMPILER_TEXT_CAPACITY);
    request.source_path[UMI_COMPILER_TEXT_CAPACITY] = '\0';
    (void)memcpy(&plan_before,&plan,sizeof(plan));
    assert(umi_compiler_provider_plan(&provider,&request,&plan) == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(memcmp(&plan,&plan_before,sizeof(plan)) == 0);
    RequestInit();
    for (size_t index = 0U; index < UMI_COMPILER_MAX_OPTIONS; ++index) {
        assert(umi_compiler_request_add_include(&request,"include") == UMI_STATUS_OK);
        assert(umi_compiler_request_add_definition(&request,"DEMO=1") == UMI_STATUS_OK);
        assert(umi_compiler_request_add_option(&request,"-O2") == UMI_STATUS_OK);
    }
    assert(umi_compiler_provider_plan(&provider,&request,&plan) == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(memcmp(&plan,&plan_before,sizeof(plan)) == 0);
    RequestInit(); provider.descriptor.available = false;
    assert(umi_compiler_provider_plan(&provider,&request,&plan) == UMI_STATUS_UNAVAILABLE);
    assert(memcmp(&plan,&plan_before,sizeof(plan)) == 0);
    provider.descriptor.available = true;
    provider.descriptor.language_mask = UMI_COMPILER_LANGUAGE_BIT(UMI_COMPILER_LANGUAGE_RUST);
    assert(umi_compiler_provider_plan(&provider,&request,&plan) == UMI_STATUS_NOT_IMPLEMENTED);
    assert(memcmp(&plan,&plan_before,sizeof(plan)) == 0);
    provider = umi_compiler_gcc_provider("gcc","x86_64-unknown-linux-gnu");
    assert(umi_compiler_provider_plan(&provider,&request,&plan) == UMI_STATUS_OK);
    assert(strcmp(plan.provider_id,"gcc") == 0);
    for (size_t index = 0U; index < plan.argument_count; ++index)
        assert(plan.argument_view[index] == plan.arguments[index]);
    char reason[UMI_COMPILER_TEXT_CAPACITY];
    assert(umi_compiler_plan_validate(&plan,reason,sizeof(reason)) == UMI_STATUS_OK);
}

int main(void)
{
    RequestStorage();
    IncrementalRequest();
    ArgumentPlans();
    ProviderPublication();
    return 0;
}
