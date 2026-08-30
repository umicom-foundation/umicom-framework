/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_dependency_graph_invalid.c
 *
 * PURPOSE:
 *   Regression coverage for repository dependency graph invalid semantics.
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
    assert(g != NULL); umi_repository_dependency_graph_init(g);
    assert(umi_repository_dependency_graph_add_edge(g,"missing","other")==UMI_STATUS_NOT_FOUND);
    free(g);
    return 0;
}
