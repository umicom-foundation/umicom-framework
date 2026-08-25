/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/theme_equivalence.h
 *
 * PURPOSE:
 *   semantic theme-token equivalence scoring and required-token coverage.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_THEME_EQUIVALENCE_H
#define UMICOM_FRONTEND_CONFORMANCE_THEME_EQUIVALENCE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFcThemeEquivalence { size_t required_tokens; size_t matched_tokens; size_t mismatched_tokens; } UmiFcThemeEquivalence;
double umi_fc_theme_equivalence_evaluate(const UmiFcThemeEquivalence *item);

#ifdef __cplusplus
}
#endif
#endif
