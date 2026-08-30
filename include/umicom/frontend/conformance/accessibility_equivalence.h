/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/accessibility_equivalence.h
 *
 * PURPOSE:
 *   accessibility capability coverage and blocker detection across frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_ACCESSIBILITY_EQUIVALENCE_H
#define UMICOM_FRONTEND_CONFORMANCE_ACCESSIBILITY_EQUIVALENCE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFcAccessibilityEquivalence { uint64_t required; uint64_t supported; } UmiFcAccessibilityEquivalence;
double umi_fc_accessibility_equivalence_evaluate(const UmiFcAccessibilityEquivalence *item);
bool umi_fc_accessibility_equivalence_has_blocker(const UmiFcAccessibilityEquivalence *item);

#ifdef __cplusplus
}
#endif
#endif
