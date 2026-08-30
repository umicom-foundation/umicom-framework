/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/ambiguity_report.h
 *
 * PURPOSE:
 *   Detect equally-ranked bindings that make a service resolution ambiguous.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_AMBIGUITY_REPORT_H
#define UMICOM_RUNTIME_BOOTSTRAP_AMBIGUITY_REPORT_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


size_t umi_bootstrap_binding_ambiguity_count(
    const UmiBootstrapBindingRegistry *registry,
    const UmiBootstrapServiceKey *key);

#ifdef __cplusplus
}
#endif

#endif
