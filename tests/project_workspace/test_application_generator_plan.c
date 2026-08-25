#include "umicom/project/workspace/application_generator_plan.h"
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
int main(void) {
    UmiProjectWorkspaceApplicationGeneratorPlan v;
    umi_project_workspace_application_generator_plan_init(&v);
    CHECK(umi_project_workspace_application_generator_plan_add(&v,"scan","Scan source",true)==UMI_STATUS_OK);
    CHECK(!umi_project_workspace_application_generator_plan_ready(&v));
    CHECK(umi_project_workspace_application_generator_plan_complete(&v,"scan")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_application_generator_plan_ready(&v));
    return 0;
}
