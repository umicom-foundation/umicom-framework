/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/summary.h
 *
 * PURPOSE:
 *   Aggregate raw CodeGuard findings into severity and category counts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */
#ifndef UMICOM_CODEGUARD_SUMMARY_H
#define UMICOM_CODEGUARD_SUMMARY_H
#include <stddef.h>
#include "umicom/codeguard/result.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiCodeGuardSummary {
    size_t total;
    size_t critical;
    size_t high;
    size_t medium;
    size_t low;
    size_t info;
    size_t memory;
    size_t buffers;
    size_t architecture;
    size_t duplicates;
} UmiCodeGuardSummary;
UmiCodeGuardSummary umi_codeguard_summary_build(const UmiCodeGuardResult *result);
int umi_codeguard_summary_failed(const UmiCodeGuardSummary *summary, UmiCodeGuardSeverity threshold);
#ifdef __cplusplus
}
#endif
#endif
