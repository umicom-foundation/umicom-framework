/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_dependency_graph.c
 *
 * PURPOSE:
 *   Verify the dependency graph SDK/runtime contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/sdk_runtime/dependency_graph.h"
int main(void)
{
    UmiSdkRuntimeDependencyGraph value,same;
    umi_sdk_runtime_dependency_graph_init(&value,"sdk-runtime.dependency_graph");
    assert(umi_sdk_runtime_dependency_graph_validate(&value)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_dependency_graph_set_path(&value,"lib/umicom")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_dependency_graph_set_detail(&value,"installed evidence")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_dependency_graph_set_node_count(&value,8U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_dependency_graph_set_edge_count(&value,13U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_dependency_graph_set_state(&value,UMI_SDK_RUNTIME_STATE_READY)==UMI_STATUS_OK);
    umi_sdk_runtime_dependency_graph_init(&same,"sdk-runtime.dependency_graph");
    assert(umi_sdk_runtime_dependency_graph_same_identity(&value,&same));
    return 0;
    }
