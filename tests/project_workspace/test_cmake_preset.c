/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_cmake_preset.c
 *
 * PURPOSE:
 *   Implement the test cmake preset behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/cmake_preset.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
int main(void) {
    UmiProjectWorkspaceCmakePreset a,b;
    CHECK(umi_project_workspace_cmake_preset_init(&a,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_cmake_preset_init(&b,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_cmake_preset_set_name(&a,"Cmake Preset")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_cmake_preset_set_detail(&a,"framework-owned")==UMI_STATUS_OK);
    umi_project_workspace_cmake_preset_set_metric(&a,7U);
    CHECK(umi_project_workspace_cmake_preset_validate(&a)==UMI_STATUS_OK);
    CHECK(a.metric==7U);
    CHECK(umi_project_workspace_cmake_preset_same_identity(&a,&b));
    CHECK(strcmp(a.base.name,"Cmake Preset")==0);
    return 0;
}
