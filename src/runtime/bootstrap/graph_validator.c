/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/graph_validator.c
 *
 * PURPOSE:
 *   Implement the graph validator behavior for
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
 * File: src/runtime/bootstrap/graph_validator.c
 *
 * PURPOSE:
 *   Validate service graphs for missing nodes, self-dependencies and cycles.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/graph_validator.h"
#include "umicom/runtime/bootstrap/cycle_report.h"
#include "umicom/runtime/bootstrap/missing_dependency.h"


#include <string.h>

/* Check that bootstrap graph satisfies its contract before another service relies on it. */
UmiStatus umi_bootstrap_graph_validate(
    const UmiBootstrapServiceGraph *graph,
    UmiBootstrapIssueReport *out_report) {
    UmiBootstrapIdList cycle;
    size_t missing;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph == NULL || out_report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_report, 0, sizeof(*out_report));
    missing = umi_bootstrap_graph_missing_dependency_count(
        graph, out_report->first_issue, sizeof(out_report->first_issue));
    out_report->missing_dependencies = missing;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_bootstrap_graph_cycle_report(graph, &cycle) == UMI_STATUS_OK) {
        out_report->cycles = 1U;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (out_report->first_issue[0] == '\0' && cycle.count > 0U) {
            (void)umi_bootstrap_copy_text(out_report->first_issue,
                sizeof(out_report->first_issue), cycle.ids[0]);
        }
    }
    return (out_report->missing_dependencies == 0U && out_report->cycles == 0U)
        ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}
