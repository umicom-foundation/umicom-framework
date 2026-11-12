/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_plan_allocation.c
 *
 * PURPOSE:
 *   Inject candidate-allocation refusal into the real project planner source and verify unchanged caller records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
static void *RefuseAllocation(size_t size) { (void)size; return NULL; }
/* Exercise only the allocation boundary with the production implementation.
 * Normal behavior is separately linked against the real Framework library. */
#define malloc RefuseAllocation
#include "../../src/developer_project/build_plan.c"
#undef malloc
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    UmiDeveloperProjectBuildPlan *plan = malloc(sizeof *plan), *before = malloc(sizeof *before);
    if (!plan || !before) { free(plan); free(before); return 1; }
    memset(plan, 0x5a, sizeof *plan); memcpy(before, plan, sizeof *plan);
    UmiStatus status;
    if (!strcmp(argv[1], "cmake"))
        status = umi_developer_project_build_plan_cmake(plan, "source", "binary", NULL, NULL, 1, 1);
    else if (!strcmp(argv[1], "zig"))
        status = umi_developer_project_build_plan_zig(plan, "source", 1);
    else if (!strcmp(argv[1], "cargo"))
        status = umi_developer_project_build_plan_cargo(plan, "source", 1, 1);
    else { free(plan); free(before); return 2; }
    int result = status != UMI_STATUS_OUT_OF_MEMORY || memcmp(plan, before, sizeof *plan) != 0;
    if (result) fprintf(stderr, "Allocation refusal changed the caller plan or status.\n");
    free(plan); free(before); return result;
}
