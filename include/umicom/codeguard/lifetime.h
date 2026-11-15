/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/lifetime.h
 *
 * PURPOSE:
 *   Track simple pointer lifetime events to detect obvious double-free and
 *   use-after-free patterns.
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
#ifndef UMICOM_CODEGUARD_LIFETIME_H
#define UMICOM_CODEGUARD_LIFETIME_H
#include <stddef.h>
#include "umicom/codeguard/result.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the code guard lifetime tracker data shared with callers of this public
 * contract.
 */
typedef struct UmiCodeGuardLifetimeTracker {
    char freed_names[64][96];
    size_t freed_lines[64];
    size_t count;
} UmiCodeGuardLifetimeTracker;
/**
 * Initialise codeguard lifetime from caller-provided values so later operations receive a
 * known state.
 */
void umi_codeguard_lifetime_init(UmiCodeGuardLifetimeTracker *tracker);
/**
 * Provide the codeguard lifetime scan operation used by this module and its client
 * applications.
 */
void umi_codeguard_lifetime_scan(UmiCodeGuardLifetimeTracker *tracker,
                                 const char *path, size_t line_number,
                                 const char *code_line, UmiCodeGuardResult *result);
/* Checked analysis preserves tracker state on failure and reports finding
 * insertion errors or exhaustion of 64 simultaneously tracked names.
 * Cleared slots can be reused. Result insertion is append-only and may be
 * partial on failure: discard the failed scan candidate rather than treating
 * it as complete. This remains a textual heuristic, not control-flow proof.
 * The historical void function remains available and discards this status. */
UmiStatus umi_codeguard_lifetime_scan_checked(UmiCodeGuardLifetimeTracker *tracker,
    const char *path, size_t line_number, const char *code_line,
    UmiCodeGuardResult *result);
#ifdef __cplusplus
}
#endif
#endif
