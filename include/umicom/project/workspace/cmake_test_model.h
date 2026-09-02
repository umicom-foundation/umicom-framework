/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/cmake_test_model.h
 *
 * PURPOSE:
 *   Publish the public cmake test model contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_CMAKE_TEST_MODEL_H
#define UMICOM_PROJECT_WORKSPACE_CMAKE_TEST_MODEL_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace cmake test model data shared with callers of this public
 * contract.
 */
    typedef struct UmiProjectWorkspaceCmakeTestModel {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceCmakeTestModel;
    UmiStatus umi_project_workspace_cmake_test_model_init(UmiProjectWorkspaceCmakeTestModel *value,const char *id);
    UmiStatus umi_project_workspace_cmake_test_model_validate(const UmiProjectWorkspaceCmakeTestModel *value);
    UmiStatus umi_project_workspace_cmake_test_model_set_name(UmiProjectWorkspaceCmakeTestModel *value,const char *name);
    UmiStatus umi_project_workspace_cmake_test_model_set_detail(UmiProjectWorkspaceCmakeTestModel *value,const char *detail);
    UmiStatus umi_project_workspace_cmake_test_model_set_state(UmiProjectWorkspaceCmakeTestModel *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_cmake_test_model_set_metric(UmiProjectWorkspaceCmakeTestModel *value,uint64_t metric);
    bool umi_project_workspace_cmake_test_model_same_identity(const UmiProjectWorkspaceCmakeTestModel *left,const UmiProjectWorkspaceCmakeTestModel *right);
#ifdef __cplusplus
}
#endif
#endif
