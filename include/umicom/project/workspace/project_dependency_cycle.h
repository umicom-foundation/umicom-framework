/* Umicom Framework | Dependency-cycle detection | MIT */
#ifndef UMICOM_PROJECT_WORKSPACE_PROJECT_DEPENDENCY_CYCLE_H
#define UMICOM_PROJECT_WORKSPACE_PROJECT_DEPENDENCY_CYCLE_H
#include "umicom/project/workspace/project_graph.h"
#ifdef __cplusplus
extern "C" {
#endif
    bool umi_project_workspace_project_dependency_cycle_detect(const UmiProjectWorkspaceProjectGraph *graph);
#ifdef __cplusplus
}
#endif
#endif
