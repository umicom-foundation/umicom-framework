/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/dependency_diagnostics.c
 *
 * PURPOSE:
 *   Implement the dependency diagnostics behavior for
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
 * File: src/runtime/bootstrap/dependency_diagnostics.c
 *
 * PURPOSE:
 *   Aggregate graph, missing-dependency and binding ambiguity diagnostics.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/dependency_diagnostics.h"
#include "umicom/runtime/bootstrap/ambiguity_report.h"
#include "umicom/runtime/bootstrap/cycle_report.h"
#include "umicom/runtime/bootstrap/missing_dependency.h"


#include <string.h>
/*
 * Provide the bootstrap dependency diagnostics collect operation used by this module and
 * its client applications.
 */
UmiStatus umi_bootstrap_dependency_diagnostics_collect(
    const UmiBootstrapServiceGraph *graph,
    const UmiBootstrapBindingRegistry *bindings,
    const UmiBootstrapServiceKey *key,
    UmiBootstrapIssueReport *out_report) {
    UmiBootstrapIdList cycle;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph == NULL || bindings == NULL || key == NULL || out_report == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_report, 0, sizeof(*out_report));
    out_report->missing_dependencies = umi_bootstrap_graph_missing_dependency_count(
        graph, out_report->first_issue, sizeof(out_report->first_issue));
    out_report->ambiguous_bindings = umi_bootstrap_binding_ambiguity_count(bindings, key);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_bootstrap_graph_cycle_report(graph, &cycle) == UMI_STATUS_OK) {
        out_report->cycles = 1U;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (out_report->first_issue[0] == '\0' && cycle.count > 0U)
            (void)umi_bootstrap_copy_text(out_report->first_issue,
                sizeof(out_report->first_issue), cycle.ids[0]);
    }
    return (out_report->missing_dependencies == 0U &&
            out_report->ambiguous_bindings == 0U &&
            out_report->cycles == 0U)
        ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}
