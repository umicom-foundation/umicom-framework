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
void umi_sdk_runtime_dependency_graph_init(UmiSdkRuntimeDependencyGraph *value,const char *id);
UmiStatus umi_sdk_runtime_dependency_graph_validate(const UmiSdkRuntimeDependencyGraph *value);
UmiStatus umi_sdk_runtime_dependency_graph_set_path(UmiSdkRuntimeDependencyGraph *value,const char *path);
UmiStatus umi_sdk_runtime_dependency_graph_set_detail(UmiSdkRuntimeDependencyGraph *value,const char *detail);
UmiStatus umi_sdk_runtime_dependency_graph_set_node_count(UmiSdkRuntimeDependencyGraph *value,uint64_t number);
UmiStatus umi_sdk_runtime_dependency_graph_set_edge_count(UmiSdkRuntimeDependencyGraph *value,uint64_t number);
UmiStatus umi_sdk_runtime_dependency_graph_set_state(UmiSdkRuntimeDependencyGraph *value,UmiSdkRuntimeState state);
bool umi_sdk_runtime_dependency_graph_same_identity(const UmiSdkRuntimeDependencyGraph *left,const UmiSdkRuntimeDependencyGraph *right);
#ifdef __cplusplus
}
#endif
#endif
