/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/project_ordering.h
 *
 * PURPOSE:
 *   Publish the public project ordering contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_PROJECT_ORDERING_H
#define UMICOM_PROJECT_WORKSPACE_PROJECT_ORDERING_H
#include "umicom/project/workspace/project_graph.h"
#ifdef __cplusplus
extern "C" {
#endif
    UmiStatus umi_project_workspace_project_ordering_topological(const UmiProjectWorkspaceProjectGraph*graph,size_t*out_order,size_t capacity,size_t*out_count);
#ifdef __cplusplus
}
#endif
#endif
