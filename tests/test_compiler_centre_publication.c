/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compiler_centre_publication.c
 *
 * PURPOSE:
 *   Keep returned compiler plans, stored commands and centre revisions coherent across success and refusal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "compiler_centre_fixture.h"
static UmiCompilerPlan plan, plan_before;
static bool has_argument(const UmiCompilerPlan *value,const char *argument)
{
    for (size_t index = 0U; index < value->argument_count; ++index)
        if (strcmp(value->arguments[index],argument) == 0) return true;
    return false;
}
int main(void)
{
    UmiCompilerCentre *centre = malloc(sizeof(*centre)), *before = malloc(sizeof(*before));
    CENTRE_REQUIRE(centre != NULL && before != NULL);
    CENTRE_REQUIRE(CentreFixture(centre) == 0);
    uint64_t revision = centre->revision, database_revision = centre->compilation_database.revision;
    CENTRE_REQUIRE(umi_compiler_centre_plan_unit_in_directory(centre,"project","main","C:/projects/demo",&plan) == UMI_STATUS_OK);
    CENTRE_REQUIRE(centre->revision == revision+1U && centre->compilation_database.revision == database_revision+1U);
    CENTRE_REQUIRE(centre->compilation_database.count == 1U);
    const UmiPolyglotCompilationEntry *stored = &centre->compilation_database.items[0];
    CENTRE_REQUIRE(strcmp(plan.working_directory,"C:/projects/demo") == 0);
    CENTRE_REQUIRE(strcmp(stored->directory,plan.working_directory) == 0);
    CENTRE_REQUIRE(plan.argument_count == stored->plan.argument_count);
    for (size_t index = 0U; index < plan.argument_count; ++index) {
        CENTRE_REQUIRE(strcmp(plan.arguments[index],stored->plan.arguments[index]) == 0);
        CENTRE_REQUIRE(plan.argument_view[index] == plan.arguments[index]);
        CENTRE_REQUIRE(stored->plan.argument_view[index] == stored->plan.arguments[index]);
    }
    /* Replanning a source must replace its prior command rather than quietly
     * returning a new plan while leaving the database with old options. */
    centre->profiles[0].policy.warnings_as_errors = true;
    database_revision = centre->compilation_database.revision;
    CENTRE_REQUIRE(umi_compiler_centre_plan_unit_in_directory(centre,"project","main","C:/projects/other",&plan) == UMI_STATUS_OK);
    CENTRE_REQUIRE(centre->compilation_database.count == 1U && centre->compilation_database.revision == database_revision+1U);
    CENTRE_REQUIRE(has_argument(&plan,"-Werror") && has_argument(&stored->plan,"-Werror"));
    CENTRE_REQUIRE(strcmp(stored->directory,"C:/projects/other") == 0);
    CENTRE_REQUIRE(umi_compiler_centre_plan_unit(centre,"project","main",&plan) == UMI_STATUS_OK);
    CENTRE_REQUIRE(strcmp(plan.working_directory,".") == 0 && strcmp(stored->directory,".") == 0);
    for (unsigned refusal = 0U; refusal < 8U; ++refusal) {
        CENTRE_REQUIRE(CentreFixture(centre) == 0);
        const char *project = "project", *unit = "main", *directory = "C:/project";
        UmiStatus expected = UMI_STATUS_CAPACITY_EXCEEDED;
        switch (refusal) {
        case 0: centre->revision = UINT64_MAX; break;
        case 1: centre->compilation_database.revision = UINT64_MAX; break;
        case 2: project = "missing"; expected = UMI_STATUS_NOT_FOUND; break;
        case 3: unit = "missing"; expected = UMI_STATUS_NOT_FOUND; break;
        case 4: directory = ""; expected = UMI_STATUS_INVALID_ARGUMENT; break;
        case 5: centre->profiles[0].enabled = false; expected = UMI_STATUS_INVALID_STATE; break;
        case 6: centre->profiles[0].policy.sanitizers = UMI_COMPILER_SANITIZER_ADDRESS | UMI_COMPILER_SANITIZER_THREAD; expected = UMI_STATUS_INVALID_STATE; break;
        case 7: centre->providers.items[0].descriptor.available = false; expected = UMI_STATUS_UNAVAILABLE; break;
        }
        (void)memcpy(before,centre,sizeof(*centre));
        (void)memset(&plan,0x5a,sizeof(plan)); (void)memcpy(&plan_before,&plan,sizeof(plan));
        CENTRE_REQUIRE(umi_compiler_centre_plan_unit_in_directory(centre,project,unit,directory,&plan) == expected);
        CENTRE_REQUIRE(memcmp(centre,before,sizeof(*centre)) == 0);
        CENTRE_REQUIRE(memcmp(&plan,&plan_before,sizeof(plan)) == 0);
    }
    CENTRE_REQUIRE(CentreFixture(centre) == 0);
    CENTRE_REQUIRE(umi_compiler_centre_plan_unit(centre,"project","main",&plan) == UMI_STATUS_OK);
    centre->compilation_database.revision = UINT64_MAX;
    (void)memcpy(before,centre,sizeof(*centre)); (void)memcpy(&plan_before,&plan,sizeof(plan));
    CENTRE_REQUIRE(umi_compiler_centre_plan_unit(centre,"project","main",&plan) == UMI_STATUS_CAPACITY_EXCEEDED);
    CENTRE_REQUIRE(memcmp(centre,before,sizeof(*centre)) == 0 && memcmp(&plan,&plan_before,sizeof(plan)) == 0);
    /* Fill with valid different source keys, then exercise a genuinely new row. */
    CENTRE_REQUIRE(CentreFixture(centre) == 0);
    CENTRE_REQUIRE(umi_compiler_centre_plan_unit(centre,"project","main",&plan) == UMI_STATUS_OK);
    for (size_t index = 0U; index < UMI_POLYGLOT_DATABASE_MAX; ++index) {
        if (index != 0U) (void)memcpy(&centre->compilation_database.items[index],&centre->compilation_database.items[0],sizeof(*stored));
        (void)snprintf(centre->compilation_database.items[index].source,sizeof(stored->source),"other-%zu.c",index);
        (void)umi_compiler_plan_arguments(&centre->compilation_database.items[index].plan);
    }
    centre->compilation_database.count = UMI_POLYGLOT_DATABASE_MAX;
    (void)memcpy(before,centre,sizeof(*centre)); (void)memcpy(&plan_before,&plan,sizeof(plan));
    CENTRE_REQUIRE(umi_compiler_centre_plan_unit(centre,"project","main",&plan) == UMI_STATUS_CAPACITY_EXCEEDED);
    CENTRE_REQUIRE(memcmp(centre,before,sizeof(*centre)) == 0 && memcmp(&plan,&plan_before,sizeof(plan)) == 0);
    free(before); free(centre);
    return 0;
}
