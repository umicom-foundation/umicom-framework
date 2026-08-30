/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_dependency_diagnostics.c
 *
 * PURPOSE:
 *   Implement the test dependency diagnostics behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_dependency_diagnostics.c
 *
 * PURPOSE:
 *   Focused regression coverage for Aggregate graph, missing-dependency and binding ambiguity diagnostics.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/graph_node.h"
#include "umicom/runtime/bootstrap/service_graph.h"
#include "umicom/runtime/bootstrap/service_key.h"
#include "umicom/runtime/bootstrap/binding_descriptor.h"
#include "umicom/runtime/bootstrap/binding_registry.h"
#include "umicom/runtime/bootstrap/dependency_diagnostics.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    UmiBootstrapServiceGraph g; UmiBootstrapGraphNode n; UmiBootstrapBindingRegistry reg;
    UmiBootstrapServiceKey key; UmiBootstrapBindingDescriptor b; UmiBootstrapIssueReport report;
    umi_bootstrap_service_graph_init(&g); CHECK(umi_bootstrap_graph_node_init(&n,"svc.a",0,true)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_service_graph_add_node(&g,&n)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_service_key_init(&key,"svc.a","")==UMI_STATUS_OK);
    CHECK(umi_bootstrap_binding_descriptor_init(&b,&key,UMI_BOOTSTRAP_BIND_FACTORY,"factory.a","",5)==UMI_STATUS_OK);
    umi_bootstrap_binding_registry_init(&reg); CHECK(umi_bootstrap_binding_registry_add(&reg,&b)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_dependency_diagnostics_collect(&g,&reg,&key,&report)==UMI_STATUS_OK);
    return 0;
}
