/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_cmake_option.c
 *
 * PURPOSE:
 *   Implement the test cmake option behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/cmake_option.h"
#define CHECK(x) do{if(!(x))return __LINE__;}while(0)
int main(void) {
    bool v=false;
    CHECK(umi_project_workspace_cmake_option_parse_bool("ON",&v)==UMI_STATUS_OK&&v);
    CHECK(umi_project_workspace_cmake_option_parse_bool("OFF",&v)==UMI_STATUS_OK&&!v);
    return 0;
}
