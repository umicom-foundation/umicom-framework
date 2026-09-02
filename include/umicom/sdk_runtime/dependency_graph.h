/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/dependency_graph.h
 *
 * PURPOSE:
 *   Describe runtime component dependency edges and closure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_DEPENDENCY_GRAPH
#define UMICOM_SDK_RUNTIME_DEPENDENCY_GRAPH
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime dependency graph data shared with callers of this public
 * contract.
 */
typedef struct UmiSdkRuntimeDependencyGraph
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t node_count;
    uint64_t edge_count;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
    } UmiSdkRuntimeDependencyGraph;
/**
 * Initialise sdk runtime dependency graph from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_dependency_graph_init(UmiSdkRuntimeDependencyGraph *value,const char *id);
/**
 * Check that sdk runtime dependency graph satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_dependency_graph_validate(const UmiSdkRuntimeDependencyGraph *value);
/**
 * Provide the sdk runtime dependency graph set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_dependency_graph_set_path(UmiSdkRuntimeDependencyGraph *value,const char *path);
/**
 * Provide the sdk runtime dependency graph set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_dependency_graph_set_detail(UmiSdkRuntimeDependencyGraph *value,const char *detail);
/**
 * Return the number of records represented by sdk runtime dependency graph set node
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_dependency_graph_set_node_count(UmiSdkRuntimeDependencyGraph *value,uint64_t number);
/**
 * Return the number of records represented by sdk runtime dependency graph set edge
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_dependency_graph_set_edge_count(UmiSdkRuntimeDependencyGraph *value,uint64_t number);
/**
 * Provide the sdk runtime dependency graph set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_dependency_graph_set_state(UmiSdkRuntimeDependencyGraph *value,UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime dependency graph same identity operation used by this module and
 * its client applications.
 */
bool umi_sdk_runtime_dependency_graph_same_identity(const UmiSdkRuntimeDependencyGraph *left,const UmiSdkRuntimeDependencyGraph *right);
#ifdef __cplusplus
}
#endif
#endif
