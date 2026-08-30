/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_dependency_graph_contract.c
 *
 * PURPOSE:
 *   Regression coverage for repository dependency graph contract semantics.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdlib.h>
#include "umicom/repository/dependency_graph.h"

int main(void)
{
    UmiRepositoryDependencyGraph *g = calloc(1U, sizeof(*g));
    UmiRepositoryDependency a,b;
    assert(g != NULL); umi_repository_dependency_graph_init(g);
    assert(umi_repository_dependency_init(&a,"parent","parent",1)==UMI_STATUS_OK);
    assert(umi_repository_dependency_init(&b,"framework","framework",1)==UMI_STATUS_OK);
    assert(umi_repository_dependency_graph_add_node(g,&a)==UMI_STATUS_OK);
    assert(umi_repository_dependency_graph_add_node(g,&b)==UMI_STATUS_OK);
    assert(umi_repository_dependency_graph_add_edge(g,"parent","framework")==UMI_STATUS_OK);
    assert(!umi_repository_dependency_graph_has_cycle(g));
    free(g);
    return 0;
}
