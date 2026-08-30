/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_project_variant.c
 *
 * PURPOSE:
 *   Implement the test project variant behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/project_variant.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
int main(void) {
    UmiProjectWorkspaceProjectVariant a,b;
    CHECK(umi_project_workspace_project_variant_init(&a,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_project_variant_init(&b,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_project_variant_set_name(&a,"Project Variant")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_project_variant_set_detail(&a,"framework-owned")==UMI_STATUS_OK);
    umi_project_workspace_project_variant_set_metric(&a,7U);
    CHECK(umi_project_workspace_project_variant_validate(&a)==UMI_STATUS_OK);
    CHECK(a.metric==7U);
    CHECK(umi_project_workspace_project_variant_same_identity(&a,&b));
    CHECK(strcmp(a.base.name,"Project Variant")==0);
    return 0;
}
