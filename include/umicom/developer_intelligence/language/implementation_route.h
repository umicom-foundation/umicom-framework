/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_intelligence/language/implementation_route.h
 *
 * PURPOSE:
 *   Evaluate Implementation Route readiness and policy using bounded copied signals.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_INTELLIGENCE_LANGUAGE_IMPLEMENTATION_ROUTE_H
#define UMICOM_DEVELOPER_INTELLIGENCE_LANGUAGE_IMPLEMENTATION_ROUTE_H

#include <stddef.h>
#include "umicom/developer_intelligence/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus UmiDeveloperIntelligenceEvaluateLanguageImplementationRoute(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot);


/** Evaluate a complete group using this policy without publishing partial rows.
 * Zero count is a no-op and permits NULL arrays. Nonempty groups require arrays
 * with count readable inputs and capacity writable outputs. At most 4096 rows
 * and 16 MiB of temporary output are accepted; allocation can fail.
 * Any failure preserves all outputs. Non-finite signals and overlong labels
 * are refused. Inputs may borrow output storage until publication. The caller
 * serializes access; this calculation does not submit orders or execute jobs.
 */
UmiStatus UmiDeveloperIntelligenceEvaluateLanguageImplementationRouteBatch(const UmiDeveloperIntelligenceInput *inputs,
    size_t count, UmiDeveloperIntelligenceSnapshot *outputs, size_t capacity);

#ifdef __cplusplus
}
#endif
#endif
