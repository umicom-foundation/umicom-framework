/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/missing_dependency.h
 *
 * PURPOSE:
 *   Count required graph dependencies whose service node is absent or disabled.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_MISSING_DEPENDENCY_H
#define UMICOM_RUNTIME_BOOTSTRAP_MISSING_DEPENDENCY_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Return the number of records represented by bootstrap graph missing dependency without
 * changing their state.
 */
size_t umi_bootstrap_graph_missing_dependency_count(
    const UmiBootstrapServiceGraph *graph,
    char *out_first_missing,
    size_t capacity);

#ifdef __cplusplus
}
#endif

#endif
