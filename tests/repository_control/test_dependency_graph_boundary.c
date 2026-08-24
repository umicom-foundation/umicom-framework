/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_dependency_graph_boundary.c
 *
 * PURPOSE:
 *   Regression coverage for repository dependency graph boundary semantics.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdlib.h>
#include "umicom/repository/dependency_graph.h"

int main(void)
{
    UmiRepositoryDependencyGraph *g = calloc(1U, sizeof(*g));
    UmiRepositoryDependency a,b;
    assert(g != NULL); umi_repository_dependency_graph_init(g);
    assert(umi_repository_dependency_init(&a,"a","a",1)==UMI_STATUS_OK);
    assert(umi_repository_dependency_init(&b,"b","b",1)==UMI_STATUS_OK);
    assert(umi_repository_dependency_graph_add_node(g,&a)==UMI_STATUS_OK);
    assert(umi_repository_dependency_graph_add_node(g,&b)==UMI_STATUS_OK);
    assert(umi_repository_dependency_graph_add_edge(g,"a","b")==UMI_STATUS_OK);
    assert(umi_repository_dependency_graph_add_edge(g,"b","a")==UMI_STATUS_OK);
    assert(umi_repository_dependency_graph_has_cycle(g));
    free(g);
    return 0;
}
