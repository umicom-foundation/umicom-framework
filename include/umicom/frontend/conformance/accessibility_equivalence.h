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

/**
 * Represent the fc accessibility equivalence data shared with callers of this public
 * contract.
 */
typedef struct UmiFcAccessibilityEquivalence { uint64_t required; uint64_t supported; } UmiFcAccessibilityEquivalence;
/**
 * Provide the fc accessibility equivalence evaluate operation used by this module and its
 * client applications.
 */
double umi_fc_accessibility_equivalence_evaluate(const UmiFcAccessibilityEquivalence *item);
/**
 * Provide the fc accessibility equivalence has blocker operation used by this module and
 * its client applications.
 */
bool umi_fc_accessibility_equivalence_has_blocker(const UmiFcAccessibilityEquivalence *item);

#ifdef __cplusplus
}
#endif
#endif
