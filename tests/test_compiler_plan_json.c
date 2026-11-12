/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compiler_plan_json.c
 *
 * PURPOSE:
 *   Verify complete JSON publication, UTF-8 escaping and bounded compiler plan admission.
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
#include "umicom/compiler/plan.h"

static UmiCompilerPlan plan, saved;
static char output[UMI_COMPILER_PLAN_JSON_CAPACITY];
static char before[UMI_COMPILER_PLAN_JSON_CAPACITY];

static void Seed(void)
{
    umi_compiler_plan_init(&plan);
    (void)strcpy(plan.provider_id,"gcc");
    (void)strcpy(plan.executable,"C:\\tools\\compiler.exe");
    (void)strcpy(plan.working_directory,"folder \"quoted\"\nline");
    plan.language = UMI_COMPILER_LANGUAGE_C;
    plan.action = UMI_COMPILER_ACTION_CHECK;
    assert(umi_compiler_plan_add_argument(&plan,"-fsyntax-only") == UMI_STATUS_OK);
    assert(umi_compiler_plan_add_argument(&plan,"source with spaces.c") == UMI_STATUS_OK);
}

static void PrepareOutput(void)
{
    (void)memset(output,0x5a,sizeof(output));
    (void)memcpy(before,output,sizeof(output));
    (void)memcpy(&saved,&plan,sizeof(plan));
}

static void Unchanged(void)
{
    assert(memcmp(output,before,sizeof(output)) == 0);
    assert(memcmp(&plan,&saved,sizeof(plan)) == 0);
}

int main(void)
{
    Seed();
    PrepareOutput();
    assert(umi_compiler_plan_json(&plan,output,sizeof(output)) == UMI_STATUS_OK);
    const char *expected =
        "{\"schema_version\":1,\"executed\":false,\"toolchain_verified\":false,"
        "\"provider\":\"gcc\",\"executable\":\"C:\\\\tools\\\\compiler.exe\","
        "\"working_directory\":\"folder \\\"quoted\\\"\\nline\",\"language\":1,\"action\":6,"
        "\"arguments\":[\"-fsyntax-only\",\"source with spaces.c\"]}\n";
    assert(strcmp(output,expected) == 0);
    assert(memcmp(&plan,&saved,sizeof(plan)) == 0);
    size_t length = strlen(output);
    PrepareOutput();
    assert(umi_compiler_plan_json(&plan,output,length) == UMI_STATUS_CAPACITY_EXCEEDED);
    Unchanged();
    assert(umi_compiler_plan_json(&plan,output,length + 1U) == UMI_STATUS_OK);
    assert(strcmp(output,expected) == 0);
    assert((unsigned char)output[length + 1U] == 0x5aU);

    const size_t bad_counts[] = {UMI_COMPILER_MAX_ARGUMENTS + 1U,SIZE_MAX};
    for (size_t i = 0U; i < sizeof(bad_counts)/sizeof(bad_counts[0]); ++i) {
        Seed(); plan.argument_count = bad_counts[i]; PrepareOutput();
        assert(umi_compiler_plan_json(&plan,output,sizeof(output)) == UMI_STATUS_INVALID_STATE);
        Unchanged();
    }
    Seed(); plan.action = (UmiCompilerAction)0; PrepareOutput();
    assert(umi_compiler_plan_json(&plan,output,sizeof(output)) == UMI_STATUS_INVALID_STATE);
    Unchanged();
    Seed(); plan.language = UMI_COMPILER_LANGUAGE_UNKNOWN; PrepareOutput();
    assert(umi_compiler_plan_json(&plan,output,sizeof(output)) == UMI_STATUS_INVALID_STATE);
    Unchanged();
    Seed(); (void)memset(plan.arguments[1],'x',sizeof(plan.arguments[1])); PrepareOutput();
    assert(umi_compiler_plan_json(&plan,output,sizeof(output)) == UMI_STATUS_INVALID_STATE);
    Unchanged();
    Seed(); plan.arguments[1][0] = (char)0xc0; plan.arguments[1][1] = (char)0xaf;
    plan.arguments[1][2] = '\0'; PrepareOutput();
    assert(umi_compiler_plan_json(&plan,output,sizeof(output)) == UMI_STATUS_INVALID_ARGUMENT);
    Unchanged();
    Seed(); PrepareOutput();
    assert(umi_compiler_plan_json(NULL,output,sizeof(output)) == UMI_STATUS_INVALID_ARGUMENT);
    Unchanged();
    assert(umi_compiler_plan_json(&plan,NULL,sizeof(output)) == UMI_STATUS_INVALID_ARGUMENT);
    Unchanged();
    assert(umi_compiler_plan_json(&plan,output,0U) == UMI_STATUS_INVALID_ARGUMENT);
    Unchanged();

    /* Maximal escaping exercises the declared allocation bound. Cached views
     * deliberately refer elsewhere: presentation reads only admitted inline text. */
    Seed();
    (void)memset(plan.provider_id,1,sizeof(plan.provider_id)-1U);
    plan.provider_id[sizeof(plan.provider_id)-1U] = '\0';
    (void)memset(plan.executable,1,sizeof(plan.executable)-1U);
    plan.executable[sizeof(plan.executable)-1U] = '\0';
    (void)memset(plan.working_directory,1,sizeof(plan.working_directory)-1U);
    plan.working_directory[sizeof(plan.working_directory)-1U] = '\0';
    plan.argument_count = UMI_COMPILER_MAX_ARGUMENTS;
    for (size_t i = 0U; i < plan.argument_count; ++i) {
        (void)memset(plan.arguments[i],1,sizeof(plan.arguments[i])-1U);
        plan.arguments[i][sizeof(plan.arguments[i])-1U] = '\0';
        plan.argument_view[i] = "borrowed cache is ignored";
    }
    PrepareOutput();
    assert(umi_compiler_plan_json(&plan,output,sizeof(output)) == UMI_STATUS_OK);
    assert(strstr(output,"\\u0001") != NULL);
    assert(memcmp(&plan,&saved,sizeof(plan)) == 0);
    Seed();
    (void)strcpy(plan.arguments[1],"caf\xc3\xa9.c");
    assert(umi_compiler_plan_json(&plan,output,sizeof(output)) == UMI_STATUS_OK);
    assert(strstr(output,"caf\xc3\xa9.c") != NULL);
    return 0;
}
