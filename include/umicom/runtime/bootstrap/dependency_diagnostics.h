/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/dependency_diagnostics.h
 *
 * PURPOSE:
 *   Aggregate graph, missing-dependency and binding ambiguity diagnostics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_DEPENDENCY_DIAGNOSTICS_H
#define UMICOM_RUNTIME_BOOTSTRAP_DEPENDENCY_DIAGNOSTICS_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


UmiStatus umi_bootstrap_dependency_diagnostics_collect(
    const UmiBootstrapServiceGraph *graph,
    const UmiBootstrapBindingRegistry *bindings,
    const UmiBootstrapServiceKey *key,
    UmiBootstrapIssueReport *out_report);

#ifdef __cplusplus
}
#endif

#endif
