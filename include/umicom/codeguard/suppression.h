/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/suppression.h
 *
 * PURPOSE:
 *   Match explicit rule/path suppressions without hiding unrelated findings.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */
#ifndef UMICOM_CODEGUARD_SUPPRESSION_H
#define UMICOM_CODEGUARD_SUPPRESSION_H
#include "umicom/codeguard/finding.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiCodeGuardSuppression {
    const char *rule_id;
    const char *path_substring;
} UmiCodeGuardSuppression;
int umi_codeguard_suppression_matches(const UmiCodeGuardSuppression *suppression,
                                      const UmiCodeGuardFinding *finding);
#ifdef __cplusplus
}
#endif
#endif
