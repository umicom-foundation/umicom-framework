/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/cycle_report.h
 *
 * PURPOSE:
 *   Extract a representative cycle path from a service dependency graph.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_CYCLE_REPORT_H
#define UMICOM_RUNTIME_BOOTSTRAP_CYCLE_REPORT_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


UmiStatus umi_bootstrap_graph_cycle_report(
    const UmiBootstrapServiceGraph *graph,
    UmiBootstrapIdList *out_cycle);

#ifdef __cplusplus
}
#endif

#endif
