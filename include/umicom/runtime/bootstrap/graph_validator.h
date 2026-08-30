/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/graph_validator.h
 *
 * PURPOSE:
 *   Validate service graphs for missing nodes, self-dependencies and cycles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_GRAPH_VALIDATOR_H
#define UMICOM_RUNTIME_BOOTSTRAP_GRAPH_VALIDATOR_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


UmiStatus umi_bootstrap_graph_validate(
    const UmiBootstrapServiceGraph *graph,
    UmiBootstrapIssueReport *out_report);

#ifdef __cplusplus
}
#endif

#endif
